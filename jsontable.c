#include "jsontable.h"

static void (* hookMethod) (void *data, char *key, char *value);
static void *hookMethodData;

int json_parse_object(char *path, char *jsonString, jsmntok_t *token, int start, int end);


void json_setTokenValue(char *jsonString, jsmntok_t *token, char *buffer) {
	memcpy(buffer, jsonString + token->start, token->end - token->start);
}


char *json_concatPaths(char *parent, char *key, int arrayIdx) {

	// TODO: organize code a little nicer
	char *path;
	char arrayIdxBuff[10];
	int arrayIdLen = 0;

	int keyLen = strlen(key);

	if ( arrayIdx >= 0 ) {
		sprintf(arrayIdxBuff, "[%d]", arrayIdx);
		arrayIdLen += strlen(arrayIdxBuff);
	}

	if ( parent == NULL ) {
		path = (char *) calloc(sizeof(char), keyLen + arrayIdLen + 1);
		memcpy(path, key, keyLen);

		if ( arrayIdx >= 0 ) {
			memcpy(path + keyLen, arrayIdxBuff, arrayIdLen);
		}
	} else if ( key == NULL ) {
		int parentLen = strlen(parent);
		path = (char *) calloc(sizeof(char), parentLen + arrayIdLen + 1);
		memcpy(path, parent, parentLen);

		if ( arrayIdx >= 0 ) {
			memcpy(path + parentLen, arrayIdxBuff, arrayIdLen);
		} 
	} else {
		int parentLen = strlen(parent);
		path = (char *) calloc(sizeof(char), parentLen + keyLen + arrayIdLen + 2);
		memcpy(path, parent, parentLen);
		path[parentLen] = '.';
		memcpy(path + parentLen + 1, key, keyLen);

		if ( arrayIdx >= 0 ) {
			memcpy(path + parentLen + keyLen + 1, arrayIdxBuff, arrayIdLen);
		}
	}
	return path;
}


int json_calc_end(jsmntok_t *token, int start, int end) {
	// TODO: optimize search
	for ( int i = start + 1; i < end; i++ ) {
		if ( token[i].start > token[start].end) {
			return i - 1;
		}
	}
	return end - 1;
}


int json_parse_array(char *path, char *jsonString, jsmntok_t *token, int start, int end) {
	int newEnd;
	char buffer[STRING_STD_LENGTH];
	char *pathBuff;
	int count = 0;

	int i = start;
	while ( i < end && i > 0) {

		pathBuff = json_concatPaths(NULL, path, count);
		count++;

		switch ( token[i].type ) {
			case JSMN_OBJECT:
				newEnd = json_calc_end(token, i, end);
				i = json_parse_object(pathBuff, jsonString, token, i + 1, newEnd + 1);
				break;
			case JSMN_STRING:
			case JSMN_PRIMITIVE:
				memset(buffer, '\0', STRING_STD_LENGTH);
				json_setTokenValue(jsonString, &token[i], buffer);	
				hookMethod(hookMethodData, pathBuff, buffer);
				memset(buffer, '\0', STRING_STD_LENGTH);
				i++;
				break;
			default:
				printf("ERROR: Not defined type\n");
				return -1;
		}
		free(pathBuff);
	}

	if ( i < 0 ) {
		return i;
	}
	return end;
}


int json_parse_object(char *path, char *jsonString, jsmntok_t *token, int start, int end) {
	int newEnd;
	char buffer[STRING_STD_LENGTH];
	char *pathBuff;

	int i = start;
	while ( i < end && i > 0) {
		if ( token[i].type == JSMN_STRING ) {
			memset(buffer, '\0', STRING_STD_LENGTH);
			json_setTokenValue(jsonString, &token[i], buffer);	

			pathBuff = json_concatPaths(path, buffer, -1);

			memset(buffer, '\0', STRING_STD_LENGTH);
		} else {
			printf("ERROR: Name of object has to be a string\n");
			return -1;
		}
		i++;

		switch ( token[i].type ) {
			case JSMN_OBJECT:
				newEnd = json_calc_end(token, i, end);
				i = json_parse_object(pathBuff, jsonString, token, i + 1, newEnd + 1);
				break;
			case JSMN_STRING:
			case JSMN_PRIMITIVE:
				memset(buffer, '\0', STRING_STD_LENGTH);
				json_setTokenValue(jsonString, &token[i], buffer);	
				hookMethod(hookMethodData, pathBuff, buffer);
				memset(buffer, '\0', STRING_STD_LENGTH);
				i++;
				break;
			case JSMN_ARRAY:
				newEnd = json_calc_end(token, i, end);
				i = json_parse_array(pathBuff, jsonString, token, i + 1, newEnd + 1);
				break;
			default:
				printf("ERROR: Not defined type\n");
				return -1;
		}
	}

	free(pathBuff);
	if ( i < 0 ) {
		return i;
	}
	return end;
}


int json_parse(char *jsonString) {
	jsmn_parser p;
	jsmntok_t token[JSON_MAX_TOKENS];

	jsmn_init(&p);

	int count = jsmn_parse(&p, jsonString, strlen(jsonString), token, JSON_MAX_TOKENS);
	if ( count < 0 ) {
		return -1;
	}

	if ( count < 1 || token[0].type != JSMN_OBJECT) {
		return -1;
	}

	return json_parse_object(NULL, jsonString, token, 1, count);
}


void json_registerHook( void *data, void* method ) {
	hookMethod = method;
	hookMethodData = data;
}


void json_hookMethod(void *data, char *key, char *value) {
	printf("%s = %s\n", key, value);
}


int main(int argc, char **argv) {
	json_registerHook(NULL, &json_hookMethod);
	json_parse("{ \"test\": { \"_id\" : { \"$oid\" : \"566950d1afc4a3c1d86fcdfb\" } }, \"name\" : \"picture\", \"file\" : \"/var/www/html/pictureIn.png\", \"moep\": [\"a\", \"b\", \"c\", \"d\", { \"test\": \"blubb\"}], \"bla\": null, \"number\": 1234, \"true\": true, \"false\": false }");
	return 0;
}