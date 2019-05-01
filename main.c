#include<ctype.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include"9cc.h"

////////////////////////// util

void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

///////////////// assembly

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の数が不正です\n");
        return 1;
    }

    if (strncmp(argv[1], "-test", 5) == 0) {
        runtest();
        return 0;
    }

    tokens = tokenize(argv[1]);

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
