#include "json2map.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jsmn.h"
#include "debugging.h"
#include "stringlib.h"

void json2map_setTokenValue(char *jsonString, jsmntok_t *token, char *buffer);

char *json2map_concatPaths(char *parent, char *key, int arrayIdx);

int json2map_calcEnd(jsmntok_t *token, int start, int end);

int json2map_parseArray(json2map_t *obj, char *path, char *jsonString, jsmntok_t *token, int start, int end);

int json2map_parseObject(json2map_t *obj, char *path, char *jsonString, jsmntok_t *token, int start, int end);



json2map_t *json2map_init() {
	DEBUG_PUT("json2map_init()... ");
	json2map_t *obj = (json2map_t *) malloc(sizeof(json2map_t));
	DEBUG_PUT("json2map_init()... DONE");
	return obj;
}

void json2map_destroy(json2map_t *obj) {
	DEBUG_PUT("json2map_destroy()... ");
	free(obj);
	DEBUG_PUT("json2map_destroy()... DONE");
}


void json2map_setTokenValue(char *jsonString, jsmntok_t *token, char *buffer) {
	DEBUG_TEXT("json2map_setTokenValue(%s, [jsmntok_t *], %s)... ", jsonString, buffer);
	memcpy(buffer, jsonString + token->start, token->end - token->start);
	DEBUG_TEXT("json2map_setTokenValue(%s, [jsmntok_t *], %s)... DONE", jsonString, buffer);
}


char *json2map_concatPaths(char *parent, char *key, int arrayIdx) {
	DEBUG_TEXT("json2map_concatPaths(%s, %s, %d)...", parent, key, arrayIdx);
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
		sprintf(arrayIdxBuff, "%c%d%c", JSON2MAP_MAP_ARRAY_START, arrayIdx, JSON2MAP_MAP_ARRAY_END);
		arrayIdLen += strlen(arrayIdxBuff);
	}
	if ( arrayIdx == -2 ) {
		sprintf(arrayIdxBuff, "%c%c%c", JSON2MAP_MAP_ARRAY_START, JSON2MAP_MAP_ARRAY_COUNT, JSON2MAP_MAP_ARRAY_END);
		arrayIdLen += strlen(arrayIdxBuff);
	}
	

	path = (char *) calloc(sizeof(char), parentLen + keyLen + arrayIdLen + addition + 1);
	memcpy(path, parent, parentLen);

	if ( parentLen && keyLen ) {
		path[parentLen] = JSON2MAP_MAP_OBJECT_SEPARATOR;
	}

	memcpy(path + parentLen + addition, key, keyLen);

	if ( arrayIdx >= 0 || arrayIdx == -2 ) {
		memcpy(path + parentLen + keyLen + addition, arrayIdxBuff, arrayIdLen);
	}

	DEBUG_TEXT("json2map_concatPaths(%s, %s, %d)... DONE", parent, key, arrayIdx);
	return path;
}


int json2map_calcEnd(jsmntok_t *token, int start, int end) {
	// TODO: optimize search
	DEBUG_TEXT("json2map_calcEnd([jsmntok_t *], %d, %d)...", start, end);
	int i;
	for ( i = start + 1; i < end; i++ ) {
		if ( token[i].start > token[start].end ) {
			DEBUG_TEXT("json2map_calcEnd([jsmntok_t *], %d, %d): return value=%d", start, end, i - 1);
			DEBUG_TEXT("json2map_calcEnd([jsmntok_t *], %d, %d)... DONE", start, end);
			return i - 1;
		}
	}
	DEBUG_TEXT("json2map_calcEnd([jsmntok_t *], %d, %d): return value=%d", start, end, end - 1);
	DEBUG_TEXT("json2map_calcEnd([jsmntok_t *], %d, %d)... DONE", start, end);
	return end - 1;
}


int json2map_parseArray(json2map_t *obj, char *path, char *jsonString, jsmntok_t *token, int start, int end) {
	DEBUG_TEXT("json2map_parseArray([json2map_t *], %s, %s, [jsmntok_t *], %d, %d)...", path, jsonString, start, end);
	int newEnd;
	char buffer[JSON2MAP_BUFFER_LENGTH];
	char *pathBuff;
	int count = 0;

	int i = start;
	while ( i < end && i > 0 ) {

		pathBuff = json2map_concatPaths(NULL, path, count);
		count++;

		switch ( token[i].type ) {
			case JSMN_OBJECT:
				newEnd = json2map_calcEnd(token, i, end);
				i = json2map_parseObject(obj, pathBuff, jsonString, token, i + 1, newEnd + 1);
				break;
			case JSMN_STRING:
			case JSMN_PRIMITIVE:
				memset(buffer, '\0', JSON2MAP_BUFFER_LENGTH);
				json2map_setTokenValue(jsonString, &token[i], buffer);
				obj->hookMethod(obj->hookMethodData, pathBuff, buffer);
				memset(buffer, '\0', JSON2MAP_BUFFER_LENGTH);
				i++;
				break;
			default:
				DEBUG_TEXT("json2map_parseArray([json2map_t *], %s, %s, [jsmntok_t *], %d, %d): ERROR: Not defined type", path, jsonString, start, end);
				return -1;
		}

		free(pathBuff);
	}

	pathBuff = json2map_concatPaths(NULL, path, -2);

	stringlib_longToString(buffer, count);
	obj->hookMethod(obj->hookMethodData, pathBuff, buffer);
	memset(buffer, '\0', JSON2MAP_BUFFER_LENGTH);
	free(pathBuff);


	DEBUG_TEXT("json2map_parseArray([json2map_t *], %s, %s, [jsmntok_t *], %d, %d)... DONE", path, jsonString, start, end);
	if ( i < 0 ) {
		return i;
	}
	return end;
}


int json2map_parseObject(json2map_t *obj, char *path, char *jsonString, jsmntok_t *token, int start, int end) {
	DEBUG_TEXT("json2map_parseObject([json2map_t *], %s, %s, [jsmntok_t *], %d, %d)...", path, jsonString, start, end);

	int newEnd;
	char buffer[JSON2MAP_BUFFER_LENGTH];
	char *pathBuff = NULL;

	int i = start;
	while ( i < end && i > 0 ) {
		if ( token[i].type == JSMN_STRING ) {
			memset(buffer, '\0', JSON2MAP_BUFFER_LENGTH);
			json2map_setTokenValue(jsonString, &token[i], buffer);

			pathBuff = json2map_concatPaths(path, buffer, -1);

			memset(buffer, '\0', JSON2MAP_BUFFER_LENGTH);
		} else {
			DEBUG_TEXT("json2map_parseObject([json2map_t *], %s, %s, [jsmntok_t *], %d, %d): ERROR: Name of object has to be a string", path, jsonString, start, end);
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
				memset(buffer, '\0', JSON2MAP_BUFFER_LENGTH);
				json2map_setTokenValue(jsonString, &token[i], buffer);
				obj->hookMethod(obj->hookMethodData, pathBuff, buffer);
				memset(buffer, '\0', JSON2MAP_BUFFER_LENGTH);
				i++;
				break;
			case JSMN_ARRAY:
				newEnd = json2map_calcEnd(token, i, end);
				i = json2map_parseArray(obj, pathBuff, jsonString, token, i + 1, newEnd + 1);
				break;
			default:
				DEBUG_TEXT("json2map_parseObject([json2map_t *], %s, %s, [jsmntok_t *], %d, %d): ERROR: Not defined type", path, jsonString, start, end);
				return -1;
		}
		free(pathBuff);
	}

	DEBUG_TEXT("json2map_parseObject([json2map_t *], %s, %s, [jsmntok_t *], %d, %d)... DONE", path, jsonString, start, end);
	if ( i < 0 ) {
		return i;
	}
	return end;
}


int json2map_parse(json2map_t *obj, char *jsonString) {
	DEBUG_TEXT("json2map_parseObject([json2map_t *], %s)... ", jsonString);

	jsmn_parser p;
	jsmntok_t token[JSON2MAP_MAX_JSON_TOKENS];

	jsmn_init(&p);

	int count = jsmn_parse(&p, jsonString, strlen(jsonString), token, JSON2MAP_MAX_JSON_TOKENS);
	if ( count < 0 ) {
		DEBUG_TEXT("json2map_parseObject([json2map_t *], %s): ERROR: no object found", jsonString);
		return -1;
	}

	if ( count < 1 || token[0].type != JSMN_OBJECT ) {
		DEBUG_TEXT("json2map_parseObject([json2map_t *], %s): ERROR: first object needs to be a valid object", jsonString);
		return -1;
	}

	DEBUG_TEXT("json2map_parseObject([json2map_t *], %s)... DONE", jsonString);
	return json2map_parseObject(obj, NULL, jsonString, token, 1, count);
}


void json2map_registerHook(json2map_t *obj, void *data, void *method) {
	DEBUG_PUT("json2map_registerHook([json2map_t *], [void *] [void *])... ");
	obj->hookMethod = method;
	obj->hookMethodData = data;
	DEBUG_PUT("json2map_registerHook([json2map_t *], [void *] [void *])... DONE");
}

