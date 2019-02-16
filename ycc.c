//#include <ycc.h>
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


enum {
  ND_NUM = 256,     // Type of integer nodes
};

typedef struct Node {
  int ty;           // operator or ND_NUM
  struct Node *lhs; // left side
  struct Node *rhs; // right side
  int val;          // if ty is ND_NUM
} Node;

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

Vector *new_vector()
{
  Vector *vec = malloc(sizeof(Vector));
  vec->data = malloc(sizeof(void *) * 16);
  vec->capacity = 16;
  vec->len = 0;
  return vec;
}

void vec_push(Vector *vec, void *elem){
  if (vec->capacity == vec->len){
    vec->capacity *= 2;
    vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
  }
  vec->data[vec->len++] = elem;
}

// array of tokens after tokenize
// no more than 100 tokens 
Token tokens[100];

// report error
void error(char message[], char *p) {
  fprintf(stderr, "%s: %s\n", message, p);
  exit(1);
}

int pos = 0;

Node *new_node(int ty, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

// check next token is expected or not
int consume(int ty) {
  if (tokens[pos].ty != ty)
    return 0;
  pos++;
  return 1;
}

// parser
Node *mul();

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume('+'))
      node = new_node('+', node, mul());
    else if (consume('-'))
      node = new_node('-', node, mul());
    else
      return node;
  }
}

Node *term() {
  if (consume('(')) {
    Node *node = add();
    if (!consume(')'))
      error("Couldn't find close parentheses", tokens[pos].input);
    return node;
  }

  if (tokens[pos].ty == TK_NUM)
    return new_node_num(tokens[pos++].val);

  error("The token is neither number nor parentheses", tokens[pos].input);
}

Node *mul() {
  Node *node = term();

  for (;;) {
    if (consume('*'))
      node = new_node('*', node, term());
    else if (consume('/'))
      node = new_node('/', node, term());
    else
      return node;
  }
}


// use stack
void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
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


void tokenize(char *p){
  int i = 0;
  while (*p){
    // skip empty str
    if (isspace(*p)){
      p++;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'){
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

//-------------------------
// Unittest for structure
//-------------------------
void expect(int line, int expected, int actual){
  if (expected == actual)
    return;
  fprintf(stderr, "%d: %d expected, but got %d\n", 
    line, expected, actual);
  exit(1);
}

void runtest(){
  Vector *vec = new_vector();
  expect(__LINE__, 0, vec->len);

  for (int i = 0; i < 100; i++){
    vec_push(vec, (void *)i); // elem=i
  }

  expect(__LINE__, 100, vec->len);
  expect(__LINE__, 0, (int)vec->data[0]);
  expect(__LINE__, 50, (int)vec->data[50]);
  expect(__LINE__, 99, (int)vec->data[99]);

  printf("OK\n");
}

//-------------------------
// ending unittest
//-------------------------

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
  Node *node = add();

  // output first half of assembly
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // generate codes
  gen(node);

  printf(" pop rax\n");
  printf(" ret\n");
  return 0;
}
