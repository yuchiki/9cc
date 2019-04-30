#include<ctype.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#define MAX_TOKENS 10000

//////////////////////// vector

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vector() {
    Vector *vec = malloc(sizeof(Vector));
    vec->data = malloc(sizeof(void *) * 16);
    vec->capacity = 16;
    vec->len = 0;
    return vec;
}

void vec_push(Vector *vec, void *elem) {
    if (vec->capacity == vec->len) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
    }
    vec->data[vec->len++] = elem;
}


////////////////////////// vector test

void expect(int line, int expected, int actual) {
    if (expected == actual) return ;
    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
}

void runtest() {
    Vector *vec =new_vector();
    expect(__LINE__, 0, vec->len);

    for(int i = 0; i < 100; i++) vec_push(vec, (void *)i);

    expect(__LINE__, 100, vec->len);
    expect(__LINE__, 0, (long)vec->data[0]);
    expect(__LINE__, 50, (long)vec->data[50]);
    expect(__LINE__, 99, (long)vec->data[99]);

    printf("OK\n");
}

////////////////////////// util

void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

///////////////////// tokens

// type of token
enum {
    TK_NUM = 256,
    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_LT,
    TK_GT,
    TK_EOF
};

typedef struct Token {
    int ty;      // type of token
    int val;
    char* input; // (for error message)
} Token;

Token *new_token(int ty, int input){
    Token *token = malloc(sizeof(Token));
    token->ty = ty;
    token->input = input;
    return token;
}

Token *new_token_num(int ty, int val, int input) {
    Token *token = malloc(sizeof(Token));
    token->ty = ty;
    token->val = val;
    token->input = input;
    return token;
}

Token tokens[MAX_TOKENS]; // tokenized tokens
int pos = 0;

/////////////////// nodes

enum {
    ND_NUM = 256,
    ND_EQ,
    ND_NE,
    ND_LE,
    ND_LT,
};

typedef struct Node {
    int ty;           // operator or ND_NUM;
    struct Node *lhs;
    struct Node *rhs;
    int val;          // ty
} Node;

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

///////////// parser

int consume(int ty) {
    if (tokens[pos].ty != ty) return 0;
    pos++;
    return 1;
}

Node *expr();

Node *term() {
    if (consume('(')) {
        Node *node = expr();
        if(!consume(')')) {
            error("開き括弧に対応する閉じ括弧がありません:%s\n", tokens[pos].input);
        }

        return node;
    }

    if (tokens[pos].ty == TK_NUM) {
        return new_node_num(tokens[pos++].val);
    }

    error("開き括弧でも数値でもない    Vector *tokens = new_vector();
ークンです: %s\n", tokens[pos].input);
}

Node *unary() {
    if (consume('+')) return term();    Vector *tokens = new_vector();

    if (consume('-')) return new_node('-', new_node_num(0), term());
    return term();
}

Node *mul() {
    Node *node = unary();

    for(;;) {
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

    for(;;) {
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

    for(;;) {
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

    for(;;) {
        if (consume(TK_EQ)) {
            node = new_node(ND_EQ, node, relational());
        } else if (consume(TK_NE)) {
            node = new_node(ND_NE, node, relational());
        } else {
            return node;
        }
    }
}

Node *expr() {
    return equality();
}


///////////////// assembly

void gen(Node *node) {
    if(node->ty == ND_NUM) {
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->ty)
    {
    case '+':
        printf("    add rax, rdi\n");
        break;
    case '-':
        printf("    sub rax, rdi\n");
        break;
    case '*':
        printf("    mul rdi\n");
        break;
    case '/':
        printf("    mov rdx, 0\n");
        printf("    div rdi\n");
        break;
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;

    }

    printf(" push rax\n");
}

void tokenize(char *p) {
    while(*p) {
        // skip spaces
        if (isspace(*p)) {
            p++;
            continue;
        }

        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if(strncmp(p,">=",2) == 0) {
            tokens[i].ty = TK_GE;
            tokens[i].input = p;
            i++;
            p+= 2;
            continue;
        }

        if(strncmp(p,">",1) == 0) {
            tokens[i].ty = TK_GT;
            tokens[i].input = p;
            i++;
            p+= 1;
            continue;
        }

        if(strncmp(p,"<=",2) == 0) {
            tokens[i].ty = TK_LE;
            tokens[i].input = p;
            i++;
            p+= 2;
            continue;
        }

        if(strncmp(p,"<",1) == 0) {
            tokens[i].ty = TK_LT;
            tokens[i].input = p;
            i++;
            p+= 1;
            continue;
        }

        if(strncmp(p,"==",2) == 0) {
            tokens[i].ty = TK_EQ;
            tokens[i].input = p;
            i++;
            p+= 2;
            continue;
        }

        if(strncmp(p,"!=",2) == 0) {
            tokens[i].ty = TK_NE;
            tokens[i].input = p;
            i++;
            p+= 2;
            continue;
        }

        if(isdigit(*p)) {
            tokens[i].ty =TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        error("トークナイズできません。: %s", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の数が不正です\n");
        return 1;
    }

    if (strncmp(argv[1], "-test", 5) == 0) {
        runtest();
        return 0;
    }


    tokenize(argv[1]);
    Node *node = expr();

    // output header
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}
