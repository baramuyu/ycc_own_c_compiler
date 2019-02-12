#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// number that represents type of token
enum {
  TK_NUM = 256,
  TK_EOF,
};

// type of token
typedef struct {
  int ty;       // type of token
  int val;      // if ty is TK_NUM, its number
  char *input;  // token str for error message 
} Token;

// array of tokens after tokenize
// no more than 100 tokens 
Token tokens[100];


void tokenize(char *p){
  int i = 0;
  while (*p){
    // skip empty str
    if (isspace(*p)){
      p++;
      continue;
    }

    if (*p == '+' || *p == '-'){
      tokens[i].ty = *p;
      tokens[i].input = p;  // why here is not pointer?
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)){
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    fprintf(stderr, "couldn't tokenize: %s\n", p);
    exit(1);
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

void error(int i){
  fprintf(stderr, "unexpected token: %s\n",
    tokens[i].input);
  exit(1);
}

int main(int argc, char **argv){
	if (argc != 2){
		fprintf(stderr, "number of arguments is incorrect.");
		return 1;
	}

  // tokenize
  tokenize(argv[1]);

  // output first half of assembly
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // first string has to be a number
  // output first 'mov'
  if (tokens[0].ty != TK_NUM)
    error(0);
  printf(" mov rax, %d\n", tokens[0].val);

  // read '+ (number)' or '- (number)',
  // output assembly at the same time
  int i = 1;
  while(tokens[i].ty != TK_EOF) {
    if (tokens[i].ty == '+'){
      i++;
      if(tokens[i].ty != TK_NUM) 
        // current string is not number
        error(i);
      printf(" add rax, %d\n", tokens[i].val);
      i++;
      continue;
    }

    if(tokens[i].ty == '-'){
      i++;
      if(tokens[i].ty != TK_NUM)
        // current string is not number
        error(i);
      printf(" sub rax, %d\n", tokens[i].val);
      i++;
      continue;
    }

    error(i);

  }

  printf(" ret\n");
  return 0;
}