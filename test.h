#ifdef DEBUG
#ifndef __TEST_H__
#define __TEST_H__

#include <stdio.h>
#include "map2json.h"

int assertValue(char *expected, char *actual);
void testValues(map2json_keyvalue_t *map, char *jsonString);

#endif
#endif