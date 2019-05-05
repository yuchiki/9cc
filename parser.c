#include <stdio.h>
#include <stdlib.h>

#include "9cc.h"

Vector *tokens;

Node *new_node(int ty, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

Node *new_node_ident(char *name) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->name = name;
    return node;
}

Node *new_node_block(Vector *statements) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_BLOCK;
    node->statements = statements;
    return node;
}

Node *new_node_ifelse(Node *cond_stmt, Node *then_stmt, Node *else_stmt) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IFELSE;
    node->cond_statement = cond_stmt;
    node->then_statement = then_stmt;
    node->else_statement = else_stmt;
    return node;
}

Node *new_node_while(Node *cond_stmt, Node *then_stmt) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_WHILE;
    node->cond_statement = cond_stmt;
    node->then_statement = then_stmt;
    return node;
}

Node *new_node_for(Node *init_stmt, Node *cond_stmt, Node *loop_stmt,
                   Node *then_stmt) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_FOR;
    node->init_statement = init_stmt;
    node->cond_statement = cond_stmt;
    node->loop_statement = loop_stmt;
    node->then_statement = then_stmt;
    return node;
}

Node *new_node_call(char *name, Vector *arguments) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_CALL;
    node->name = name;
    node->arguments = arguments;
    return node;
}

Function_Definition *new_function(char *name, Vector *arguments, Node *body) {
    Function_Definition *definition = malloc(sizeof(Function_Definition));
    definition->name = name;
    definition->arguments = arguments;
    definition->body = body;
    return definition;
}

int pos = 0;

int consume(int ty) {
    if ((((Token **)(tokens->data))[pos])->ty != ty) return 0;
    pos++;
    return 1;
}

void must_consume(int ty) {
    if (!consume(ty))
        error("%d:'%c'ではないトークンです: %s", ty, (char)ty,
              (((Token **)(tokens->data))[pos])->input);
}

Node *expr();

char *ident() {
    if ((((Token **)(tokens->data))[pos])->ty != TK_IDENT)
        error("This is not an identifier:%s\n",
              ((Token *)tokens->data[pos])->input);
    return (((Token **)(tokens->data))[pos++])->name;
}

Node *term() {
    if (consume('(')) {
        Node *node = expr();
        if (!consume(')')) {
            error("開き括弧に対応する閉じ括弧がありません:%s\n",
                  (((Token **)(tokens->data))[pos])->input);
        }

        return node;
    }

    if ((((Token **)(tokens->data))[pos])->ty == TK_NUM) {
        return new_node_num((((Token **)(tokens->data))[pos++])->val);
    }

    if ((((Token **)(tokens->data))[pos])->ty == TK_IDENT) {
        char *name = ident();

        if (consume('(')) {
            Vector *arguments = new_vector();
            if (consume(')')) {
                return new_node_call(name, arguments);
            }
            vec_push(arguments, expr());

            while (consume(',')) {
                vec_push(arguments, expr());
            }
            must_consume(')');
            return new_node_call(name, arguments);
        }

        return new_node_ident(name);
    }

    error("開き括弧でも数値でもないトークンです: %s\n",
          (((Token **)(tokens->data))[pos])->input);

    return 0; // unreachable here. This line exists to suppress warnings.
}

Node *unary() {
    if (consume('+')) return term();
    if (consume('-')) return new_node('-', new_node_num(0), term());
    return term();
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume('*')) {
            node = new_node('*', node, unary());
        } else if (consume('/')) {
            node = new_node('/', node, unary());
        } else {
            return node;
        }
    }
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume('+')) {
            node = new_node('+', node, mul());
        } else if (consume('-')) {
            node = new_node('-', node, mul());
        } else {
            return node;
        }
    }
}

Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume(TK_LE)) {
            node = new_node(ND_LE, node, add());
        } else if (consume(TK_GE)) {
            node = new_node(ND_LE, add(), node);
        } else if (consume(TK_LT)) {
            node = new_node(ND_LT, node, add());
        } else if (consume(TK_GT)) {
            node = new_node(ND_LT, add(), node);
        } else {
            return node;
        }
    }
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume(TK_EQ)) {
            node = new_node(ND_EQ, node, relational());
        } else if (consume(TK_NE)) {
            node = new_node(ND_NE, node, relational());
        } else {
            return node;
        }
    }
}

Node *assign() {
    Node *node = equality();

    while (consume('=')) {
        node = new_node('=', node, assign());
    }
    return node;
}

Node *expr() { return assign(); }

Node *stmt() {
    if (consume('{')) {
        Vector *statements = new_vector();
        while (!consume('}')) vec_push(statements, stmt());
        return new_node_block(statements);
    }

    if (consume(TK_IF)) {
        must_consume('(');
        Node *cond_stmt = expr();
        must_consume(')');
        Node *then_stmt = stmt();
        Node *else_stmt;
        if (consume(TK_ELSE)) {
            else_stmt = stmt();
        } else {
            else_stmt = new_node_block(new_vector());
        }
        return new_node_ifelse(cond_stmt, then_stmt, else_stmt);
    }

    if (consume(TK_WHILE)) {
        must_consume('(');
        Node *cond_stmt = expr();
        must_consume(')');
        Node *then_stmt = stmt();
        return new_node_while(cond_stmt, then_stmt);
    }

    if (consume(TK_FOR)) {
        must_consume('(');
        Node *init_stmt = expr();
        must_consume(';');
        Node *cond_stmt = expr();
        must_consume(';');
        Node *loop_stmt = expr();
        must_consume(')');
        Node *then_stmt = stmt();

        return new_node_for(init_stmt, cond_stmt, loop_stmt, then_stmt);
    }

    Node *node;

    if (consume(TK_RETURN)) {
        node = new_node(ND_RETURN, assign(), NULL);
    } else {
        node = assign();
    }

    if (!consume(';'))
        error("';'ではないトークンです: %s",
              (((Token **)(tokens->data))[pos])->input);
    return node;
}

Vector *definition_argument_list() {
    must_consume('(');
    Vector *arguments = new_vector();
    if (consume(')')) {
        return arguments;
    }
    vec_push(arguments, ident());

    while (consume(',')) {
        vec_push(arguments, ident());
    }
    must_consume(')');
    return arguments;
}

Function_Definition *function_definition() {
    if (((Token *)(tokens->data[pos]))->ty != TK_IDENT)
        error("function does not start from identifier: %s\n",
              ((Token *)(tokens->data[pos]))->input);
    char *name = (((Token **)(tokens->data))[pos++])->name;

    Vector *arguments = definition_argument_list();

    must_consume('{');
    Vector *statements = new_vector();
    while (!consume('}')) vec_push(statements, stmt());
    return new_function(name, arguments, new_node_block(statements));
}

Node *code[100];

void program() {
    int i = 0;
    while ((((Token **)(tokens->data))[pos])->ty != TK_EOF) {
        code[i++] = stmt();
    }
    code[i] = NULL;
}

Function_Definition *parse(Vector *tokenized_tokens) {
    tokens = tokenized_tokens;
    return function_definition();
}
