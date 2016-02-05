#include "jsonobject.h"


jsonobject_t* jsonobject_init(int count) {
	jsonobject_t *obj;

	obj = (jsonobject_t *) malloc (sizeof(jsonobject_t));
	obj->pairs = (jsonobject_keyvalue_t *) calloc (sizeof(jsonobject_keyvalue_t), count);
	obj->count = count;
	obj->pos = 0;

	return obj;
}


void jsonobject_push(jsonobject_t *obj, char* key, char* value) {

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


long jsonobject_checkArrayObject(char *key) {
    char *ptr = strchr(key, '[');
    if ( ptr && strchr (key, ']') ) {
        return ptr - key;
    }
    return 0;
}


jsonobject_tree_t* jsonobject_findTreeNode(jsonobject_tree_t *root, char *key) {
	jsonobject_tree_t *obj;
    
	obj = root->children;
	while ( obj != NULL ) {
		if ( ! strcmp(obj->key, key) ) {
			return obj;
		}
		obj = obj->next;
	}
	return NULL;
}


jsonobject_tree_t* jsonobject_createEmptyTreeObject(char *key) {
	jsonobject_tree_t* obj;

	obj = (jsonobject_tree_t *) malloc (sizeof(jsonobject_tree_t));
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


long jsonobject_getArrayId(char *key) {
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


void jsonobject_storeValues(jsonobject_tree_t *obj, char *value) {
    
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


jsonobject_tree_t* jsonobject_createTree(jsonobject_t *obj) {
	jsonobject_tree_t *treeRoot;
	jsonobject_tree_t *treeObj;
	jsonobject_tree_t *treeChild;
	stringlib_tokens_t nameTokens[64];
	char buffer[128];
	int i;
	int j;

	treeRoot = jsonobject_createEmptyTreeObject(NULL);

	for ( i = 0; i < obj->count; i++ ) {
		int count = stringlib_splitTokens(nameTokens, obj->pairs[i].key, '.', 64);
		treeObj = treeRoot;

		for ( j = 0; j < count; j++ ) {
			stringlib_getToken(&nameTokens[j], obj->pairs[i].key, buffer);
            
            long arrayId = -1;
            long pos = jsonobject_checkArrayObject(buffer);
            if ( pos ) {
                arrayId = jsonobject_getArrayId(&buffer[pos]);
                buffer[pos] = '\0';
            }
            
			treeChild = jsonobject_findTreeNode(treeObj, buffer);
            
			if ( treeChild == NULL ) {
                treeChild = jsonobject_createEmptyTreeObject(buffer);
                treeChild->next = treeObj->children;
                treeObj->children = treeChild;
                if ( pos ) {
                    treeChild->type = JSMN_ARRAY;
                }
            }
            
            if ( pos ) {
                jsonobject_tree_t *arrayObj = jsonobject_createEmptyTreeObject(NULL);
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
		jsonobject_storeValues(treeObj, obj->pairs[i].value);
	}
	return treeRoot;
}


char *jsonobject_createJsonString(char *buffer, jsonobject_tree_t *tree) {
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
		pos = jsonobject_createJsonString(pos, tree->children);
		memcpy(pos, " }", 2);
		pos += 2;
	}
    
    if ( tree->type == JSMN_ARRAY ) {
        int i;
        memcpy(pos, "[ ", 2);
        pos += 2;
        
        for ( i = 0; i < tree->maxArrayId + 1; i++ ) {
            jsonobject_tree_t *arrayObj = tree->arrayObjects;
            while ( arrayObj != NULL ) {
                if ( arrayObj->arrayId == i ) {
                    pos = jsonobject_createJsonString(pos, arrayObj);
                    
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
		pos = jsonobject_createJsonString(pos, tree->next);
	}

	return pos;
}


char *jsonobject_create(jsonobject_t *obj) {
    obj->buffer = (char *) calloc(sizeof(char), 8192);
    obj->tree = jsonobject_createTree(obj);
	jsonobject_createJsonString(obj->buffer, obj->tree);
	return obj->buffer;
}


void jsonobject_freeTreeMemory(jsonobject_tree_t *obj) {
    
    if ( obj == NULL ) {
        return;
    }
    
    jsonobject_freeTreeMemory(obj->arrayObjects);
    jsonobject_freeTreeMemory(obj->children);
    jsonobject_freeTreeMemory(obj->next);
    
    if ( obj->key != NULL ) {
        free(obj->key);
    }
    free(obj);
}


void jsonobject_destroy(jsonobject_t *obj) {
	int i;
	for ( i = 0; i < obj->count; i++ ) {
		free(obj->pairs[i].key);
		free(obj->pairs[i].value);
	}
	free(obj->pairs);
	free(obj);
    free(obj->buffer);
    jsonobject_freeTreeMemory(obj->tree);
}
