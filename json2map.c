#include "json2map.h"


#define BUFFER_LENGTH 1<<10
#define MAX_JSON_TOKENS 1<<8


json2map_t *json2map_init() {
	json2map_t *obj = (json2map_t *) malloc (sizeof(json2map_t));
	return obj;
}

void json2map_destroy(json2map_t *obj) {
	free (obj);
}


void json2map_setTokenValue(char *jsonString, jsmntok_t *token, char *buffer) {
	memcpy(buffer, jsonString + token->start, token->end - token->start);
}


char *json2map_concatPaths(char *parent, char *key, int arrayIdx) {
	char *path;
	char arrayIdxBuff[10];

	long arrayIdLen = 0;
	long keyLen = 0;
	long parentLen = 0;
	long addition = -1;

	if ( key != NULL ) {
		keyLen = strlen(key);
		addition++;
	}

	if ( parent != NULL ) {
		parentLen = strlen(parent);
		addition++;
	}

	if ( arrayIdx >= 0 ) {
		sprintf(arrayIdxBuff, "[%d]", arrayIdx);
		arrayIdLen += strlen(arrayIdxBuff);
	}
	
    path = (char *) calloc(sizeof(char), parentLen + keyLen + arrayIdLen + addition + 1);
    memcpy(path, parent, parentLen);

    if ( parentLen && keyLen ) {
    	path[parentLen] = '.';
	}

    memcpy(path + parentLen + addition, key, keyLen);

    if ( arrayIdx >= 0 ) {
        memcpy(path + parentLen + keyLen + addition, arrayIdxBuff, arrayIdLen);
    }

	return path;
}


int json2map_calcEnd(jsmntok_t *token, int start, int end) {
	// TODO: optimize search
	int i;
	for ( i = start + 1; i < end; i++ ) {
		if ( token[i].start > token[start].end) {
			return i - 1;
		}
	}
	return end - 1;
}


int json2map_parseArray(json2map_t *obj, char *path, char *jsonString, jsmntok_t *token, int start, int end) {
	int newEnd;
	char buffer[BUFFER_LENGTH];
	char *pathBuff;
	int count = 0;

	int i = start;
	while ( i < end && i > 0) {

		pathBuff = json2map_concatPaths(NULL, path, count);
		count++;

		switch ( token[i].type ) {
			case JSMN_OBJECT:
				newEnd = json2map_calcEnd(token, i, end);
				i = json2map_parseObject(obj, pathBuff, jsonString, token, i + 1, newEnd + 1);
				break;
			case JSMN_STRING:
			case JSMN_PRIMITIVE:
				memset(buffer, '\0', BUFFER_LENGTH);
				json2map_setTokenValue(jsonString, &token[i], buffer);	
				obj->hookMethod(obj->hookMethodData, pathBuff, buffer);
				memset(buffer, '\0', BUFFER_LENGTH);
				i++;
				break;
			default:
				printf("ERROR: Not defined type\n");
				return -1;
		}
        
        free(pathBuff);
	}

	if ( i < 0 ) {
		return i;
	}
	return end;
}


int json2map_parseObject(json2map_t *obj, char *path, char *jsonString, jsmntok_t *token, int start, int end) {
	int newEnd;
	char buffer[BUFFER_LENGTH];
	char *pathBuff = NULL;

	int i = start;
	while ( i < end && i > 0) {
		if ( token[i].type == JSMN_STRING ) {
			memset(buffer, '\0', BUFFER_LENGTH);
			json2map_setTokenValue(jsonString, &token[i], buffer);	

			pathBuff = json2map_concatPaths(path, buffer, -1);

			memset(buffer, '\0', BUFFER_LENGTH);
		} else {
			printf("ERROR: Name of object has to be a string\n");
			return -1;
		}
		i++;

		switch ( token[i].type ) {
			case JSMN_OBJECT:
				newEnd = json2map_calcEnd(token, i, end);
				i = json2map_parseObject(obj, pathBuff, jsonString, token, i + 1, newEnd + 1);
				break;
			case JSMN_STRING:
			case JSMN_PRIMITIVE:
				memset(buffer, '\0', BUFFER_LENGTH);
				json2map_setTokenValue(jsonString, &token[i], buffer);	
				obj->hookMethod(obj->hookMethodData, pathBuff, buffer);
				memset(buffer, '\0', BUFFER_LENGTH);
				i++;
				break;
			case JSMN_ARRAY:
				newEnd = json2map_calcEnd(token, i, end);
				i = json2map_parseArray(obj, pathBuff, jsonString, token, i + 1, newEnd + 1);
				break;
			default:
				printf("ERROR: Not defined type\n");
				return -1;
		}
        free(pathBuff);
	}
    
	if ( i < 0 ) {
		return i;
	}
	return end;
}


int json2map_parse(json2map_t *obj, char *jsonString) {
	jsmn_parser p;
	jsmntok_t token[MAX_JSON_TOKENS];

	jsmn_init(&p);

	int count = jsmn_parse(&p, jsonString, strlen(jsonString), token, MAX_JSON_TOKENS);
	if ( count < 0 ) {
		printf("ERROR: no object found\n");
		return -1;
	}

	if ( count < 1 || token[0].type != JSMN_OBJECT) {
		printf("ERROR: first found object needs to be a valid object\n");
		return -1;
	}

	return json2map_parseObject(obj, NULL, jsonString, token, 1, count);
}


void json2map_registerHook(json2map_t *obj, void *data, void* method ) {
	obj->hookMethod = method;
	obj->hookMethodData = data;
}

