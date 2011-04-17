#ifndef dmap_parser_h
#define dmap_parser_h
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void (*dmap_int32_cb) (void *ctx, const char* code, const char* name, int32_t value);

typedef struct {
	dmap_int32_cb on_int32;
	void *ctx;
} dmap_settings;

const char* dmap_name_from_code(const char* code);
	int dmap_parse(const dmap_settings* settings, const char* buf, int len);

#ifdef __cplusplus
}
#endif
#endif
