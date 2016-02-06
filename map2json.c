#include "map2json.h"


map2json_t* map2json_init(int count) {
	// TODO: pairs should be a linked list
	// TODO: init all variables in here
	map2json_t *obj;

	obj = (map2json_t *) malloc (sizeof(map2json_t));
	obj->pairs = (map2json_keyvalue_t *) calloc (sizeof(map2json_keyvalue_t), count);
	obj->count = count;
	obj->pos = 0;

	return obj;
}


void map2json_push(map2json_t *obj, char* key, char* value) {

	if ( obj->pos < obj->count) {
		unsigned long keyLen = strlen(key);
		unsigned long valLen = strlen(value);

		obj->pairs[obj->pos].key = (char *) calloc (sizeof(char), keyLen + 1);
		obj->pairs[obj->pos].value = (char *) calloc (sizeof(char), valLen + 1);

		memcpy(obj->pairs[obj->pos].key, key, keyLen);
		memcpy(obj->pairs[obj->pos].value, value, valLen);

		obj->pos++;
	}
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
    char buffer[1<<8];

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
	// TODO: buffer size should be adquat or configurable
	// TODO: nameTokens should be configurable
	// TOOO: re-org code nicely, specially the ARRAY case

	map2json_tree_t *treeRoot;
	map2json_tree_t *treeObj;
	map2json_tree_t *treeChild;
	stringlib_tokens_t nameTokens[64];
	char buffer[128];
	int i;
	int j;

	treeRoot = map2json_createEmptyTreeObject(NULL);

	for ( i = 0; i < obj->count; i++ ) {
		int count = stringlib_splitTokens(nameTokens, obj->pairs[i].key, '.', 64);
		treeObj = treeRoot;

		for ( j = 0; j < count; j++ ) {
			stringlib_getToken(&nameTokens[j], obj->pairs[i].key, buffer);
            
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
		map2json_storeValues(treeObj, obj->pairs[i].value);
	}
	return treeRoot;
}


char *map2json_createJsonString(char *buffer, map2json_tree_t *tree) {
	// TODO: savety on buffer overflow

    char *pos = buffer;
	unsigned long length;

	if ( tree == NULL ) {
		return pos;
	}

	if ( tree->key != NULL ) {
        *pos ='\"';
        pos++;
		length = strlen(tree->key);
		memcpy(pos, tree->key, length);
		pos += length;
        *pos ='\"';
        pos++;
		memcpy(pos, " : ", 3);
		pos += 3;
	}

	if ( tree->type == JSMN_OBJECT ) {
		memcpy(pos, "{ ", 2);
		pos+= 2;
		pos = map2json_createJsonString(pos, tree->children);
		memcpy(pos, " }", 2);
		pos += 2;
	}
    
    if ( tree->type == JSMN_ARRAY ) {
        int i;
        memcpy(pos, "[ ", 2);
        pos += 2;
        
        for ( i = 0; i < tree->maxArrayId + 1; i++ ) {
            map2json_tree_t *arrayObj = tree->arrayObjects;
            while ( arrayObj != NULL ) {
                if ( arrayObj->arrayId == i ) {
                    pos = map2json_createJsonString(pos, arrayObj);
                    
                    if ( i < tree->maxArrayId ) {
                        memcpy(pos, ", ", 2);
                        pos += 2;
                    }
                    
                    break;
                }
                arrayObj = arrayObj->arrayObjects;
            }

        }
        memcpy(pos, " ]", 2);
        pos += 2;
    }
    
	if ( tree->type == JSMN_PRIMITIVE || tree->type == JSMN_STRING ) {
        if ( tree->type == JSMN_STRING ) {
            *pos ='\"';
            pos++;
        }
		length = strlen(tree->value);
		memcpy(pos, tree->value, length);
		pos += length;
        if ( tree->type == JSMN_STRING ) {
            *pos ='\"';
            pos++;
        }
	}

	if ( tree->next != NULL ) {
		memcpy(pos, ", ", 2);
		pos += 2;
		pos = map2json_createJsonString(pos, tree->next);
	}

	return pos;
}


char *map2json_create(map2json_t *obj) {
	// TODO: Buffer size configurable

    obj->buffer = (char *) calloc(sizeof(char), 8192);
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


void map2json_destroy(map2json_t *obj) {
	int i;
	for ( i = 0; i < obj->count; i++ ) {
		free(obj->pairs[i].key);
		free(obj->pairs[i].value);
	}
	free(obj->pairs);
	free(obj);
    free(obj->buffer);
    map2json_freeTreeMemory(obj->tree);
}
