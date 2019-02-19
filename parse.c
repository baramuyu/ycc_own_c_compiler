#include "ycc.h"

int pos = 0;

static Map *keyword_map() {
  Map *map = new_map();
  return map;
}

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

Node *local_variable(Token *t){
  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
  node->name = t->name;
  node->val = t->val;
  return node;
}

// check next token is expected or not
int consume(int ty) {
  if (tokens[pos].ty != ty)
    return 0;
  pos++;
  return 1;
}

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
    return local_variable(&tokens[pos++]);
    //return new_node_ident(tokens[pos++].val);

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
    if(consume('=')){
      node = new_node('=', node, assign());
    }
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

static char *ident(char *p, Token *t){
  int len = 1;
  while ('a' <= p[len] && p[len] <= 'z')
    len++;
  char *name = strndup(p, len);

  int ty = map_geti(keywords, name, TK_IDENT);
  t->ty = ty;
  t->name = *name;
  t->start = p;
  t->end = p + len;
  return p + len;
}

void tokenize(char *p){
  if (!keywords)
    keywords = keyword_map();

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
      p = ident(p, &tokens[i]);
      i++;
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