#ifndef __STRINGLIB_H__
#define __STRINGLIB_H__

#include "string.h"

typedef struct {
	long start;
	long length;
} stringlib_tokens_t;

void stringlib_getToken(stringlib_tokens_t *token, char *str, char *buffer);
int stringlib_getIndexOf(char *str, char chr);
int stringlib_splitTokens(stringlib_tokens_t *tokens, char *str, char chr, int maxCount);
int stringlib_isInteger(char *str);

#endif