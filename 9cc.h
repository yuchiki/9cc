

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

Vector *tokenize(char *p);
