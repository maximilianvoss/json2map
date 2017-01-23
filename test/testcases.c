#include <string.h>
#include <stdio.h>
#include "../map2json.h"
#include "../config.h"
#include "../json2map.h"

#define TESTCALL(TESTNAME, TESTMETHOD)\
    printf("\nTest case:\t%s\n", TESTNAME);\
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


#define LIST_SIZE 10
#define STRING_LENGTH 255

typedef struct {
	char key[STRING_LENGTH];
	char value[STRING_LENGTH];
} keyvalue_t;


void json2map_hookMethod(void *data, char *key, char *value) {
	int i;
	keyvalue_t *list = (keyvalue_t *) data;

	i = 0;
	while ( strcmp(list->key, "") ) {
		i++;
		list++;
	}
	strcpy(list->key, key);
	strcpy(list->value, value);
}

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
	map2json_push(map2jsonObj, "array[1].array2[x]", "2");
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

int test_json2map_emptyJson() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init();
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, "{}");

	ASSERTSTR("", map[0].key);
	ASSERTSTR("", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_null() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init();
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, "{\"nullpointer\":null}");

	ASSERTSTR("nullpointer", map[0].key);
	ASSERTSTR("null", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_true() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init();
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, "{\"booleanValue\":true}");

	ASSERTSTR("booleanValue", map[0].key);
	ASSERTSTR("true", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_false() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init();
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, "{\"booleanValue\":false}");

	ASSERTSTR("booleanValue", map[0].key);
	ASSERTSTR("false", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_numberInt() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init();
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, "{\"number\":123456}");

	ASSERTSTR("number", map[0].key);
	ASSERTSTR("123456", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_numberFloat() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init();
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, "{\"number\":123456.789}");

	ASSERTSTR("number", map[0].key);
	ASSERTSTR("123456.789", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_array() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init();
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, "{\"array\":[{\"object1\":{\"key1\":\"value1\",\"key2\":2}},{\"array2\":[\"test\",\"test2\"]},\"b\",1]}");

	ASSERTSTR("array[0].object1.key1", map[0].key);
	ASSERTSTR("value1", map[0].value);

	ASSERTSTR("array[0].object1.key2", map[1].key);
	ASSERTSTR("2", map[1].value);

	ASSERTSTR("array[1].array2[0]", map[2].key);
	ASSERTSTR("test", map[2].value);

	ASSERTSTR("array[1].array2[1]", map[3].key);
	ASSERTSTR("test2", map[3].value);

	ASSERTSTR("array[1].array2[x]", map[4].key);
	ASSERTSTR("2", map[4].value);

	ASSERTSTR("array[2]", map[5].key);
	ASSERTSTR("b", map[5].value);

	ASSERTSTR("array[3]", map[6].key);
	ASSERTSTR("1", map[6].value);

	ASSERTSTR("array[x]", map[7].key);
	ASSERTSTR("4", map[7].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_primitive() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init();
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, "{\"fakePrimitive\":function()}");

	ASSERTSTR("fakePrimitive", map[0].key);
	ASSERTSTR("function()", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_deepNesting() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init();
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, "{\"lvl1\":{\"lvl2\":{\"lvl3\":{\"lvl4\":{\"lvl5\":[{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}]}}}]}}}}}");

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[0].lvl9", map[0].key);
	ASSERTSTR("123456789", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_jsonNull() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init();
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, NULL);

	ASSERTSTR("", map[0].key);
	ASSERTSTR("", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_noDataHook() {
	json2map_t *json2mapObj = json2map_init();
	json2map_parse(json2mapObj, NULL);
	json2map_destroy(json2mapObj);
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

	TESTCALL("test_json2map_emptyJson", test_json2map_emptyJson);
	TESTCALL("test_json2map_null", test_json2map_null);
	TESTCALL("test_json2map_true", test_json2map_true);
	TESTCALL("test_json2map_false", test_json2map_false);
	TESTCALL("test_json2map_numberInt", test_json2map_numberInt);
	TESTCALL("test_json2map_numberFloat", test_json2map_numberFloat);
	TESTCALL("test_json2map_array", test_json2map_array);
	TESTCALL("test_json2map_primitive", test_json2map_primitive);
	TESTCALL("test_json2map_deepNesting", test_json2map_deepNesting);
	TESTCALL("test_json2map_jsonNull", test_json2map_jsonNull);
	TESTCALL("test_json2map_noDataHook", test_json2map_noDataHook);

	return -1;
}