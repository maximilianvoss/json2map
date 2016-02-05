#ifndef __JSONOBJECT_H__
#define __JSONOBJECT_H__

#include <stdio.h>
#include <stdlib.h>
#include "stringlib.h"
#include "jsmn.h"
#include "config.h"

typedef struct {
	char *key;
	char *value;
} jsonobject_keyvalue_t;

typedef struct s_jsonobject_tree {
	char *key;
	char *value;
    long arrayId;
    long maxArrayId;
	jsmntype_t type;
	struct s_jsonobject_tree *next;
	struct s_jsonobject_tree *children;
	struct s_jsonobject_tree *arrayObjects;
} jsonobject_tree_t;

typedef struct {
    int count;
    int pos;
    jsonobject_keyvalue_t *pairs;
    char *buffer;
    jsonobject_tree_t *tree;
} jsonobject_t;

jsonobject_t* jsonobject_init(int count);
void jsonobject_push(jsonobject_t *obj, char* key, char* value);
char *jsonobject_create(jsonobject_t *obj);
void jsonobject_destroy(jsonobject_t *obj);

#endif