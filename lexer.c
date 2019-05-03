#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "9cc.h"

Token *new_token(int ty, char *input) {
    Token *token = malloc(sizeof(Token));
    token->ty = ty;
    token->input = input;
    return token;
}

Token *new_token_num(int ty, int val, char *input) {
    Token *token = malloc(sizeof(Token));
    token->ty = ty;
    token->val = val;
    token->input = input;
    return token;
}

Token *new_token_ident(char *name, char *input) {
    Token *token = malloc(sizeof(Token));
    token->ty = TK_IDENT;
    token->name = name;
    token->input = input;
    return token;
}

Vector *tokenize(char *p) {
    Vector *tokens = new_vector();
    while (*p) {
        // skip spaces
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !isalnum(p[6])) {
            vec_push(tokens, new_token(TK_RETURN, p));
            p += 6;
            continue;
        }

        if (strncmp(p, ">=", 2) == 0) {
            vec_push(tokens, new_token(TK_GE, p));
            p += 2;
            continue;
        }

        if (strncmp(p, ">", 1) == 0) {
            vec_push(tokens, new_token(TK_GT, p));
            p += 1;
            continue;
        }

        if (strncmp(p, "<=", 2) == 0) {
            vec_push(tokens, new_token(TK_LE, p));
            p += 2;
            continue;
        }

        if (strncmp(p, "<", 1) == 0) {
            vec_push(tokens, new_token(TK_LT, p));
            p += 1;
            continue;
        }

        if (strncmp(p, "==", 2) == 0) {
            vec_push(tokens, new_token(TK_EQ, p));
            p += 2;
            continue;
        }

        if (strncmp(p, "!=", 2) == 0) {
            vec_push(tokens, new_token(TK_NE, p));
            p += 2;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
            *p == ')' || *p == '=' || *p == ';' || *p == '{' || *p == '}') {
            vec_push(tokens, new_token(*p, p));
            p++;
            continue;
        }

        if (isdigit(*p)) {
            char *input = p;
            vec_push(tokens, new_token_num(TK_NUM, strtol(p, &p, 10), input));
            continue;
        }

        if (isalpha(*p)) {
            int len = 1;
            while (isalnum(*(p + len))) len++;

            char *name = malloc(sizeof(char) * (len + 1));
            strncpy(name, p, len);
            name[len] = '\0';
            vec_push(tokens, new_token_ident(name, p));
            p += len;
            continue;
        }

        error("トークナイズできません: %s", p);
        exit(1);
    }

    vec_push(tokens, new_token(TK_EOF, p));
    return tokens;
}
