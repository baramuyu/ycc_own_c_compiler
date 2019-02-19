#include "ycc.h"

int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "number of arguments is incorrect.");
    return 1;
  }

  if (strcmp(argv[1],"-test") == 0){
    runtest();
    return 0;
  }

  // tokenize
  tokenize(argv[1]);
  program();
  
  // output first half of assembly
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // prologue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n"); // 26 alphabet * 8 bite

  // generate codes
  for (int i = 0; code[i]; i++){
    gen(code[i]);

    // should be one number left in the stack
    printf("  pop rax\n");
  }

  // epilogue
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
