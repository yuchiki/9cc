
#ifndef NINECC_H
#define NINECC_H

//////////////////////////// util

void error(char *fmt, ...);

/////////////////////////// vector

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vector();
void vec_push(Vector *vec, void *elem);

////////////////////////// vector test

void runtest();

////////////////////////// token

// type of token
enum {
    TK_NUM = 256,
    TK_IDENT,
    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_LT,
    TK_GT,
    TK_EOF
};

typedef struct Token {
    int ty; // type of token
    int val;
    char *input; // (for error message)
} Token;

Vector *tokenize(char *p);

/////////////////// nodes

enum {
    ND_NUM = 256,
    ND_IDENT,
    ND_EQ,
    ND_NE,
    ND_LE,
    ND_LT,
};

typedef struct Node {
    int ty; // operator or ND_NUM;
    struct Node *lhs;
    struct Node *rhs;
    int val; // ty
} Node;

Node *parse(Vector *tokenized_tokens);

////////////////////// codegen

void gen(Node *node);

#endif // NINECC_H
