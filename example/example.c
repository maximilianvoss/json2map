#include <string.h>
#include <stdio.h>
#include "../json2map.h"
#include "../map2json.h"
#include "../config.h"

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
		\"false\":false,\
		\"test.fakePrimitive\": function()\
	}\
}"


void hookMethod(void *data, char *key, char *value) {
	printf("Map entry: %s = %s\n", key, value);
}

void json2map() {

	json2map_t *json2mapObj = json2map_init();
	json2map_registerDataHook(json2mapObj, NULL, &hookMethod);
	json2map_parse(json2mapObj, NULL, JSON_EXAMPLE);
	json2map_destroy(json2mapObj);
}

void map2json() {
	map2json_t *map2jsonObj = map2json_init(NULL);
	map2json_push(map2jsonObj, "test._id.$oid", "566950d1afc4a3c1d86fcdfb");
	map2json_push(map2jsonObj, "test.name", "picture");
	map2json_push(map2jsonObj, "test.file", "/var/www/html/pictureIn.png");
	map2json_push(map2jsonObj, "test.array[0].mysubobject", "value");
	map2json_push(map2jsonObj, "test.array[0].secondobject", "0");
	map2json_push(map2jsonObj, "test.array[1]", "1");
	map2json_push(map2jsonObj, "test.array[2]", "b");
	map2json_push(map2jsonObj, "test.array[3]", "3");
	map2json_push(map2jsonObj, "test.array[4]", "d");
	map2json_push(map2jsonObj, "test.array[5].object", "test");
	map2json_push(map2jsonObj, "test.array[5].object2", "test2");
	map2json_push(map2jsonObj, "test.array[x]", "6");
	map2json_push(map2jsonObj, "test.nullpointer", "null");
	map2json_push(map2jsonObj, "test.number", "1234");
	map2json_push(map2jsonObj, "test.true", "true");
	map2json_push(map2jsonObj, "test.false", "false");

	char primitiveMethod[] = " function()";
	*primitiveMethod = JSON2MAP_PRIMITIVE_PREFIXER;
	map2json_push(map2jsonObj, "test.fakePrimitive", primitiveMethod);

	printf("%s\n", map2json_create(map2jsonObj));

	map2json_destroy(map2jsonObj);
}

int main(int argc, char **argv) {
	json2map();
	map2json();

	return 0;
}



