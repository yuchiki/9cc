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

Token *new_token_string(char *string, char *input) {
    Token *token = malloc(sizeof(Token));
    token->ty = TK_STRING;
    token->string = string;
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

        if (*p == '"') {
            p++;
            int len = 0;
            while (*(p + len) != '"') len++;

            char *string = malloc(len + 1);
            strncpy(string, p, len);
            string[len] = '\0';
            vec_push(tokens, new_token_string(string, p - 1));
            p += len + 1;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !isalnum(p[6])) {
            vec_push(tokens, new_token(TK_RETURN, p));
            p += 6;
            continue;
        }

        if (strncmp(p, "if", 2) == 0 && !isalnum(p[2])) {
            vec_push(tokens, new_token(TK_IF, p));
            p += 2;
            continue;
        }

        if (strncmp(p, "else", 4) == 0 && !isalnum(p[4])) {
            vec_push(tokens, new_token(TK_ELSE, p));
            p += 4;
            continue;
        }

        if (strncmp(p, "while", 5) == 0 && !isalnum(p[5])) {
            vec_push(tokens, new_token(TK_WHILE, p));
            p += 5;
            continue;
        }

        if (strncmp(p, "for", 3) == 0 && !isalnum(p[3])) {
            vec_push(tokens, new_token(TK_FOR, p));
            p += 3;
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
            *p == ')' || *p == '=' || *p == ';' || *p == '{' || *p == '}' ||
            *p == ',') {
            vec_push(tokens, new_token(*p, p));
            p++;
            continue;
        }

        if (isdigit(*p)) {
            char *input = p;
            vec_push(tokens, new_token_num(TK_NUM, strtol(p, &p, 10), input));
            continue;
        }

        // ident
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
