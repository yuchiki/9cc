#include <stdio.h>
#include <stdlib.h>

#include "9cc.h"

int unique_id = 0;
int gen_unique_id() { return unique_id++; }

long push_variable(Map *variables, char *name) {
    long offset = (variables->keys->len + 1) * 8;
    map_put(variables, name, (void *)offset);

    if (variables->keys->len > MAX_VARIABLES) {
        error("too many variables. only %d variables are allowed in a "
              "function.\n",
              MAX_VARIABLES);
    }
    return offset;
}

void gen_lval(Node *node, Map *variables) { // push the address
    if (node->ty != ND_IDENT) {
        error("代入の左辺値が変数ではありません");
    }

    long offset = (long)map_get(variables, node->name);

    if (offset == 0) offset = push_variable(variables, node->name);

    printf("    mov rax, rbp\n");
    printf("    sub rax, %ld\n", offset);
    printf("    push rax\n");
}

void gen(Node *node, Map *variables) {

    if (node->ty == ND_NUM) {
        printf("    push %d\n", node->val);
        return;
    }

    if (node->ty == ND_IDENT) {
        gen_lval(node, variables);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    }

    if (node->ty == ND_CALL) {
        int stack_depth = 0; // this is temporary.

        if (node->arguments->len > 6)
            error("too many arguments for %s\n", node->name);

        for (int i = 0; i < node->arguments->len; i++)
            gen(node->arguments->data[i], variables);

        if (node->arguments->len >= 6) printf("    pop r9\n");
        if (node->arguments->len >= 5) printf("    pop r8\n");
        if (node->arguments->len >= 4) printf("    pop rcx\n");
        if (node->arguments->len >= 3) printf("    pop rdx\n");
        if (node->arguments->len >= 2) printf("    pop rsi\n");
        if (node->arguments->len >= 1) printf("    pop rdi\n");

        if (stack_depth % 2 == 1) {
            printf("    push rax\n");
            printf("    call %s\n", node->name);
            printf("    pop rax\n");
        } else {
            printf("    call %s\n", node->name);
        }
        printf("    push rax\n");

        return;
    }

    if (node->ty == ND_RETURN) {
        gen(node->lhs, variables);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    }

    if (node->ty == ND_IFELSE) {
        int id = gen_unique_id();
        gen(node->cond_statement, variables);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lelse%d\n", id);
        gen(node->then_statement, variables);
        printf("    jmp .Lfi%d\n", id);
        printf(".Lelse%d:\n", id);
        gen(node->else_statement, variables);
        printf(".Lfi%d:\n", id);

        return;
    }

    if (node->ty == ND_WHILE) {
        int id = gen_unique_id();
        printf(".Lwhile%d:\n", id);
        gen(node->cond_statement, variables);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lwhend%d\n", id);
        gen(node->then_statement, variables);
        printf("    pop rax\n");
        printf("    jmp .Lwhile%d\n", id);
        printf(".Lwhend%d:\n", id);
        return;
    }

    if (node->ty == ND_FOR) {
        int id = gen_unique_id();
        gen(node->init_statement, variables);
        printf("    pop rax\n");
        printf(".Lfor%d:\n", id);
        gen(node->cond_statement, variables);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lforend%d\n", id);
        gen(node->then_statement, variables);
        printf("    pop rax\n");
        gen(node->loop_statement, variables);
        printf("    pop rax\n");
        printf("    jmp .Lfor%d\n", id);
        printf(".Lforend%d:\n", id);
        printf("    push rax\n");
        return;
    }

    if (node->ty == ND_BLOCK) {
        for (int i = 0; i < node->statements->len; i++) {
            gen(node->statements->data[i], variables);
            printf("    pop rax\n");
        }
        printf("    push rax\n");
        return;
    }

    if (node->ty == '=') {
        gen_lval(node->lhs, variables);
        gen(node->rhs, variables);

        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return;
    }

    gen(node->lhs, variables);
    gen(node->rhs, variables);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->ty) {
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
    printf("    push rax\n");
}

void function_gen(Function_Definition *function) {

    printf("%s:\n", function->name);
    // prologue
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n", 8 * MAX_VARIABLES);

    if (function->arguments->len >= 6) printf("    mov [rbp-48], r9\n");
    if (function->arguments->len >= 5) printf("    mov [rbp-40], r8\n");
    if (function->arguments->len >= 4) printf("    mov [rbp-32], rcx\n");
    if (function->arguments->len >= 3) printf("    mov [rbp-24], rdx\n");
    if (function->arguments->len >= 2) printf("    mov [rbp-16], rsi\n");
    if (function->arguments->len >= 1) printf("    mov [rbp-8], rdi\n");

    gen(function->body, new_map());

    printf("    pop rax\n");
    // epilogue
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
}
