#include "json2map.h"
#include "config.h"

#include <stdio.h>
#include <csafestring.h>
#include "jsmn.h"
#include "debugging.h"
#include "stringlib.h"

static char *json2map_setTokenValue(char *jsonString, jsmntok_t *token);
static csafestring_t *json2map_concatPaths(char *parent, char *key, int arrayIdx);
static int json2map_calcEnd(jsmntok_t *token, int start, int end);
static int json2map_parseArray(json2map_t *obj, char *path, char *jsonString, jsmntok_t *token, int start, int end);
static int json2map_parseObject(json2map_t *obj, char *path, char *jsonString, jsmntok_t *token, int start, int end);

json2map_t *json2map_init(char saveSubobjectString) {
	DEBUG_PUT("json2map_init()... ");
	json2map_t *obj = (json2map_t *) malloc(sizeof(json2map_t));
	obj->dataMapData = NULL;
	obj->dataMapHook = NULL;
	obj->saveSubobjectString = saveSubobjectString;
	DEBUG_PUT("json2map_init()... DONE");
	return obj;
}

void json2map_destroy(json2map_t *obj) {
	DEBUG_PUT("json2map_destroy()... ");
	free(obj);
	DEBUG_PUT("json2map_destroy()... DONE");
}


int json2map_parse(json2map_t *obj, char *prefix, char *jsonString) {
	DEBUG_TEXT("json2map_parseObject([json2map_t *], %s)... ", jsonString);

	jsmn_parser p;
	jsmntok_t *token;

	if ( jsonString == NULL ) {
		return -1;
	}

	jsmn_init(&p);

	int count = jsmn_parse(&p, jsonString, strlen(jsonString), NULL, 0);
	DEBUG_TEXT("json2map_parseObject([json2map_t *], %s): Count: %d", jsonString, count);
	if ( count < 0 ) {
		DEBUG_TEXT("json2map_parseObject([json2map_t *], %s): ERROR: no object found", jsonString);
		return -1;
	}

	token = (jsmntok_t *) malloc(sizeof(jsmntok_t) * count);
	p.pos = 0;
	jsmn_parse(&p, jsonString, strlen(jsonString), token, count);

	if ( count < 1 || token[0].type != JSMN_OBJECT ) {
		DEBUG_TEXT("json2map_parseObject([json2map_t *], %s): ERROR: first object needs to be a valid object", jsonString);
		return -1;
	}

	DEBUG_TEXT("json2map_parseObject([json2map_t *], %s)... DONE", jsonString);
	int retVal = json2map_parseObject(obj, prefix, jsonString, token, 1, count);
	free(token);
	return retVal;
}


void json2map_registerDataHook(json2map_t *obj, void *data, void *method) {
	DEBUG_PUT("json2map_registerDataHook([json2map_t *], [void *] [void *])... ");
	obj->dataMapHook = method;
	obj->dataMapData = data;
	DEBUG_PUT("json2map_registerDataHook([json2map_t *], [void *] [void *])... DONE");
}

static char *json2map_setTokenValue(char *jsonString, jsmntok_t *token) {
	DEBUG_TEXT("json2map_setTokenValue(%s, [jsmntok_t *])... ", jsonString);
	char *buffer;
	buffer = calloc(sizeof(char), token->end - token->start + 1);
	memcpy(buffer, jsonString + token->start, token->end - token->start);
	DEBUG_TEXT("json2map_setTokenValue(%s, [jsmntok_t *])... DONE", jsonString);
	return buffer;
}


static csafestring_t *json2map_concatPaths(char *parent, char *key, int arrayIdx) {
	DEBUG_TEXT("json2map_concatPaths(%s, %s, %d)...", parent, key, arrayIdx);

	char arrayIdxBuff[10];
	csafestring_t *buffer = safe_create(NULL);

	if ( parent != NULL && *parent != '\0' ) {
		safe_strcpy(buffer, parent);
		safe_strchrappend(buffer, JSON2MAP_MAP_OBJECT_SEPARATOR);
		safe_strcat(buffer, key);
	} else {
		safe_strcpy(buffer, key);
	}

	if ( arrayIdx >= 0 ) {
		sprintf(arrayIdxBuff, "%c%d%c", JSON2MAP_MAP_ARRAY_START, arrayIdx, JSON2MAP_MAP_ARRAY_END);
		safe_strcat(buffer, arrayIdxBuff);
	}
	if ( arrayIdx == -2 ) {
		sprintf(arrayIdxBuff, "%c%c%c", JSON2MAP_MAP_ARRAY_START, JSON2MAP_MAP_ARRAY_COUNT, JSON2MAP_MAP_ARRAY_END);
		safe_strcat(buffer, arrayIdxBuff);
	}

	DEBUG_TEXT("json2map_concatPaths(%s, %s, %d)... DONE", parent, key, arrayIdx);
	return buffer;
}


static int json2map_calcEnd(jsmntok_t *token, int start, int end) {
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


static int json2map_parseArray(json2map_t *obj, char *path, char *jsonString, jsmntok_t *token, int start, int end) {
	DEBUG_TEXT("json2map_parseArray([json2map_t *], %s, %s, [jsmntok_t *], %d, %d)...", path, jsonString, start, end);
	int newEnd;
	char *buffer;
	csafestring_t *pathBuff;
	csafestring_t *objPathBuff;
	int count = 0;

	int i = start;
	while ( i < end && i > 0 ) {

		pathBuff = json2map_concatPaths(NULL, path, count);
		count++;

		switch ( token[i].type ) {
			case JSMN_OBJECT:
				newEnd = json2map_calcEnd(token, i, end);

				if ( obj->saveSubobjectString ) {

					objPathBuff = safe_clone(pathBuff);
					safe_strchrappend(objPathBuff, JSON2MAP_MAP_ARRAY_START);
					safe_strchrappend(objPathBuff, JSON2MAP_MAP_OBJECT_CODE);
					safe_strchrappend(objPathBuff, JSON2MAP_MAP_ARRAY_END);

					csafestring_t *tmp = safe_create(NULL);
					safe_memset(tmp, '\0', token[i].end - token[i].start + 1);
					safe_memcpy(tmp, &jsonString[token[i].start], token[i].end - token[i].start);

					obj->dataMapHook(obj->dataMapData, objPathBuff->data, tmp->data);
					safe_destroy(tmp);
					safe_destroy(objPathBuff);
				}

				i = json2map_parseObject(obj, pathBuff->data, jsonString, token, i + 1, newEnd + 1);

				break;
			case JSMN_STRING:
			case JSMN_PRIMITIVE:
				buffer = json2map_setTokenValue(jsonString, &token[i]);
				if ( obj->dataMapHook != NULL ) {
					obj->dataMapHook(obj->dataMapData, pathBuff->data, buffer);
				}
				free(buffer);
				i++;
				break;
			default:
				DEBUG_TEXT("json2map_parseArray([json2map_t *], %s, %s, [jsmntok_t *], %d, %d): ERROR: Not defined type", path, jsonString, start, end);
				return -1;
		}

		safe_destroy(pathBuff);
	}

	pathBuff = json2map_concatPaths(NULL, path, -2);

	char smallBuffer[25];
	stringlib_longToString(smallBuffer, count);
	if ( obj->dataMapHook != NULL ) {
		obj->dataMapHook(obj->dataMapData, pathBuff->data, smallBuffer);
	}
	safe_destroy(pathBuff);

	DEBUG_TEXT("json2map_parseArray([json2map_t *], %s, %s, [jsmntok_t *], %d, %d)... DONE", path, jsonString, start, end);
	if ( i < 0 ) {
		return i;
	}
	return end;
}


static int json2map_parseObject(json2map_t *obj, char *path, char *jsonString, jsmntok_t *token, int start, int end) {
	DEBUG_TEXT("json2map_parseObject([json2map_t *], %s, %s, [jsmntok_t *], %d, %d)...", path, jsonString, start, end);

	int newEnd;
	char *buffer;
	csafestring_t *pathBuff = NULL;
	csafestring_t *objPathBuff = NULL;

	int i = start;
	while ( i < end && i > 0 ) {
		if ( token[i].type == JSMN_STRING ) {
			buffer = json2map_setTokenValue(jsonString, &token[i]);
			pathBuff = json2map_concatPaths(path, buffer, -1);
			free(buffer);
		} else {
			DEBUG_TEXT("json2map_parseObject([json2map_t *], %s, %s, [jsmntok_t *], %d, %d): ERROR: Name of object has to be a string", path, jsonString, start, end);
			return -1;
		}
		i++;

		switch ( token[i].type ) {
			case JSMN_OBJECT:
				newEnd = json2map_calcEnd(token, i, end);

				if ( obj->saveSubobjectString ) {
					objPathBuff = safe_clone(pathBuff);
					safe_strchrappend(objPathBuff, JSON2MAP_MAP_ARRAY_START);
					safe_strchrappend(objPathBuff, JSON2MAP_MAP_OBJECT_CODE);
					safe_strchrappend(objPathBuff, JSON2MAP_MAP_ARRAY_END);
					
					csafestring_t *tmp = safe_create(NULL);
					safe_memset(tmp, '\0', token[i].end - token[i].start + 1);
					safe_memcpy(tmp, &jsonString[token[i].start], token[i].end - token[i].start);

					obj->dataMapHook(obj->dataMapData, objPathBuff->data, tmp->data);
					safe_destroy(tmp);
					safe_destroy(objPathBuff);
				}

				i = json2map_parseObject(obj, pathBuff->data, jsonString, token, i + 1, newEnd + 1);
				break;
			case JSMN_STRING:
			case JSMN_PRIMITIVE:
				buffer = json2map_setTokenValue(jsonString, &token[i]);
				if ( obj->dataMapHook != NULL ) {
					obj->dataMapHook(obj->dataMapData, pathBuff->data, buffer);
				}
				free(buffer);
				i++;
				break;
			case JSMN_ARRAY:
				newEnd = json2map_calcEnd(token, i, end);
				i = json2map_parseArray(obj, pathBuff->data, jsonString, token, i + 1, newEnd + 1);
				break;
			default:
				DEBUG_TEXT("json2map_parseObject([json2map_t *], %s, %s, [jsmntok_t *], %d, %d): ERROR: Not defined type", path, jsonString, start, end);
				return -1;
		}
		safe_destroy(pathBuff);
	}

	DEBUG_TEXT("json2map_parseObject([json2map_t *], %s, %s, [jsmntok_t *], %d, %d)... DONE", path, jsonString, start, end);
	if ( i < 0 ) {
		return i;
	}
	return end;
}

