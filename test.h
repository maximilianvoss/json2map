#ifndef __JSON2MAP_TEST_H__
#define __JSON2MAP_TEST_H__

#ifdef DEBUG

#include <stdio.h>
#include "map2json.h"

int assertValue(char *expected, char *actual);
void testValues(map2json_keyvalue_t *map, char *jsonString);

#endif
#endif