#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jsmn.h"
#define STRING_STD_LENGTH 1<<10
#define JSON_MAX_TOKENS 128


void json_setTokenValue(char *jsonString, jsmntok_t *token, char *buffer) {
	memcpy(buffer, jsonString + token->start, token->end - token->start);
}


/*
void json_calcPath(char *jsonString, jsmntok_t token[], int point, char *tokenPath) {

	char tmp[STRING_STD_LENGTH];

	if ( point < 0 ) {
		return;
	}

	memset(tmp, '\0', STRING_STD_LENGTH);

	json_calcPath(jsonString, token, token[point].parent - 1, tokenPath);

	if ( tokenPath[0] != '\0') {
		strcat(tokenPath, ".");
	}
	json_setTokenValue(jsonString, &token[point], tmp);
	strcat(tokenPath, tmp);
	
}


char *json_parseStringObject (char *jsonString, jsmntok_t *token, int pos ) {
	char *tokenValue = NULL;
	if ( token[pos].type == JSMN_STRING ) {
		tokenValue = (char *) calloc ( token[pos].end - token[pos].start + 1, sizeof(char) );
		json_setTokenValue(jsonString, &token[pos], tokenValue);
	}
	return tokenValue;
}


void json_flatten(char *jsonString, jsmntok_t *token, int count) {
	char tokenName[STRING_STD_LENGTH];
	char *tokenValue;
	char tokenPath[STRING_STD_LENGTH];

	memset(tokenName, '\0', STRING_STD_LENGTH);
	memset(tokenPath, '\0', STRING_STD_LENGTH);

	for ( int i = 1; i < count; i++ ) {
		if ( token[i].type == JSMN_STRING ) {
			json_setTokenValue(jsonString, &token[i], tokenName);
			json_calcPath(jsonString, token, i, tokenPath);

			tokenValue = json_parseStringObject(jsonString, token, i + 1);

			//json_setTokenValue(jsonString, &token[i + 1], tokenValue);
			i++;		
		}

		if ( token[i].type == JSMN_ARRAY ) {
			printf("HAHA\n");
			printf("%d\t%d\n", token[i].start, token[i].end);
		}
		if ( token[i].type == JSMN_OBJECT ) {
			printf("HUHU\n");
			printf("%d\t%d\n", token[i].start, token[i].end);
		}

		printf("tokenName: %s\ntokenValue: %s\ntokenPath: %s\n\n", tokenName, tokenValue, tokenPath);
		memset(tokenName, '\0', STRING_STD_LENGTH);
		free(tokenValue);
		memset(tokenPath, '\0', STRING_STD_LENGTH);

	}
}
//*/

int json_calc_end(jsmntok_t *token, int start, int end) {
	// TODO: optimize search
	for ( int i = start + 1; i < end; i++ ) {
		if ( token[i].start > token[start].end) {
			return i - 1;
		}
	}
	return end - 1;
}


void json_parse_array(char *jsonString, jsmntok_t *token, int start, int end) {
	int i;
	char buffer[STRING_STD_LENGTH];
	int newEnd;
printf ("array\n");
	for ( i = start; i < end; i++ ) {
		if ( token[i].type == JSMN_OBJECT ) {
			/*newEnd = json_calc_end(token, i, end);
			json_parse_object(jsonString, token, i, newEnd);
			i = newEnd; */
		} else if ( token[i].type == JSMN_ARRAY ) {
			newEnd = json_calc_end(token, i, end);
			json_parse_array(jsonString, token, i + 1, newEnd);
			i = newEnd;
		} else if ( token[i].type == JSMN_STRING ) {
			json_setTokenValue(jsonString, &token[i], buffer);
			printf("%s\n", buffer);
			memset(buffer, '\0', STRING_STD_LENGTH);
		} else {
			printf("dump\n");
		}
	}
}

void json_parse_object(char *jsonString, jsmntok_t *token, int start, int end) {
	int newEnd;
	int i;
	char buffer[STRING_STD_LENGTH];

	for ( i = start; i < end; i++ ) {
		printf("%d\n", i);
		if ( token[i].type == JSMN_OBJECT ) {
			newEnd = json_calc_end(token, i, end);
			json_parse_object(jsonString, token, i + 1, newEnd);
			i = newEnd;
		} else if ( token[i].type == JSMN_ARRAY ) {
			newEnd = json_calc_end(token, i, end);
			json_parse_array(jsonString, token, i + 1, newEnd);
			i = newEnd;
		} else if ( token[i].type == JSMN_STRING ) {
			json_setTokenValue(jsonString, &token[i], buffer);
			printf("%s\n", buffer);
			memset(buffer, '\0', STRING_STD_LENGTH);
		} else {
			printf("dump\n");
		}
	}
}

void json_parse(char *jsonString) {
	jsmn_parser p;
	jsmntok_t token[JSON_MAX_TOKENS];

	jsmn_init(&p);

	int count = jsmn_parse(&p, jsonString, strlen(jsonString), token, JSON_MAX_TOKENS);
	if ( count < 0 ) {
		return;
	}

	if ( count < 1 || token[0].type != JSMN_OBJECT) {
		return;
	}

	json_parse_object(jsonString, token, 1, count);
}


int main(int argc, char **argv) {
	printf("go\n");
	json_parse("{ \"test\": { \"_id\" : { \"$oid\" : \"566950d1afc4a3c1d86fcdfb\" } }, \"name\" : \"picture\", \"file\" : \"/var/www/html/pictureIn.png\", \"moep\": [\"a\", \"b\", \"c\", \"d\"] }");
	printf("end\n");
	return 0;
}