#include "map2json.h"

#define BUFFER_LENGTH 1<<13
#define MAX_MAP_KEY_DEPTH 1<<6

map2json_t* map2json_init() {
	map2json_t *obj;

	obj = (map2json_t *) malloc (sizeof(map2json_t));
    obj->buffer = (char *) calloc(sizeof(char), BUFFER_LENGTH);
    obj->pairs = NULL;
    
	return obj;
}


void map2json_push(map2json_t *obj, char* key, char* value) {
	map2json_keyvalue_t *pair = (map2json_keyvalue_t *) malloc (sizeof(map2json_keyvalue_t));

	unsigned long keyLen = strlen(key);
	unsigned long valLen = strlen(value);

	pair->key = (char *) calloc (sizeof(char), keyLen + 1);
	pair->value = (char *) calloc (sizeof(char), valLen + 1);

	memcpy(pair->key, key, keyLen);
	memcpy(pair->value, value, valLen);
	
	pair->next = obj->pairs;
	obj->pairs = pair;
}


long map2json_checkArrayObject(char *key) {
    char *ptr = strchr(key, '[');
    if ( ptr && strchr (key, ']') ) {
        return ptr - key;
    }
    return 0;
}


map2json_tree_t* map2json_findTreeNode(map2json_tree_t *root, char *key) {
	map2json_tree_t *obj;
    
	obj = root->children;
	while ( obj != NULL ) {
		if ( ! strcmp(obj->key, key) ) {
			return obj;
		}
		obj = obj->next;
	}
	return NULL;
}


map2json_tree_t* map2json_createEmptyTreeObject(char *key) {
	map2json_tree_t* obj;

	obj = (map2json_tree_t *) malloc (sizeof(map2json_tree_t));
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
		obj->key = (char *) malloc (sizeof(char) * (strlen(key) + 1) );
		memcpy(obj->key, key, strlen(key));
	}
	return obj;
}


long map2json_getArrayId(char *key) {
    char buffer[BUFFER_LENGTH];

    if ( key == NULL ) {
        return -1;
    }
    key++;
    long length = strlen(key);
    
    memcpy(buffer, key, length);
    buffer[length - 1] = '\0';
    
    return atoi(buffer);
}


void map2json_storeValues(map2json_tree_t *obj, char *value) {
    
    if ( obj->type == JSMN_ARRAY ) {
        return;
    }
    
	if ( stringlib_isInteger(value) || !strcmp(value, "null") || !strcmp(value, "true") || !strcmp(value, "false") ) {
		obj->type = JSMN_PRIMITIVE;
        obj->value = value;
	} else {
		obj->type = JSMN_STRING;
        obj->value = value;
	}
}


map2json_tree_t* map2json_createTree(map2json_t *obj) {
	// TOOO: re-org code nicely, specially the ARRAY case

	map2json_tree_t *treeRoot;
	map2json_tree_t *treeObj;
	map2json_tree_t *treeChild;
	map2json_keyvalue_t *pair;
	stringlib_tokens_t nameTokens[MAX_MAP_KEY_DEPTH];
	char buffer[BUFFER_LENGTH];
	int i;

	treeRoot = map2json_createEmptyTreeObject(NULL);

	pair = obj->pairs;
	while(pair != NULL) {
		int count = stringlib_splitTokens(nameTokens, pair->key, '.', MAX_MAP_KEY_DEPTH);
		treeObj = treeRoot;

		for ( i = 0; i < count; i++ ) {
			stringlib_getToken(&nameTokens[i], pair->key, buffer);
            
            long arrayId = -1;
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
                map2json_tree_t *arrayObj = map2json_createEmptyTreeObject(NULL);
                arrayObj->arrayId = arrayId;
                
                if ( treeChild->maxArrayId < arrayId ) {
                    treeChild->maxArrayId = arrayId;
                }
                
                arrayObj->arrayObjects = treeChild->arrayObjects;
                treeChild->arrayObjects = arrayObj;
                treeObj = arrayObj;
            } else {
                treeObj = treeChild;
            }
		}
		map2json_storeValues(treeObj, pair->value);
		pair = pair->next;
	}
	return treeRoot;
}


char *map2json_addChar(char *str, char chr) {
	*str = chr;
	str++;
	return str;
}

char *map2json_createJsonStringArray(char *buffer, map2json_tree_t *tree) {
    int i;
	char *pos = buffer;
	pos = map2json_addChar(pos, '[');
    
    for ( i = 0; i < tree->maxArrayId + 1; i++ ) {
        map2json_tree_t *arrayObj = tree->arrayObjects;
        while ( arrayObj != NULL ) {
            if ( arrayObj->arrayId == i ) {
                pos = map2json_createJsonString(pos, arrayObj);
                
                if ( i < tree->maxArrayId ) {
                	pos = map2json_addChar(pos, ',');
                }
                
                break;
            }
            arrayObj = arrayObj->arrayObjects;
        }
    }
    pos = map2json_addChar(pos, ']');
    return pos;
}

char *map2json_createJsonString(char *buffer, map2json_tree_t *tree) {
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

	return pos;
}


char *map2json_create(map2json_t *obj) {
    obj->tree = map2json_createTree(obj);
	map2json_createJsonString(obj->buffer, obj->tree);
	return obj->buffer;
}


void map2json_freeTreeMemory(map2json_tree_t *obj) {
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
}


void map2json_freePairsMemory(map2json_keyvalue_t *pair) {
	if ( pair == NULL ) {
		return;
	}
	map2json_freePairsMemory(pair->next);
	free (pair->key);
	free (pair->value);
	free (pair);
}


void map2json_destroy(map2json_t *obj) {
	map2json_freePairsMemory (obj->pairs);
	free(obj);
    free(obj->buffer);
    map2json_freeTreeMemory(obj->tree);
}
