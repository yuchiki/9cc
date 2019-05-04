#include <stdio.h>
#include <stdlib.h>

#include "9cc.h"

int unique_id = 0;
int gen_unique_id() { return unique_id++; }

void gen_lval(Node *node, Map *variables) { // push the address
    if (node->ty != ND_IDENT) {
        error("代入の左辺値が変数ではありません");
    }

    long offset = (long)map_get(variables, node->name);

    if (offset == 0) {
        offset = (variables->keys->len + 1) * 8;
        map_put(variables, node->name, (void *)offset);

        if (variables->keys->len > MAX_VARIABLES) {
            error("too many variables. only %d variables are allowed in a "
                  "function.\n",
                  MAX_VARIABLES);
        }
    }

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
        printf("    jmp .Lwhile%d\n", id);
        printf(".Lwhend%d:\n", id);
        return;
    }

    if (node->ty == ND_BLOCK) {
        for (int i = 0; i < node->statements->len; i++) {
            gen(node->statements->data[i], variables);
            printf("    pop rax\n");
        }
        return;
        printf("    push rax\n");
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
