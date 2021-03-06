#include "9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////////////////////////// util

void error(char *fmt, ...) {
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

    Vector *tokens = tokenize(argv[1]);
    Vector *functions = parse(tokens);

    // output header
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");

    for (int i = 0; i < functions->len; i++) function_gen(functions->data[i]);

    return 0;
}
