#include "ycc.h"

void gen_lval(Node *node){
  if (node->ty != ND_IDENT){
    error("Lvalue is not a variable", NULL);
  }
  int offset = ('z' - node->name +1) * 8; // here has to fix for multi-string variables
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", offset);
  printf("  push rax\n");
}

// use stack
void gen(Node *node) {
  // number
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  // variable?
  if (node->ty == ND_IDENT){
    gen_lval(node); //RAX is stack top
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n"); // push address of RAX
    return;
  }

  if (node->ty == '='){
    gen_lval(node->lhs); //RAX is stack top
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n"); // push address of RDI
    return;
  }

  gen(node->lhs);
  gen(node->rhs);
  printf("  pop rdi\n");
  printf("  pop rax\n");


  switch (node->ty){
  case '+':
    printf("  add rax, rdi\n");
    break;
  case '-':
    printf("  sub rax, rdi\n");
    break;
  case '*':
    printf("  mul rdi\n");
    break;
  case '/':
    printf("  mov rdx, 0\n");
    printf("  div rdi\n");
  }

  printf("  push rax\n");
}
