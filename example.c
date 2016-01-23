#include "jsontable.h"
#include "stdio.h"


#define JSON_EXAMPLE "{\
   \"test\":{  \
      \"_id\":{ \
         \"$oid\":\"566950d1afc4a3c1d86fcdfb\"\
      },\
   \"name\":\"picture\",\
   \"file\":\"/var/www/html/pictureIn.png\",\
   \"array\":[  \
      \"1\",\
      \"b\",\
      \"3\",\
      \"d\",\
      {  \
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
	printf("%s\n", JSON_EXAMPLE);
	jsontable_registerHook(NULL, &hookMethod);
	jsontable_parse(JSON_EXAMPLE);
	return 0;
}