#include "jsontable.h"

static void (* jsontable_hookMethod) (void *data, char *key, char *value);
static void *jsontable_hookMethodData;


void jsontable_setTokenValue(char *jsonString, jsmntok_t *token, char *buffer) {
	memcpy(buffer, jsonString + token->start, token->end - token->start);
}


char *jsontable_concatPaths(char *parent, char *key, int arrayIdx) {
	// TODO: organize code a little nicer
	char *path;
	char arrayIdxBuff[10];
	int arrayIdLen = 0;

	long keyLen = strlen(key);

	if ( arrayIdx >= 0 ) {
		sprintf(arrayIdxBuff, "[%d]", arrayIdx);
		arrayIdLen += strlen(arrayIdxBuff);
	}

	if ( parent == NULL ) {
		path = (char *) calloc(sizeof(char), keyLen + arrayIdLen + 1);
		memcpy(path, key, keyLen);

		if ( arrayIdx >= 0 ) {
			memcpy(path + keyLen, arrayIdxBuff, arrayIdLen);
		}
        return path;
	}
    if ( key == NULL ) {
		long parentLen = strlen(parent);
		path = (char *) calloc(sizeof(char), parentLen + arrayIdLen + 1);
		memcpy(path, parent, parentLen);

		if ( arrayIdx >= 0 ) {
			memcpy(path + parentLen, arrayIdxBuff, arrayIdLen);
		}
        return path;
	}
    
    long parentLen = strlen(parent);
    path = (char *) calloc(sizeof(char), parentLen + keyLen + arrayIdLen + 2);
    memcpy(path, parent, parentLen);
    path[parentLen] = '.';
    memcpy(path + parentLen + 1, key, keyLen);

    if ( arrayIdx >= 0 ) {
        memcpy(path + parentLen + keyLen + 1, arrayIdxBuff, arrayIdLen);
    }
	return path;
}


int jsontable_calcEnd(jsmntok_t *token, int start, int end) {
	// TODO: optimize search
	int i;
	for ( i = start + 1; i < end; i++ ) {
		if ( token[i].start > token[start].end) {
			return i - 1;
		}
	}
	return end - 1;
}


int jsontable_parseArray(char *path, char *jsonString, jsmntok_t *token, int start, int end) {
	int newEnd;
	char buffer[STRING_STD_LENGTH];
	char *pathBuff;
	int count = 0;

	int i = start;
	while ( i < end && i > 0) {

		pathBuff = jsontable_concatPaths(NULL, path, count);
		count++;

		switch ( token[i].type ) {
			case JSMN_OBJECT:
				newEnd = jsontable_calcEnd(token, i, end);
				i = jsontable_parseObject(pathBuff, jsonString, token, i + 1, newEnd + 1);
				break;
			case JSMN_STRING:
			case JSMN_PRIMITIVE:
				memset(buffer, '\0', STRING_STD_LENGTH);
				jsontable_setTokenValue(jsonString, &token[i], buffer);	
				jsontable_hookMethod(jsontable_hookMethodData, pathBuff, buffer);
				memset(buffer, '\0', STRING_STD_LENGTH);
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


int jsontable_parseObject(char *path, char *jsonString, jsmntok_t *token, int start, int end) {
	int newEnd;
	char buffer[STRING_STD_LENGTH];
	char *pathBuff = NULL;

	int i = start;
	while ( i < end && i > 0) {
		if ( token[i].type == JSMN_STRING ) {
			memset(buffer, '\0', STRING_STD_LENGTH);
			jsontable_setTokenValue(jsonString, &token[i], buffer);	

			pathBuff = jsontable_concatPaths(path, buffer, -1);

			memset(buffer, '\0', STRING_STD_LENGTH);
		} else {
			printf("ERROR: Name of object has to be a string\n");
			return -1;
		}
		i++;

		switch ( token[i].type ) {
			case JSMN_OBJECT:
				newEnd = jsontable_calcEnd(token, i, end);
				i = jsontable_parseObject(pathBuff, jsonString, token, i + 1, newEnd + 1);
				break;
			case JSMN_STRING:
			case JSMN_PRIMITIVE:
				memset(buffer, '\0', STRING_STD_LENGTH);
				jsontable_setTokenValue(jsonString, &token[i], buffer);	
				jsontable_hookMethod(jsontable_hookMethodData, pathBuff, buffer);
				memset(buffer, '\0', STRING_STD_LENGTH);
				i++;
				break;
			case JSMN_ARRAY:
				newEnd = jsontable_calcEnd(token, i, end);
				i = jsontable_parseArray(pathBuff, jsonString, token, i + 1, newEnd + 1);
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


int jsontable_parse(char *jsonString) {
	jsmn_parser p;
	jsmntok_t token[JSON_MAX_TOKENS];

	jsmn_init(&p);

	int count = jsmn_parse(&p, jsonString, strlen(jsonString), token, JSON_MAX_TOKENS);
	if ( count < 0 ) {
		printf("ERROR: no object found\n");
		return -1;
	}

	if ( count < 1 || token[0].type != JSMN_OBJECT) {
		printf("ERROR: first found object needs to be a valid object\n");
		return -1;
	}

	return jsontable_parseObject(NULL, jsonString, token, 1, count);
}


void jsontable_registerHook( void *data, void* method ) {
	jsontable_hookMethod = method;
	jsontable_hookMethodData = data;
}

