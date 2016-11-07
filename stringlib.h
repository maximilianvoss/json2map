#ifndef __JSON2MAP_STRINGLIB_H__
#define __JSON2MAP_STRINGLIB_H__

#include "string.h"

typedef struct {
	long start;
	long length;
} stringlib_tokens_t;

char *stringlib_getToken(stringlib_tokens_t *token, char *str);

int stringlib_getIndexOf(char *str, char chr);

int stringlib_splitTokens(stringlib_tokens_t *tokens, char *str, char chr, int maxCount);

int stringlib_isInteger(char *str);
char *stringlib_longToString(char *buffer, long value);

#endif