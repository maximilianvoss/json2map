#include "json2map.h"
#include "map2json.h"
#include "stdio.h"


#define JSON_EXAMPLE "\
{\
	\"test\":{\
		\"_id\":{\
			\"$oid\":\"566950d1afc4a3c1d86fcdfb\"\
		},\
		\"name\":\"picture\",\
		\"file\":\"/var/www/html/pictureIn.png\",\
		\"array\":[\
			\"1\",\
			\"b\",\
			\"3\",\
			\"d\",\
			{\
				\"object\":\"test\"\
			}\
		],\
		\"nullpointer\":null,\
		\"number\":1234,\
		\"true\":true,\
		\"false\":false\
	}\
}"


void hookMethod(void *data, char *key, char *value) {
	printf("%s = %s\n", key, value);
}

int main(int argc, char **argv) {

	json2map_registerHook(NULL, &hookMethod);
	json2map_parse(JSON_EXAMPLE);
    
    printf("\n\n");

	map2json_t *obj = map2json_init(12);
	map2json_push(obj, "test.name", "picture");
	map2json_push(obj, "test.file", "/var/www/html/pictureIn.png");
    map2json_push(obj, "test._id.$oid", "566950d1afc4a3c1d86fcdfb");
	map2json_push(obj, "test.array[0]", "1");
	map2json_push(obj, "test.array[1]", "b");
	map2json_push(obj, "test.array[2]", "3");
	map2json_push(obj, "test.array[3]", "d");
	map2json_push(obj, "test.array[4].object", "test");
	map2json_push(obj, "test.nullpointer", "null");
	map2json_push(obj, "test.number", "1234");
	map2json_push(obj, "test.true", "true");
	map2json_push(obj, "test.false", "false");

	printf("%s\n", map2json_create(obj));
	map2json_destroy(obj);

	return 0;
}



