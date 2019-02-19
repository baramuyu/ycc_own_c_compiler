#include "ycc.h"

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

void *map_geti(Map *map, char *key, int default_){
  for (int i = map->keys->len - 1; i >= 0; i--){
    if (!strcmp(map->keys->data[i], key)){
      return (intptr_t)map->vals->data[i];
    }
  }
  return default_;
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

char *strdup(const char *str)
{
    int n = strlen(str) + 1;
    char *dup = malloc(n);
    if(dup)
    {
        strcpy(dup, str);
    }
    return dup;
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
  enum
  Map *map = new_map();
  expect(__LINE__, 0, (int)map_get(map, "foo"));

  map_put(map, "foo", (void *)2);
  expect(__LINE__, 2, (int)map_get(map, "foo"));

  map_put(map, "bar", (void *)4);
  expect(__LINE__, 4, (int)map_get(map, "bar"));

  map_put(map, "foo", (void *)6);
  expect(__LINE__, 6, (int)map_get(map, "foo"));

  // test map_geti
  Map *mapi = new_map();
  expect(__LINE__, TK_IDENT, (int)map_geti(mapi, "foo", TK_IDENT));

  map_put(mapi, "foo", (void *)2);
  expect(__LINE__, 2, (int)map_geti(mapi, "foo", TK_IDENT));

  map_put(mapi, "bar", (void *)4);
  expect(__LINE__, 4, (int)map_geti(mapi, "bar", TK_IDENT));

  map_put(mapi, "foo", (void *)6);
  expect(__LINE__, 6, (int)map_geti(mapi, "foo", TK_IDENT));
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
