#include "jsontable.h"
#include "jsonobject.h"
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

	jsontable_registerHook(NULL, &hookMethod);
	jsontable_parse(JSON_EXAMPLE);
    
    printf("\n\n");

	jsonobject_t *obj = jsonobject_init(12);
	jsonobject_push(obj, "test.name", "picture");
	jsonobject_push(obj, "test.file", "/var/www/html/pictureIn.png");
    jsonobject_push(obj, "test._id.$oid", "566950d1afc4a3c1d86fcdfb");
	jsonobject_push(obj, "test.array[0]", "1");
	jsonobject_push(obj, "test.array[1]", "b");
	jsonobject_push(obj, "test.array[2]", "3");
	jsonobject_push(obj, "test.array[3]", "d");
	jsonobject_push(obj, "test.array[4].object", "test");
	jsonobject_push(obj, "test.nullpointer", "null");
	jsonobject_push(obj, "test.number", "1234");
	jsonobject_push(obj, "test.true", "true");
	jsonobject_push(obj, "test.false", "false");

	printf("%s\n", jsonobject_create(obj));
	jsonobject_destroy(obj);

	return 0;
}



