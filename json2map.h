#ifndef __JSON2MAP_H__
#define __JSON2MAP_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jsmn.h"

typedef struct {
	void (* hookMethod) (void *data, char *key, char *value);
	void *hookMethodData;
} json2map_t;


json2map_t *json2map_init();
int json2map_parse(json2map_t *obj, char *jsonString);
void json2map_registerHook(json2map_t *obj, void *data, void* method );
void json2map_setTokenValue(char *jsonString, jsmntok_t *token, char *buffer);
char *json2map_concatPaths(char *parent, char *key, int arrayIdx);
int json2map_calcEnd(jsmntok_t *token, int start, int end);
int json2map_parseArray(json2map_t *obj, char *path, char *jsonString, jsmntok_t *token, int start, int end);
int json2map_parseObject(json2map_t *obj, char *path, char *jsonString, jsmntok_t *token, int start, int end);

#endif