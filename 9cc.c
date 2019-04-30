#include<ctype.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#define MAX_TOKENS 100

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
    TK_EOF
};

typedef struct Token {
    int ty;      // type of token
    int val;
    char* input; // (for error message)
} Token;

Token tokens[MAX_TOKENS]; // tokenized tokens
int pos = 0;

/////////////////// nodes

enum {
    ND_NUM = 256,
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

Node *add();

Node *term() {
    if (consume('(')) {
        Node *node = add();
        if(!consume(')')) {
            error("開き括弧に対応する閉じ括弧がありません:%s\n", tokens[pos].input);
        }

        return node;
    }

    if (tokens[pos].ty == TK_NUM) {
        return new_node_num(tokens[pos++].val);
    }

    error("開き括弧でも数値でもないトークンです: %s\n", tokens[pos].input);
}

Node *mul() {
    Node *node = term();

    for(;;) {
        if (consume('*')) {
            node = new_node('*', node, term());
        } else if (consume('/')) {
            node = new_node('/', node, term());
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
    }

    printf(" push rax\n");
}

void tokenize(char *p) {
    int i = 0; //first uninitialized token
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

    tokenize(argv[1]);
    Node *node = add();

    // output header
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}
