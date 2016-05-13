#ifndef __JSON2MAP_JSON2MAP_H__
#define __JSON2MAP_JSON2MAP_H__

typedef struct {
	void (*hookMethod)(void *data, char *key, char *value);

	void *hookMethodData;
} json2map_t;


json2map_t *json2map_init();

void json2map_destroy(json2map_t *obj);

int json2map_parse(json2map_t *obj, char *jsonString);

void json2map_registerHook(json2map_t *obj, void *data, void *method);

#endif