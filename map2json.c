#include <stdio.h>
#include "map2json.h"
#include "config.h"
#include "debugging.h"
#include "stringlib.h"

static void map2json_createJsonString(csafestring_t *buffer, map2json_tree_t *tree);
static long map2json_checkArrayObject(char *key);
static map2json_tree_t *map2json_findTreeNode(map2json_tree_t *root, char *key);
static map2json_tree_t *map2json_createEmptyTreeObject(char *key);
static long map2json_getArrayId(char *key);
static void map2json_storeValues(map2json_tree_t *obj, char *value);
static void map2json_createTree(map2json_t *obj);
static void map2json_createJsonStringArray(csafestring_t *buffer, map2json_tree_t *tree);
static void map2json_freeTreeMemory(map2json_tree_t *obj);
static void map2json_freePairsMemory(map2json_keyvalue_t *pair);

#define ARRAYID_NOT_SET -1
#define ARRAYID_IS_COUNT -2

map2json_t *map2json_init() {
	DEBUG_PUT("map2json_init()... ");
	map2json_t *obj;

	obj = (map2json_t *) malloc(sizeof(map2json_t));
	obj->buffer = safe_create(NULL);
	obj->pairs = NULL;
	obj->tree = NULL;

	DEBUG_PUT("map2json_init()... DONE");
	return obj;
}


void map2json_push(map2json_t *obj, char *key, char *value) {
	DEBUG_TEXT("map2json_push([map2json_t *], %s, %s)... ", key, value);

	map2json_keyvalue_t *pair = (map2json_keyvalue_t *) malloc(sizeof(map2json_keyvalue_t));

	size_t keyLen = strlen(key) + 1;
	size_t valLen = strlen(value) + 1;

	pair->key = (char *) calloc(sizeof(char), keyLen);
	pair->value = (char *) calloc(sizeof(char), valLen);

	memcpy(pair->key, key, keyLen);
	memcpy(pair->value, value, valLen);

	pair->next = obj->pairs;
	obj->pairs = pair;

	DEBUG_TEXT("map2json_push([map2json_t *], %s, %s)... DONE", key, value);
}


static long map2json_checkArrayObject(char *key) {
	DEBUG_TEXT("map2json_checkArrayObject(%s)... ", key);
	char *ptr = strchr(key, JSON2MAP_MAP_ARRAY_START);
	if ( ptr && strchr(key, JSON2MAP_MAP_ARRAY_END) ) {
		return ptr - key;
	}
	DEBUG_TEXT("map2json_checkArrayObject(%s)... ", key);
	return 0;
}


static map2json_tree_t *map2json_findTreeNode(map2json_tree_t *root, char *key) {
	DEBUG_TEXT("map2json_findTreeNode([map2json_tree_t *], %s)... ", key);
	map2json_tree_t *obj;

	obj = root->children;
	while ( obj != NULL ) {
		if ( !strcmp(obj->key, key) ) {
			DEBUG_TEXT("map2json_findTreeNode([map2json_tree_t *], %s): key found", key);
			DEBUG_TEXT("map2json_findTreeNode([map2json_tree_t *], %s)... DONE", key);
			return obj;
		}
		obj = obj->next;
	}
	DEBUG_TEXT("map2json_findTreeNode([map2json_tree_t *], %s)... DONE", key);
	return NULL;
}


static map2json_tree_t *map2json_createEmptyTreeObject(char *key) {
	DEBUG_TEXT("map2json_createEmptyTreeObject(%s)... ", key);
	map2json_tree_t *obj;

	obj = (map2json_tree_t *) malloc(sizeof(map2json_tree_t));
	obj->next = NULL;
	obj->value = NULL;
	obj->children = NULL;
	obj->arrayObjects = NULL;
	obj->type = JSMN_OBJECT;
	obj->arrayId = -1;
	obj->maxArrayId = -1;

	if ( key == NULL ) {
		obj->key = NULL;
	} else {
		obj->key = (char *) calloc(sizeof(char), strlen(key) + 1);
		memcpy(obj->key, key, strlen(key));
	}

	DEBUG_TEXT("map2json_createEmptyTreeObject(%s)... DONE", key);
	return obj;
}


static long map2json_getArrayId(char *key) {
	DEBUG_TEXT("map2json_getArrayId(%s)... ", key);

	char *buffer;
	int value;

	if ( key == NULL ) {
		return ARRAYID_NOT_SET;
	}
	key++;
	long length = strlen(key);

	buffer = calloc(sizeof(char), length);
	memcpy(buffer, key, length);
	buffer[length - 1] = '\0';

	if ( *key == JSON2MAP_MAP_ARRAY_COUNT ) {
		free(buffer);
		return ARRAYID_IS_COUNT;
	}

	value = atoi(buffer);
	free(buffer);
	DEBUG_TEXT("map2json_getArrayId(%s)... DONE", key);
	return value;
}


static void map2json_storeValues(map2json_tree_t *obj, char *value) {
	DEBUG_TEXT("map2json_storeValues([map2json_tree_t *], %s)... ", value);

	if ( obj->type == JSMN_ARRAY ) {
		DEBUG_TEXT("map2json_storeValues([map2json_tree_t *], %s): object is array", value);
		obj->maxArrayId = atoi(value);
		DEBUG_TEXT("map2json_storeValues([map2json_tree_t *], %s)... DONE ", value);
		return;
	}

	if ( stringlib_isInteger(value) || !strcmp(value, "null") || !strcmp(value, "true") || !strcmp(value, "false") || *value == JSON2MAP_PRIMITIVE_PREFIXER ) {
		DEBUG_TEXT("map2json_storeValues([map2json_tree_t *], %s): object is primitive", value);
		obj->type = JSMN_PRIMITIVE;
		obj->value = value;
		if ( *value == JSON2MAP_PRIMITIVE_PREFIXER ) {
			obj->value++;
		}
	} else {
		DEBUG_TEXT("map2json_storeValues([map2json_tree_t *], %s): object is string", value);
		obj->type = JSMN_STRING;
		obj->value = value;
	}

	DEBUG_TEXT("map2json_storeValues([map2json_tree_t *], %s)... DONE", value);
}


map2json_tree_t *map2json_getArrayObject(map2json_tree_t *obj, long arrayId) {
	map2json_tree_t *arrObj;

	if ( arrayId == ARRAYID_IS_COUNT ) {
		return obj;
	}

	arrObj = obj->arrayObjects;
	while ( arrObj != NULL ) {
		if ( arrObj->arrayId == arrayId ) {
			return arrObj;
		}
		arrObj = arrObj->arrayObjects;

	}
	arrObj = map2json_createEmptyTreeObject(NULL);
	if ( obj->maxArrayId < arrayId ) {
		obj->maxArrayId = arrayId + 1;
	}
	arrObj->arrayId = arrayId;
	arrObj->arrayObjects = obj->arrayObjects;
	obj->arrayObjects = arrObj;

	return arrObj;
}


static void map2json_createTree(map2json_t *obj) {
	DEBUG_PUT("map2json_createTree([map2json_t *])... ");
	map2json_tree_t *treeRoot;
	map2json_tree_t *treeObj;
	map2json_tree_t *treeChild;
	map2json_keyvalue_t *pair;
	stringlib_tokens_t *tokens;
	stringlib_tokens_t *token;
	char *buffer;

	obj->tree = map2json_createEmptyTreeObject(NULL);
	treeRoot = obj->tree;

	pair = obj->pairs;
	while ( pair != NULL ) {
		tokens = stringlib_splitTokens(pair->key, JSON2MAP_MAP_OBJECT_SEPARATOR);
		token = tokens;
		treeObj = treeRoot;

		while ( token != NULL ) {
			buffer = stringlib_getToken(token, pair->key);

			long arrayId = ARRAYID_NOT_SET;
			long pos = map2json_checkArrayObject(buffer);
			if ( pos ) {
				arrayId = map2json_getArrayId(&buffer[pos]);
				buffer[pos] = '\0';
			}

			treeChild = map2json_findTreeNode(treeObj, buffer);

			if ( treeChild == NULL ) {
				treeChild = map2json_createEmptyTreeObject(buffer);
				treeChild->next = treeObj->children;
				treeObj->children = treeChild;
				if ( pos ) {
					treeChild->type = JSMN_ARRAY;
				}
			}

			if ( pos ) {
				treeObj = map2json_getArrayObject(treeChild, arrayId);
			} else {
				treeObj = treeChild;
			}
			free(buffer);
			token = token->next;
		}
		map2json_storeValues(treeObj, pair->value);
		pair = pair->next;
		stringlib_freeTokens(tokens);
	}

	DEBUG_PUT("map2json_createTree([map2json_t *])... DONE");
}

static void map2json_createJsonStringArray(csafestring_t *buffer, map2json_tree_t *tree) {
	DEBUG_TEXT("map2json_createJsonStringArray(%s, [map2json_tree_t *])... ", buffer->data);

	int i;
	safe_strchrappend(buffer, JSON2MAP_MAP_ARRAY_START);

	for ( i = 0; i < tree->maxArrayId; i++ ) {
		map2json_tree_t *arrayObj = tree->arrayObjects;
		while ( arrayObj != NULL ) {
			if ( arrayObj->arrayId == i ) {
				map2json_createJsonString(buffer, arrayObj);
			}
			arrayObj = arrayObj->arrayObjects;
		}
		if ( i < tree->maxArrayId - 1 ) {
			safe_strchrappend(buffer, ',');
		}
	}
	safe_strchrappend(buffer, JSON2MAP_MAP_ARRAY_END);

	DEBUG_TEXT("map2json_createJsonStringArray(%s, [map2json_tree_t *])... DONE", buffer->data);
}

static void map2json_createJsonString(csafestring_t *buffer, map2json_tree_t *tree) {
	DEBUG_TEXT("map2json_createJsonString(%s, [map2json_tree_t *])... ", buffer->data);

	if ( tree == NULL ) {
		DEBUG_TEXT("map2json_createJsonString(%s, [map2json_tree_t *])... DONE", buffer->data);
		return;
	}

	if ( tree->key != NULL ) {
		safe_strchrappend(buffer, '\"');
		safe_strcat(buffer, tree->key);
		safe_strchrappend(buffer, '\"');
		safe_strchrappend(buffer, ':');
	}

	if ( tree->type == JSMN_OBJECT ) {
		safe_strchrappend(buffer, '{');
		map2json_createJsonString(buffer, tree->children);
		safe_strchrappend(buffer, '}');
	}

	if ( tree->type == JSMN_ARRAY ) {
		map2json_createJsonStringArray(buffer, tree);
	}

	if ( tree->type == JSMN_PRIMITIVE || tree->type == JSMN_STRING ) {
		if ( tree->type == JSMN_STRING ) {
			safe_strchrappend(buffer, '\"');
		}
		safe_strcat(buffer, tree->value);
		if ( tree->type == JSMN_STRING ) {
			safe_strchrappend(buffer, '\"');
		}
	}

	if ( tree->next != NULL ) {
		safe_strchrappend(buffer, ',');
		map2json_createJsonString(buffer, tree->next);
	}
	DEBUG_TEXT("map2json_createJsonString(%s, [map2json_tree_t *])... DONE", buffer->data);
}

char *map2json_create(map2json_t *obj) {
	DEBUG_PUT("map2json_create([map2json_t *])... ");

	map2json_freeTreeMemory(obj->tree);

	map2json_createTree(obj);
	safe_strcpy(obj->buffer, "");
	map2json_createJsonString(obj->buffer, obj->tree);

	DEBUG_PUT("map2json_create([map2json_t *])... DONE");
	return obj->buffer->data;
}


static void map2json_freeTreeMemory(map2json_tree_t *obj) {
	DEBUG_PUT("map2json_freeTreeMemory([map2json_t *])... ");

	if ( obj == NULL ) {
		DEBUG_PUT("map2json_freeTreeMemory([map2json_t *])... DONE");
		return;
	}

	map2json_freeTreeMemory(obj->arrayObjects);
	map2json_freeTreeMemory(obj->children);
	map2json_freeTreeMemory(obj->next);

	if ( obj->key != NULL ) {
		free(obj->key);
	}
	free(obj);
	DEBUG_PUT("map2json_freeTreeMemory([map2json_t *])... DONE");
}


static void map2json_freePairsMemory(map2json_keyvalue_t *pair) {
	DEBUG_PUT("map2json_freePairsMemory([map2json_t *])... ");
	if ( pair == NULL ) {
		DEBUG_PUT("map2json_freePairsMemory([map2json_t *])... DONE");
		return;
	}
	map2json_freePairsMemory(pair->next);
	free(pair->key);
	free(pair->value);
	free(pair);
	DEBUG_PUT("map2json_freePairsMemory([map2json_t *])... DONE");
}


void map2json_destroy(map2json_t *obj) {
	DEBUG_PUT("map2json_destroy([map2json_t *])... ");
	map2json_freePairsMemory(obj->pairs);
	map2json_freeTreeMemory(obj->tree);
	safe_destroy(obj->buffer);
	free(obj);
	DEBUG_PUT("map2json_destroy([map2json_t *])... DONE");
}
