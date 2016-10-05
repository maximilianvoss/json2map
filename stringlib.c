#include <stdio.h>
#include "stringlib.h"


void stringlib_getToken(stringlib_tokens_t *token, char *str, char *buffer) {
	memcpy(buffer, str + token->start, token->length);
	char *ptr = buffer + token->length;
	*ptr = '\0';
}


int stringlib_splitTokens(stringlib_tokens_t *tokens, char *str, char chr, int maxCount) {
	char *lastPos;
	char *newPos;
	int count = 0;

	newPos = str;
	lastPos = str;

	while ( *newPos != '\0' ) {
		if ( *newPos == chr ) {
			tokens[count].start = ( lastPos - str );
			tokens[count].length = ( newPos - lastPos );
			lastPos = newPos + 1;
			count++;
			if ( count >= maxCount ) {
				return maxCount;
			}
		}
		newPos++;
	}
	if ( newPos != str ) {
		tokens[count].start = ( lastPos - str );
		tokens[count].length = ( newPos - lastPos );
		count++;
	}
	return count;
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
