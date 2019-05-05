
#ifndef NINECC_H
#define NINECC_H

//////////////////////////// util

void error(char *fmt, ...);

/////////////////////////// vector

// MAX_VARIABLES must be odd.
// if even, the function call gets broken.
// in the ABI, sbp must be a multiple of 16 when do call.
#define MAX_VARIABLES 9

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vector();
void vec_push(Vector *vec, void *elem);

////////////////////////// map

typedef struct {
    Vector *keys;
    Vector *vals;
} Map;

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);

////////////////////////// container test

void runtest();

////////////////////////// token

// type of token
enum {
    TK_NUM = 256,
    TK_IDENT,
    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_LT,
    TK_GT,
    TK_EOF
};

typedef struct {
    int ty;      // type of token
    int val;     // used when TK_NUM
    char *name;  // used when TK_IDENT
    char *input; // (for error message)
} Token;

Vector *tokenize(char *p);

/////////////////// nodes

enum {
    ND_NUM = 256,
    ND_IDENT,
    ND_CALL,
    ND_RETURN,
    ND_BLOCK,
    ND_IFELSE,
    ND_WHILE,
    ND_FOR,
    ND_EQ,
    ND_NE,
    ND_LE,
    ND_LT,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;                     // only used when ND_NUM
    char *name;                  // only used when ND_IDENT, CALL
    Vector *statements;          // only used when ND_BLOCK
    Vector *arguments;           // only used when ND_CALL
    struct Node *cond_statement; // only used when ND_IFELSE, WHILE, FOR
    struct Node *then_statement; // only used when ND_IFELSE, WHILE, FOR
    struct Node *else_statement; // only used when ND_IFELSE
    struct Node *init_statement; // only used when ND_FOR
    struct Node *loop_statement; // only used when ND_FOR
} Node;

typedef struct {
    char *name;
    Vector *arguments;
    Node *body;
} Function_Definition;

Function_Definition *parse(Vector *tokenized_tokens);

extern Node *code[100];

////////////////////// codegen

void gen(Node *node, Map *variables);

void function_gen(Function_Definition *function);

#endif // NINECC_H
