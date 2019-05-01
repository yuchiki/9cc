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

Node *new_node_ident(char *input) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->val = *input;
    return node;
}

int pos = 0;

int consume(int ty) {
    if ((((Token **)(tokens->data))[pos])->ty != ty)
        return 0;
    pos++;
    return 1;
}

Node *expr();

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
        return new_node_ident((((Token **)(tokens->data))[pos++])->input);
    }

    error("開き括弧でも数値でもないトークンです: %s\n",
          (((Token **)(tokens->data))[pos])->input);

    return 0; // unreachable here. This line exists to suppress warnings.
}

Node *unary() {
    if (consume('+'))
        return term();
    if (consume('-'))
        return new_node('-', new_node_num(0), term());
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

Node *expr() { return equality(); }

Node *parse(Vector *tokenized_tokens) {
    tokens = tokenized_tokens;
    return expr();
}
