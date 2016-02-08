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

	json2map_t* json2mapObj = json2map_init();
	json2map_registerHook(json2mapObj, NULL, &hookMethod);
	json2map_parse(json2mapObj, JSON_EXAMPLE);
    
    printf("\n\n");

	map2json_t *map2jsonObj = map2json_init(12);
	map2json_push(map2jsonObj, "test.name", "picture");
	map2json_push(map2jsonObj, "test.file", "/var/www/html/pictureIn.png");
    map2json_push(map2jsonObj, "test._id.$oid", "566950d1afc4a3c1d86fcdfb");
	map2json_push(map2jsonObj, "test.array[0]", "1");
	map2json_push(map2jsonObj, "test.array[1]", "b");
	map2json_push(map2jsonObj, "test.array[2]", "3");
	map2json_push(map2jsonObj, "test.array[3]", "d");
	map2json_push(map2jsonObj, "test.array[4].map2jsonObject", "test");
	map2json_push(map2jsonObj, "test.nullpointer", "null");
	map2json_push(map2jsonObj, "test.number", "1234");
	map2json_push(map2jsonObj, "test.true", "true");
	map2json_push(map2jsonObj, "test.false", "false");

	printf("%s\n", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);

	return 0;
}



