#ifndef __JSON2MAP_JSON2MAP_H__
#define __JSON2MAP_JSON2MAP_H__

typedef struct {
	void (*dataMapHook)(void *data, char *key, char *value);
	void *dataMapData;
	void (*objectMapHook)(void *data, char *key, char *value);
	void *objectMapData;
} json2map_t;

json2map_t *json2map_init();
void json2map_destroy(json2map_t *obj);
int json2map_parse(json2map_t *obj, char *prefix, char *jsonString);
void json2map_registerDataHook(json2map_t *obj, void *data, void *method);
void json2map_registerObjectHook(json2map_t *obj, void *data, void *method);

#endif