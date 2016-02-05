#ifndef __JSONTABLE_H__
#define __JSONTABLE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jsmn.h"
#include "config.h"

int jsontable_parse(char *jsonString);
void jsontable_registerHook( void *data, void* method );
void jsontable_setTokenValue(char *jsonString, jsmntok_t *token, char *buffer);
char *jsontable_concatPaths(char *parent, char *key, int arrayIdx);
int jsontable_calcEnd(jsmntok_t *token, int start, int end);
int jsontable_parseArray(char *path, char *jsonString, jsmntok_t *token, int start, int end);
int jsontable_parseObject(char *path, char *jsonString, jsmntok_t *token, int start, int end);

#endif