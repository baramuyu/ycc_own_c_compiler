#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

// number that represents type of token
enum {
  TK_NUM = 256, // integer token
  TK_IDENT,     // idetifier
  TK_EOF,       // end of token
  ND_NUM = 256,     // type of integer node
  ND_IDENT,   // type of identifier node; 257
};

// type of token
typedef struct {
  int ty;       // type of token
  int val;      // if ty is TK_NUM, its number
  char *name;
  int *end;
  int *start;
  char *input;  // token str for error message 
} Token;


typedef struct Node {
  int ty;           // operator or ND_NUM
  struct Node *lhs; // left side
  struct Node *rhs; // right side
  int val;          // if ty is ND_NUM
  char name;      // if ty is ND_IDENT <-- didn't use
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

static Map *keywords;

// parse.c
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_ident(int val);
Node *local_variable(Token *t);
int consume(int ty);
Node *mul();
Node *add();
Node *term();
Node *assign();
Node *stmt();
void program();
static char *ident(char *p, Token *t);
void tokenize(char *p);
char *strdup(const char *str);

// codegen.c
void gen_lval(Node *node);
void gen(Node *node);

// container.c
Vector *new_vector();
void vec_push(Vector *vec, void *elem);
Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
void *map_geti(Map *map, char *key, int default_);
void error(char message[], char *p);
void expect(int line, int expected, int actual);
void test_vector();
void test_map();
void runtest();
static Map *keyword_map();

Token tokens[100];
Node *code[100];
