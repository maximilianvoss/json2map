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


#define LIST_SIZE 255
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
	map2json_t *map2jsonObj = map2json_init(NULL);
	ASSERTSTR("{}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_null() {
	map2json_t *map2jsonObj = map2json_init(NULL);
	map2json_push(map2jsonObj, "nullpointer", "null");
	ASSERTSTR("{\"nullpointer\":null}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_true() {
	map2json_t *map2jsonObj = map2json_init(NULL);
	map2json_push(map2jsonObj, "booleanValue", "true");
	ASSERTSTR("{\"booleanValue\":true}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_false() {
	map2json_t *map2jsonObj = map2json_init(NULL);
	map2json_push(map2jsonObj, "booleanValue", "false");
	ASSERTSTR("{\"booleanValue\":false}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_numberInt() {
	map2json_t *map2jsonObj = map2json_init(NULL);
	map2json_push(map2jsonObj, "number", "123456");
	ASSERTSTR("{\"number\":123456}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_numberFloat() {
	map2json_t *map2jsonObj = map2json_init(NULL);
	map2json_push(map2jsonObj, "number", "123456.789");
	ASSERTSTR("{\"number\":123456.789}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_array() {
	map2json_t *map2jsonObj = map2json_init(NULL);
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
	map2json_t *map2jsonObj = map2json_init(NULL);

	char primitiveMethod[] = " function()";
	*primitiveMethod = JSON2MAP_PRIMITIVE_PREFIXER;
	map2json_push(map2jsonObj, "fakePrimitive", primitiveMethod);

	ASSERTSTR("{\"fakePrimitive\":function()}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_deepNesting() {
	map2json_t *map2jsonObj = map2json_init(NULL);
	map2json_push(map2jsonObj, "lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[0].lvl9", "123456789");

	ASSERTSTR("{\"lvl1\":{\"lvl2\":{\"lvl3\":{\"lvl4\":{\"lvl5\":[{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}]}}}]}}}}}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_map2json_prefix() {
	map2json_t *map2jsonObj = map2json_init("prefix");
	map2json_push(map2jsonObj, "prefix.lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[0].lvl9", "123456789");

	ASSERTSTR("{\"lvl1\":{\"lvl2\":{\"lvl3\":{\"lvl4\":{\"lvl5\":[{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}]}}}]}}}}}", map2json_create(map2jsonObj));
	map2json_destroy(map2jsonObj);
	return 0;
}

int test_json2map_emptyJson() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init(0);
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, NULL, "{}");

	ASSERTSTR("", map[0].key);
	ASSERTSTR("", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_null() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init(0);
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, NULL, "{\"nullpointer\":null}");

	ASSERTSTR("nullpointer", map[0].key);
	ASSERTSTR("null", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_true() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init(0);
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, NULL, "{\"booleanValue\":true}");

	ASSERTSTR("booleanValue", map[0].key);
	ASSERTSTR("true", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_false() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init(0);
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, NULL, "{\"booleanValue\":false}");

	ASSERTSTR("booleanValue", map[0].key);
	ASSERTSTR("false", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_numberInt() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init(0);
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, NULL, "{\"number\":123456}");

	ASSERTSTR("number", map[0].key);
	ASSERTSTR("123456", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_numberFloat() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init(0);
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, NULL, "{\"number\":123456.789}");

	ASSERTSTR("number", map[0].key);
	ASSERTSTR("123456.789", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_array() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init(0);
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, NULL, "{\"array\":[{\"object1\":{\"key1\":\"value1\",\"key2\":2}},{\"array2\":[\"test\",\"test2\"]},\"b\",1]}");

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

	json2map_t *json2mapObj = json2map_init(0);
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, NULL, "{\"fakePrimitive\":function()}");

	ASSERTSTR("fakePrimitive", map[0].key);
	ASSERTSTR("function()", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_deepNesting() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init(0);
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, NULL, "{\"lvl1\":{\"lvl2\":{\"lvl3\":{\"lvl4\":{\"lvl5\":[{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}]}}}]}}}}}");

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[0].lvl9", map[0].key);
	ASSERTSTR("123456789", map[0].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[x]", map[1].key);
	ASSERTSTR("1", map[1].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[x]", map[2].key);
	ASSERTSTR("1", map[2].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_jsonNull() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init(0);
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, NULL, NULL);

	ASSERTSTR("", map[0].key);
	ASSERTSTR("", map[0].value);

	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_noDataHook() {
	json2map_t *json2mapObj = json2map_init(0);
	json2map_parse(json2mapObj, NULL, NULL);
	json2map_destroy(json2mapObj);
	return 0;
}

int test_json2map_objectHook() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);
	int i;

	json2map_t *json2mapObj = json2map_init(1);
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, NULL, "{\"lvl1\":{\"lvl2\":{\"lvl3\":{\"lvl4\":{\"lvl5\":[{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}, {\"key\":\"value\"}]}}}, \"abc\"]}}}}}");

	ASSERTSTR("lvl1[o]", map[0].key);
	ASSERTSTR("{\"lvl2\":{\"lvl3\":{\"lvl4\":{\"lvl5\":[{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}, {\"key\":\"value\"}]}}}, \"abc\"]}}}}", map[0].value);

	ASSERTSTR("lvl1.lvl2[o]", map[1].key);
	ASSERTSTR("{\"lvl3\":{\"lvl4\":{\"lvl5\":[{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}, {\"key\":\"value\"}]}}}, \"abc\"]}}}", map[1].value);

	ASSERTSTR("lvl1.lvl2.lvl3[o]", map[2].key);
	ASSERTSTR("{\"lvl4\":{\"lvl5\":[{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}, {\"key\":\"value\"}]}}}, \"abc\"]}}", map[2].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4[o]", map[3].key);
	ASSERTSTR("{\"lvl5\":[{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}, {\"key\":\"value\"}]}}}, \"abc\"]}", map[3].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[0][o]", map[4].key);
	ASSERTSTR("{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}, {\"key\":\"value\"}]}}}", map[4].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6[o]", map[5].key);
	ASSERTSTR("{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}, {\"key\":\"value\"}]}}", map[5].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7[o]", map[6].key);
	ASSERTSTR("{\"lvl8\":[{\"lvl9\":123456789}, {\"key\":\"value\"}]}", map[6].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[0][o]", map[7].key);
	ASSERTSTR("{\"lvl9\":123456789}", map[7].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[0].lvl9", map[8].key);
	ASSERTSTR("123456789", map[8].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[1][o]", map[9].key);
	ASSERTSTR("{\"key\":\"value\"}", map[9].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[1].key", map[10].key);
	ASSERTSTR("value", map[10].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[x]", map[11].key);
	ASSERTSTR("2", map[11].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[1]", map[12].key);
	ASSERTSTR("abc", map[12].value);

	ASSERTSTR("lvl1.lvl2.lvl3.lvl4.lvl5[x]", map[13].key);
	ASSERTSTR("2", map[13].value);

	map2json_t *tree = map2json_init(NULL);
	for ( i = 0; i < 14; i++ ) {
		map2json_push(tree, map[i].key, map[i].value);
	}
	ASSERTSTR("{\"lvl1\":{\"lvl2\":{\"lvl3\":{\"lvl4\":{\"lvl5\":[\"{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}, {\"key\":\"value\"}]}}}\",\"abc\"]}}}}}", map2json_create(tree));
	json2map_destroy(json2mapObj);
	free(map);
	return 0;
}

int test_json2map_prefix() {
	keyvalue_t *map = (keyvalue_t *) calloc(sizeof(keyvalue_t), LIST_SIZE);

	json2map_t *json2mapObj = json2map_init(0);
	json2map_registerDataHook(json2mapObj, map, &json2map_hookMethod);
	json2map_parse(json2mapObj, "prefix", "{\"lvl1\":{\"lvl2\":{\"lvl3\":{\"lvl4\":{\"lvl5\":[{\"lvl6\":{\"lvl7\":{\"lvl8\":[{\"lvl9\":123456789}]}}}]}}}}}");

	ASSERTSTR("prefix.lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[0].lvl9", map[0].key);
	ASSERTSTR("123456789", map[0].value);

	ASSERTSTR("prefix.lvl1.lvl2.lvl3.lvl4.lvl5[0].lvl6.lvl7.lvl8[x]", map[1].key);
	ASSERTSTR("1", map[1].value);

	ASSERTSTR("prefix.lvl1.lvl2.lvl3.lvl4.lvl5[x]", map[2].key);
	ASSERTSTR("1", map[2].value);

	json2map_destroy(json2mapObj);
	free(map);
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
	TESTCALL("test_map2json_prefix", test_map2json_prefix);

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
	TESTCALL("test_json2map_objectHook", test_json2map_objectHook);
	TESTCALL("test_json2map_prefix", test_json2map_prefix);

	return -1;
}