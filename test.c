#include "test.h"

#ifdef DEBUG

int assertValue(char *expected, char *actual) {
	if ( expected == NULL && actual == NULL ) {
		return 0;
	}
	if ( expected == NULL ) {
		printf("Expected: (null)\t Actual: %s\n", actual);
		return 1;
	}
	if ( actual == NULL ) {
		printf("Expected: %s\t Actual: (null)\n", expected);
		return 1;
	}
	if ( !strcmp (expected, actual )) {
		return 0;
	}
	printf("Expected: %s\t Actual: %s\n", expected, actual);
	return 1;
}


void testValues(map2json_keyvalue_t *map, char *jsonString) {
	int errorCount = 0;
	errorCount += assertValue("test._id.$oid", map[0].key);
	errorCount += assertValue("566950d1afc4a3c1d86fcdfb", map[0].value);

	errorCount += assertValue("test.name", map[1].key);
	errorCount += assertValue("picture", map[1].value);

	errorCount += assertValue("test.file", map[2].key);
	errorCount += assertValue("/var/www/html/pictureIn.png", map[2].value);

	errorCount += assertValue("test.array[0]", map[3].key);
	errorCount += assertValue("1", map[3].value);

	errorCount += assertValue("test.array[1]", map[4].key);
	errorCount += assertValue("b", map[4].value);

	errorCount += assertValue("test.array[2]", map[5].key);
	errorCount += assertValue("3", map[5].value);

	errorCount += assertValue("test.array[3]", map[6].key);
	errorCount += assertValue("d", map[6].value);

	errorCount += assertValue("test.array[4].object", map[7].key);
	errorCount += assertValue("test", map[7].value);

	errorCount += assertValue("test.nullpointer", map[8].key);
	errorCount += assertValue("null", map[8].value);

	errorCount += assertValue("test.number", map[9].key);
	errorCount += assertValue("1234", map[9].value);

	errorCount += assertValue("test.true", map[10].key);
	errorCount += assertValue("true", map[10].value);

	errorCount += assertValue("test.false", map[11].key);
	errorCount += assertValue("false", map[11].value);
	//assertValue()

	errorCount += assertValue("{ \"test\" : { \"false\" : false, \"true\" : true, \"number\" : 1234, \"nullpointer\" : null, \"array\" : [ 1, \"b\", 3, \"d\", { \"object\" : \"test\" } ], \"_id\" : { \"$oid\" : \"566950d1afc4a3c1d86fcdfb\" }, \"file\" : \"/var/www/html/pictureIn.png\", \"name\" : \"picture\" } }", jsonString);
}

#endif