#ifndef __JSON2MAP_H__
#define __JSON2MAP_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jsmn.h"
#include "config.h"

int json2map_parse(char *jsonString);
void json2map_registerHook( void *data, void* method );
void json2map_setTokenValue(char *jsonString, jsmntok_t *token, char *buffer);
char *json2map_concatPaths(char *parent, char *key, int arrayIdx);
int json2map_calcEnd(jsmntok_t *token, int start, int end);
int json2map_parseArray(char *path, char *jsonString, jsmntok_t *token, int start, int end);
int json2map_parseObject(char *path, char *jsonString, jsmntok_t *token, int start, int end);

#endif