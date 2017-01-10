#include <string.h>
#include <printf.h>
#include "../map2json.h"
#include "../config.h"

#define TESTCALL(TESTNAME, TESTMETHOD)\
    if ( argv[1] == NULL ) \
        TESTMETHOD();\
    else\
        if (!strcmp(argv[1], TESTNAME))\
            return TESTMETHOD();

#define ASSERTSTR(EXPECTED, ACTUAL)\
    printf("Expected: \t%s\nActual: \t%s\n\n", EXPECTED, ACTUAL);\
    if (strcmp(EXPECTED, ACTUAL))\
        return 1;

#define ASSERTINT(EXPECTED, ACTUAL)\
    printf("Expected: \t%d\nActual: \t%d\n\n", EXPECTED, ACTUAL);\
    if (EXPECTED != ACTUAL)\
        return 1;

int test_map2json_emptyMap() {
	map2json_t *map2jsonObj = map2json_init();
	ASSERTSTR("{}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_null() {
	map2json_t *map2jsonObj = map2json_init();
	map2json_push(map2jsonObj, "nullpointer", "null");
	ASSERTSTR("{\"nullpointer\":null}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_true() {
	map2json_t *map2jsonObj = map2json_init();
	map2json_push(map2jsonObj, "booleanValue", "true");
	ASSERTSTR("{\"booleanValue\":true}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_false() {
	map2json_t *map2jsonObj = map2json_init();
	map2json_push(map2jsonObj, "booleanValue", "false");
	ASSERTSTR("{\"booleanValue\":false}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_numberInt() {
	map2json_t *map2jsonObj = map2json_init();
	map2json_push(map2jsonObj, "number", "123456");
	ASSERTSTR("{\"number\":123456}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_numberFloat() {
	map2json_t *map2jsonObj = map2json_init();
	map2json_push(map2jsonObj, "number", "123456.789");
	ASSERTSTR("{\"number\":123456.789}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_array() {
	map2json_t *map2jsonObj = map2json_init();
	map2json_push(map2jsonObj, "array[0].object1.key1", "value1");
	map2json_push(map2jsonObj, "array[0].object1.key2", "2");
	map2json_push(map2jsonObj, "array[3]", "1");
	map2json_push(map2jsonObj, "array[2]", "b");
	map2json_push(map2jsonObj, "array[1].array2[0]", "test");
	map2json_push(map2jsonObj, "array[1].array2[1]", "test2");
	map2json_push(map2jsonObj, "array[x]", "4");

	ASSERTSTR("{\"array\":[{\"object1\":{\"key1\":\"value1\",\"key2\":2}},{\"array2\":[\"test\",\"test2\"]},\"b\",1]}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_primitive() {
	map2json_t *map2jsonObj = map2json_init();

	char primitiveMethod[] = " function()";
	*primitiveMethod = JSON2MAP_PRIMITIVE_PREFIXER;
	map2json_push(map2jsonObj, "fakePrimitive", primitiveMethod);

	ASSERTSTR("{\"fakePrimitive\":function()}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}


int test_map2json_deepNesting() {
	map2json_t *map2jsonObj = map2json_init();
	map2json_push(map2jsonObj, "lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[0].lvl9", "123456789");

	ASSERTSTR("{\"lvl1\":{\"lvl2\":{\"lvl3\":{\"lvl4\":{\"lvl5\":[{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}]}}}]}}}}}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int main(int argc, char **argv) {
	TESTCALL("test_map2json_emptyMap", test_map2json_emptyMap);
	TESTCALL("test_map2json_null", test_map2json_null);
	TESTCALL("test_map2json_true", test_map2json_true);
	TESTCALL("test_map2json_false", test_map2json_false);
	TESTCALL("test_map2json_numberInt", test_map2json_numberInt);
	TESTCALL("test_map2json_numberFloat", test_map2json_numberFloat);
	TESTCALL("test_map2json_array", test_map2json_array);
	TESTCALL("test_map2json_primitive", test_map2json_primitive);
	TESTCALL("test_map2json_deepNesting", test_map2json_deepNesting);

	return -1;
}