#ifndef __TEST_H__
#define __TEST_H__

#ifdef DEBUG

#include <stdio.h>
#include "map2json.h"

int assertValue(char *expected, char *actual);
void testValues(map2json_keyvalue_t *map, char *jsonString);

#define DEBUG_TEXT(fmt, ...) printf(fmt, __VA_ARGS__)

#else

#define DEBUG_TEXT(fmt, ...)

#endif
#endif