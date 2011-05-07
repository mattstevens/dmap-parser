#ifndef dmap_parser_h
#define dmap_parser_h
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void (*dmap_dict_cb) (void *ctx, const char *code, const char *name);
typedef void (*dmap_int32_cb) (void *ctx, const char *code, const char *name, int32_t value);
typedef void (*dmap_int64_cb) (void *ctx, const char *code, const char *name, int64_t value);
typedef void (*dmap_data_cb)  (void *ctx, const char *code, const char *name, const char *buf, int len);

typedef struct {
	dmap_dict_cb  on_dict_start;
	dmap_dict_cb  on_dict_end;
	dmap_int32_cb on_int32;
	dmap_int64_cb on_int64;
	dmap_data_cb  on_string;
	dmap_data_cb  on_data;
	void *ctx;
} dmap_settings;

const char *dmap_name_from_code(const char *code);
int dmap_parse(const dmap_settings *settings, const char *buf, int len);

#ifdef __cplusplus
}
#endif
#endif
