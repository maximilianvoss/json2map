#ifndef __MAP2JSON_H__
#define __MAP2JSON_H__

#include <stdio.h>
#include <stdlib.h>
#include "stringlib.h"
#include "jsmn.h"

typedef struct {
	char *key;
	char *value;
} map2json_keyvalue_t;

typedef struct s_map2json_tree {
	char *key;
	char *value;
    long arrayId;
    long maxArrayId;
	jsmntype_t type;
	struct s_map2json_tree *next;
	struct s_map2json_tree *children;
	struct s_map2json_tree *arrayObjects;
} map2json_tree_t;

typedef struct {
    int count;
    int pos;
    map2json_keyvalue_t *pairs;
    char *buffer;
    map2json_tree_t *tree;
} map2json_t;

map2json_t* map2json_init(int count);
void map2json_push(map2json_t *obj, char* key, char* value);
char *map2json_create(map2json_t *obj);
void map2json_destroy(map2json_t *obj);

#endif