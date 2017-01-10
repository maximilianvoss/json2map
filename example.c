#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "json2map.h"
#include "map2json.h"
#include "config.h"

#ifdef DEBUG
#include "test.h"
#endif

#define ARRAY_COUNT 16

#define JSON_EXAMPLE "\
{\
	\"test\":{\
		\"_id\":{\
			\"$oid\":\"566950d1afc4a3c1d86fcdfb\"\
		},\
		\"name\":\"picture\",\
		\"file\":\"/var/www/html/pictureIn.png\",\
		\"array\":[\
			{ \
				\"mysubobject\":\"value\",\
				\"secondobject\":0\
			},\
			\"1\",\
			\"b\",\
			\"3\",\
			\"d\",\
			{\
				\"object\":\"test\",\
				\"object2\":\"test2\"\
			}\
		],\
		\"nullpointer\":null,\
		\"number\":1234,\
		\"true\":true,\
		\"false\":false\
	}\
}"


void hookMethod(void *data, char *key, char *value) {
	int i;

	map2json_keyvalue_t *map = (map2json_keyvalue_t *) data;
	for ( i = 0; i < ARRAY_COUNT; i++ ) {
		if ( map[i].key == NULL ) {
			map[i].key = (char *) calloc(sizeof(char), strlen(key) + 1);
			map[i].value = (char *) calloc(sizeof(char), strlen(value) + 1);
			memcpy(map[i].key, key, strlen(key));
			memcpy(map[i].value, value, strlen(value));
			printf("%d: %s = %s\n", i, map[i].key, map[i].value);
			return;
		}
	}
}


int main(int argc, char **argv) {
	map2json_keyvalue_t *map;
	map = (map2json_keyvalue_t *) calloc(sizeof(map2json_keyvalue_t), ARRAY_COUNT);

	json2map_t *json2mapObj = json2map_init();
	json2map_registerHook(json2mapObj, map, &hookMethod);
	json2map_parse(json2mapObj, JSON_EXAMPLE);
	json2map_destroy(json2mapObj);

	printf("\n\n");

	map2json_t *map2jsonObj = map2json_init();
	map2json_push(map2jsonObj, "test.name", "picture");
	map2json_push(map2jsonObj, "test.file", "/var/www/html/pictureIn.png");
	map2json_push(map2jsonObj, "test._id.$oid", "566950d1afc4a3c1d86fcdfb");
//	map2json_push(map2jsonObj, "test.array[0].mysubobject", "value");
//	map2json_push(map2jsonObj, "test.array[0].secondobject", "0");
//	map2json_push(map2jsonObj, "test.array[1]", "1");
//	map2json_push(map2jsonObj, "test.array[2]", "b");
//	map2json_push(map2jsonObj, "test.array[3]", "3");
//	map2json_push(map2jsonObj, "test.array[4]", "d");
//	map2json_push(map2jsonObj, "test.array[5].object", "test");
//	map2json_push(map2jsonObj, "test.array[5].object2", "test2");
//	map2json_push(map2jsonObj, "test.array[x]", "6");
//	map2json_push(map2jsonObj, "test.nullpointer", "null");
//	map2json_push(map2jsonObj, "test.number", "1234");
//	map2json_push(map2jsonObj, "test.true", "true");
//	map2json_push(map2jsonObj, "test.false", "false");
	
	char primitiveMethod[] = " new Function()";
	*primitiveMethod = JSON2MAP_PRIMITIVE_PREFIXER;
	map2json_push(map2jsonObj, "test.fakePrimitive", primitiveMethod);

	printf("%s\n", map2json_create(map2jsonObj));

#ifdef DEBUG
	testValues(map, map2jsonObj->buffer->data);
#endif

	map2json_destroy(map2jsonObj);

	int i;
	for ( i = 0; i < ARRAY_COUNT; i++ ) {
		free(map[i].key);
		free(map[i].value);
	}
	free(map);

	return 0;
}



