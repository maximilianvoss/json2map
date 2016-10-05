#include "map2json.h"
#include "config.h"
#include "debugging.h"
#include <stdio.h>
#include <stdlib.h>
#include "stringlib.h"


long map2json_checkArrayObject(char *key);
map2json_tree_t *map2json_findTreeNode(map2json_tree_t *root, char *key);
map2json_tree_t *map2json_createEmptyTreeObject(char *key);
long map2json_getArrayId(char *key);
void map2json_storeValues(map2json_tree_t *obj, char *value);
map2json_tree_t *map2json_createTree(map2json_t *obj);
char *map2json_addChar(char *str, char chr);
char *map2json_createJsonStringArray(char *buffer, map2json_tree_t *tree);
void map2json_freeTreeMemory(map2json_tree_t *obj);
void map2json_freePairsMemory(map2json_keyvalue_t *pair);
void map2json_destroy(map2json_t *obj);

#define ARRAYID_NOT_SET -1
#define ARRAYID_IS_COUNT -2

map2json_t *map2json_init() {
	DEBUG_PUT("map2json_init()... ");
	map2json_t *obj;

	obj = (map2json_t *) malloc(sizeof(map2json_t));
	obj->buffer = (char *) calloc(sizeof(char), JSON2MAP_BUFFER_LENGTH);
	obj->pairs = NULL;

	DEBUG_PUT("map2json_init()... DONE");
	return obj;
}


void map2json_push(map2json_t *obj, char *key, char *value) {
	DEBUG_TEXT("map2json_push([map2json_t *], %s, %s)... ", key, value);

	map2json_keyvalue_t *pair = (map2json_keyvalue_t *) malloc(sizeof(map2json_keyvalue_t));

	unsigned long keyLen = strlen(key) + 1;
	unsigned long valLen = strlen(value) + 1;

	pair->key = (char *) calloc(sizeof(char), keyLen);
	pair->value = (char *) calloc(sizeof(char), valLen);

	memcpy(pair->key, key, keyLen);
	memcpy(pair->value, value, valLen);

	pair->next = obj->pairs;
	obj->pairs = pair;

	DEBUG_TEXT("map2json_push([map2json_t *], %s, %s)... DONE", key, value);
}


long map2json_checkArrayObject(char *key) {
	DEBUG_TEXT("map2json_checkArrayObject(%s)... ", key);
	char *ptr = strchr(key, JSON2MAP_MAP_ARRAY_START);
	if ( ptr && strchr(key, JSON2MAP_MAP_ARRAY_END) ) {
		return ptr - key;
	}
	DEBUG_TEXT("map2json_checkArrayObject(%s)... ", key);
	return 0;
}


map2json_tree_t *map2json_findTreeNode(map2json_tree_t *root, char *key) {
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


map2json_tree_t *map2json_createEmptyTreeObject(char *key) {
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


long map2json_getArrayId(char *key) {
	DEBUG_TEXT("map2json_getArrayId(%s)... ", key);

	char buffer[JSON2MAP_BUFFER_LENGTH];

	if ( key == NULL ) {
		return ARRAYID_NOT_SET;
	}
	key++;
	long length = strlen(key);

	memcpy(buffer, key, length);
	buffer[length - 1] = '\0';

	if ( *key == JSON2MAP_MAP_ARRAY_COUNT ) {
		return ARRAYID_IS_COUNT;
	}

	DEBUG_TEXT("map2json_getArrayId(%s)... DONE", key);
	return atoi(buffer);
}


void map2json_storeValues(map2json_tree_t *obj, char *value) {
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
		arrObj = arrObj->next;

	}
	arrObj = map2json_createEmptyTreeObject(NULL);
	if ( obj->maxArrayId < arrayId ) {
		obj->maxArrayId = arrayId;
	}
	arrObj->arrayId = arrayId;
	arrObj->next = obj->arrayObjects;
	obj->arrayObjects = arrObj;

	return arrObj;
}


map2json_tree_t *map2json_createTree(map2json_t *obj) {
	DEBUG_PUT("map2json_createTree([map2json_t *])... ");
	map2json_tree_t *treeRoot;
	map2json_tree_t *treeObj;
	map2json_tree_t *treeChild;
	map2json_keyvalue_t *pair;
	stringlib_tokens_t nameTokens[JSON2MAP_MAX_MAP_KEY_DEPTH];
	char buffer[JSON2MAP_BUFFER_LENGTH];
	int i;

	treeRoot = map2json_createEmptyTreeObject(NULL);

	pair = obj->pairs;
	while ( pair != NULL ) {
		int count = stringlib_splitTokens(nameTokens, pair->key, JSON2MAP_MAP_OBJECT_SEPARATOR, JSON2MAP_MAX_MAP_KEY_DEPTH);
		treeObj = treeRoot;

		for ( i = 0; i < count; i++ ) {
			stringlib_getToken(&nameTokens[i], pair->key, buffer);

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
		}
		map2json_storeValues(treeObj, pair->value);
		pair = pair->next;
	}

	DEBUG_PUT("map2json_createTree([map2json_t *])... DONE");
	return treeRoot;
}


char *map2json_addChar(char *str, char chr) {
	DEBUG_TEXT("map2json_addChar(%s, %c)... ", str, chr);
	*str = chr;
	str++;
	DEBUG_TEXT("map2json_addChar(%s, %c)... DONE", str, chr);
	return str;
}


char *map2json_createJsonStringArray(char *buffer, map2json_tree_t *tree) {
	DEBUG_TEXT("map2json_createJsonStringArray(%s, [map2json_tree_t *])... ", buffer);

	int i;
	char *pos = buffer;
	pos = map2json_addChar(pos, JSON2MAP_MAP_ARRAY_START);

	for ( i = 0; i < tree->maxArrayId + 1; i++ ) {
		map2json_tree_t *arrayObj = tree->arrayObjects;
		while ( arrayObj != NULL ) {
			if ( arrayObj->arrayId == i ) {
				pos = map2json_createJsonString(pos, arrayObj);
			}
			arrayObj = arrayObj->arrayObjects;
		}
	}
	pos = map2json_addChar(pos, JSON2MAP_MAP_ARRAY_END);

	DEBUG_TEXT("map2json_createJsonStringArray(%s, [map2json_tree_t *])... DONE", buffer);
	return pos;
}

char *map2json_createJsonString(char *buffer, map2json_tree_t *tree) {
	DEBUG_TEXT("map2json_createJsonString(%s, [map2json_tree_t *])... ", buffer);

	char *pos = buffer;
	unsigned long length;

	if ( tree == NULL ) {
		return pos;
	}

	if ( tree->key != NULL ) {
		pos = map2json_addChar(pos, '\"');
		length = strlen(tree->key);
		memcpy(pos, tree->key, length);
		pos += length;
		pos = map2json_addChar(pos, '\"');
		pos = map2json_addChar(pos, ':');
	}

	if ( tree->type == JSMN_OBJECT ) {
		pos = map2json_addChar(pos, '{');
		pos = map2json_createJsonString(pos, tree->children);
		pos = map2json_addChar(pos, '}');
	}

	if ( tree->type == JSMN_ARRAY ) {
		pos = map2json_createJsonStringArray(pos, tree);
	}

	if ( tree->type == JSMN_PRIMITIVE || tree->type == JSMN_STRING ) {
		if ( tree->type == JSMN_STRING ) {
			pos = map2json_addChar(pos, '\"');
		}
		length = strlen(tree->value);
		memcpy(pos, tree->value, length);
		pos += length;
		if ( tree->type == JSMN_STRING ) {
			pos = map2json_addChar(pos, '\"');
		}
	}

	if ( tree->next != NULL ) {
		pos = map2json_addChar(pos, ',');
		pos = map2json_createJsonString(pos, tree->next);
	}

	DEBUG_TEXT("map2json_createJsonString(%s, [map2json_tree_t *])... DONE", buffer);
	return pos;
}

char *map2json_create(map2json_t *obj) {
	DEBUG_PUT("map2json_create([map2json_t *])... ");
	obj->tree = map2json_createTree(obj);
	map2json_createJsonString(obj->buffer, obj->tree);

	DEBUG_PUT("map2json_create([map2json_t *])... ");
	return obj->buffer;
}


void map2json_freeTreeMemory(map2json_tree_t *obj) {
	DEBUG_PUT("map2json_freeTreeMemory([map2json_t *])... ");

	if ( obj == NULL ) {
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


void map2json_freePairsMemory(map2json_keyvalue_t *pair) {
	DEBUG_PUT("map2json_freePairsMemory([map2json_t *])... ");
	if ( pair == NULL ) {
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
	free(obj->buffer);
	free(obj);
	DEBUG_PUT("map2json_destroy([map2json_t *])... DONE");
}
