//#include <ycc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// number that represents type of token
enum {
  TK_NUM = 256, // integer token
  TK_IDENT = 257,     // idetifier
  TK_EOF,       // end of token
};

enum {
  ND_NUM = 256,     // type of integer node
  ND_IDENT = 257,         // type of identifier node; 257
};

// type of token
typedef struct {
  int ty;       // type of token
  int val;      // if ty is TK_NUM, its number
  char *input;  // token str for error message 
} Token;


typedef struct Node {
  int ty;           // operator or ND_NUM
  struct Node *lhs; // left side
  struct Node *rhs; // right side
  int val;          // if ty is ND_NUM
  //char name;      // if ty is ND_IDENT <-- didn't use
} Node;

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

typedef struct {
  Vector *keys;
  Vector *vals;
} Map;

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

Map *new_map(){
  Map *map = malloc(sizeof(Map));
  map->keys = new_vector();
  map->vals = new_vector();
  return map;
}

void map_put(Map *map, char *key, void *val){
  vec_push(map->keys, key);
  vec_push(map->vals, val);
}

void *map_get(Map *map, char *key){
  for (int i = map->keys->len - 1; i >= 0; i--){
    if (strcmp(map->keys->data[i], key) == 0){
      return map->vals->data[i];
    }
  }
  return NULL;
}

// array of tokens after tokenize
// no more than 100 tokens 
Token tokens[100];

Node *code[100];

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

Node *new_node_ident(int val) { // <-- needed
  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
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

  // variables
  if (tokens[pos].ty == TK_IDENT)
    return new_node_ident(tokens[pos++].val);


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

Node *assign() { // <-- needed
  Node *node = add();

  for(;;){
    if(consume('='))
      node = new_node('=', node, assign());
    else
      return node;
  }
}

Node *stmt(){
  Node *node = assign();
  if (!consume(';')){
    error("The token is not ';'", tokens[pos].input);
  }
  return node; // <-- needed
}

void program(){
  int i = 0;
  while(tokens[pos].ty != TK_EOF)
    code[i++] = stmt();
  code[i] = NULL;
}


void gen_lval(Node *node){
  if (node->ty != ND_IDENT){
    error("Lvalue is not a variable", NULL);
  }

  int offset = ('z' - node->val +1) * 8; // <- instead of node->name
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


void tokenize(char *p){
  int i = 0;
  while (*p){
    // skip empty str
    if (isspace(*p)){
      p++;
      continue;
    }

    if (*p == '+' \
        || *p == '-' \
        || *p == '*' \
        || *p == '/' \
        || *p == '(' \
        || *p == ')' \
        || *p == ';' \
        || *p == '=' 
        ){
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if ('a' <= *p && *p <= 'z'){
      tokens[i].ty = TK_IDENT;
      tokens[i].input = p;
      tokens[i].val = *p;
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
    fprintf(stderr, "Line %d => %d\n", line, actual);
    return;
  fprintf(stderr, "Line %d: %d expected, but got %d\n", 
    line, expected, actual);
  exit(1);
}

void test_vector(){
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

void test_map(){
  Map *map = new_map();
  expect(__LINE__, 0, (int)map_get(map, "foo"));

  map_put(map, "foo", (void *)2);
  expect(__LINE__, 2, (int)map_get(map, "foo"));

  map_put(map, "bar", (void *)4);
  expect(__LINE__, 4, (int)map_get(map, "bar"));

  map_put(map, "foo", (void *)6);
  expect(__LINE__, 6, (int)map_get(map, "foo"));
}

void runtest(){
  printf("--test vector\n");
  test_vector();
  printf("--test map\n");
  test_map();
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
