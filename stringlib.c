#include <stdio.h>
#include <stdlib.h>
#include "stringlib.h"


char *stringlib_getToken(stringlib_tokens_t *token, char *str) {
	char *buffer;

	buffer = calloc(sizeof(char), token->length + 1);
	memcpy(buffer, str + token->start, token->length);

	return buffer;
}


stringlib_tokens_t *stringlib_splitTokens(char *str, char chr) {
	char *lastPos;
	char *newPos;

	stringlib_tokens_t *tokens = NULL;
	stringlib_tokens_t *currentToken;
	stringlib_tokens_t *newToken;

	newPos = str;
	lastPos = str;

	while ( *newPos != '\0' ) {
		if ( *newPos == chr ) {
			newToken = (stringlib_tokens_t *) malloc(sizeof(stringlib_tokens_t));
			newToken->next = NULL;

			if ( tokens == NULL ) {
				tokens = newToken;
				currentToken = newToken;
			} else {
				currentToken->next = newToken;
				currentToken = newToken;
			}

			currentToken->start = ( lastPos - str );
			currentToken->length = ( newPos - lastPos );
			lastPos = newPos + 1;
		}
		newPos++;
	}
	if ( newPos != str ) {
		newToken = (stringlib_tokens_t *) malloc(sizeof(stringlib_tokens_t));
		newToken->next = NULL;

		if ( tokens == NULL ) {
			tokens = newToken;
			currentToken = newToken;
		} else {
			currentToken->next = newToken;
			currentToken = newToken;
		}

		currentToken->start = ( lastPos - str );
		currentToken->length = ( newPos - lastPos );
	}
	return tokens;
}

void stringlib_freeTokens(stringlib_tokens_t *tokens) {
	stringlib_tokens_t *token;
	while ( tokens != NULL ) {
		token = tokens;
		tokens = tokens->next;
		free(token);
	}
}

int stringlib_isInteger(char *str) {
	char *ptr = str;

	if ( *ptr == '\0' ) {
		return 0;
	}

	while ( *ptr != '\0' ) {
		if ( ( *ptr < 48 || *ptr > 57 ) && *ptr != '.' ) {
			return 0;
		}
		ptr++;
	}
	return 1;
}

char *stringlib_longToString(char *buffer, long value) {
	sprintf(buffer, "%ld", value);
	return buffer;
}
