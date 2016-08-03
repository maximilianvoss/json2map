#include <string.h>
#include "test.h"

#ifdef DEBUG

int assertValue(char *expected, char *actual) {
	if ( expected == NULL && actual == NULL ) {
		return 0;
	}
	if ( expected == NULL ) {
		printf("\nrExpected:\t(null)\nActual:\t\t%s\n\n", actual);
		return 1;
	}
	if ( actual == NULL ) {
		printf("\nExpected:\t%s\nActual:\t\t(null)\n\n", expected);
		return 1;
	}

	if ( !strcmp (expected, actual )) {
		return 0;
	}
	printf("\nExpected:\t%s\nActual:\t\t%s\n\n", expected, actual);
	return 1;
}


void testValues(map2json_keyvalue_t *map, char *jsonString) {
	int i = 0;
	int errorCount = 0;

	errorCount += assertValue("test._id.$oid", map[i].key);
	errorCount += assertValue("566950d1afc4a3c1d86fcdfb", map[i].value);
	i++;

	errorCount += assertValue("test.name", map[i].key);
	errorCount += assertValue("picture", map[i].value);
	i++;

	errorCount += assertValue("test.file", map[i].key);
	errorCount += assertValue("/var/www/html/pictureIn.png", map[i].value);
	i++;

	errorCount += assertValue("test.array[0].mysubobject", map[i].key);
	errorCount += assertValue("value", map[i].value);
	i++;

	errorCount += assertValue("test.array[0].secondobject", map[i].key);
	errorCount += assertValue("0", map[i].value);
	i++;

	errorCount += assertValue("test.array[1]", map[i].key);
	errorCount += assertValue("1", map[i].value);
	i++;

	errorCount += assertValue("test.array[2]", map[i].key);
	errorCount += assertValue("b", map[i].value);
	i++;

	errorCount += assertValue("test.array[3]", map[i].key);
	errorCount += assertValue("3", map[i].value);
	i++;

	errorCount += assertValue("test.array[4]", map[i].key);
	errorCount += assertValue("d", map[i].value);
	i++;

	errorCount += assertValue("test.array[5].object", map[i].key);
	errorCount += assertValue("test", map[i].value);
	i++;

	errorCount += assertValue("test.array[5].object2", map[i].key);
	errorCount += assertValue("test2", map[i].value);
	i++;

	errorCount += assertValue("test.nullpointer", map[i].key);
	errorCount += assertValue("null", map[i].value);
	i++;

	errorCount += assertValue("test.number", map[i].key);
	errorCount += assertValue("1234", map[i].value);
	i++;

	errorCount += assertValue("test.true", map[i].key);
	errorCount += assertValue("true", map[i].value);
	i++;

	errorCount += assertValue("test.false", map[i].key);
	errorCount += assertValue("false", map[i].value);
	i++;

	errorCount += assertValue("{\"test\":{\"name\":\"picture\",\"file\":\"/var/www/html/pictureIn.png\",\"_id\":{\"$oid\":\"566950d1afc4a3c1d86fcdfb\"},\"array\":[{\"mysubobject\":\"value\","
			                          "\"secondobject\":0},1,\"b\",3,\"d\",""{\"object\":\"test\",\"object2\":\"test2\"}],\"nullpointer\":null,\"number\":1234,\"true\":true,\"false\":false,\"fakePrimitive\":new Function()}}",
	                          jsonString);

	printf("Error Count: %d\n", errorCount);
}

#endif