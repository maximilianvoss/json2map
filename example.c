#include "jsontable.h"
#include "stdio.h"

void hookMethod(void *data, char *key, char *value) {
	printf("%s = %s\n", key, value);
}

int main(int argc, char **argv) {
	jsontable_registerHook(NULL, &hookMethod);
	jsontable_parse("{ \"test\": { \"_id\" : { \"$oid\" : \"566950d1afc4a3c1d86fcdfb\" } }, \"name\" : \"picture\", \"file\" : \"/var/www/html/pictureIn.png\", \"moep\": [\"a\", \"b\", \"c\", \"d\", { \"test\": \"blubb\"}], \"bla\": null, \"number\": 1234, \"true\": true, \"false\": false }");
	return 0;
}