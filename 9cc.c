#include<ctype.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#define MAX_TOKENS 100

// type of token
enum {
    TK_NUM = 256,
    TK_EOF
};

typedef struct {
    int ty;      // type of token
    int val;
    char* input // (for error message)
} Token;

Token tokens[MAX_TOKENS]; // tokenized tokens

void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void tokenize(char *p) {
    int i = 0; //first uninitialized token
    while(*p) {
        // skip spaces
        if (isspace(*p)) {
            p++;
            continue;
        }

        if(*p == '+' || *p == '-') {
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

    // output header
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");


    if (tokens[0].ty != TK_NUM)
        error("最初の項が数ではありません。");

    printf("    mov rax, %d\n", tokens[0].val);

    int i = 1;
    while(tokens[i].ty != TK_EOF) {
        if (tokens[i].ty == '+') {
            i++;
            if(tokens[i].ty != TK_NUM)
                error("unexpected token: %s", tokens[i].input);
            printf("    add rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        if (tokens[i].ty == '-') {
            i++;
            if(tokens[i].ty != TK_NUM)
                error("unexpected token: %s\n", tokens[i].input);
            printf("    sub rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        error("予期せぬトークンです： %s\n", tokens[i].input);
        return 1;
    }
    printf("    ret\n");
    return 0;
}
