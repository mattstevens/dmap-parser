#ifndef dmap_parser_h
#define dmap_parser_h
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sys/types.h>

#define DMAP_VERSION_MAJOR 1
#define DMAP_VERSION_MINOR 1
#define DMAP_VERSION_PATCH 0

#define DMAP_VERSION (DMAP_VERSION_MAJOR * 1000000 + \
                      DMAP_VERSION_MINOR * 1000 + \
                      DMAP_VERSION_PATCH)

typedef void (*dmap_dict_cb)   (void *ctx, const char *code, const char *name);
typedef void (*dmap_int32_cb)  (void *ctx, const char *code, const char *name, int32_t value);
typedef void (*dmap_int64_cb)  (void *ctx, const char *code, const char *name, int64_t value);
typedef void (*dmap_uint32_cb) (void *ctx, const char *code, const char *name, uint32_t value);
typedef void (*dmap_uint64_cb) (void *ctx, const char *code, const char *name, uint64_t value);
typedef void (*dmap_data_cb)   (void *ctx, const char *code, const char *name, const char *buf, size_t len);

typedef struct {
	dmap_dict_cb   on_dict_start;
	dmap_dict_cb   on_dict_end;
	dmap_int32_cb  on_int32;
	dmap_int64_cb  on_int64;
	dmap_uint32_cb on_uint32;
	dmap_uint64_cb on_uint64;
	dmap_uint32_cb on_date;
	dmap_data_cb   on_string;
	dmap_data_cb   on_data;
	void *ctx;
} dmap_settings;

/**
 * Returns the library version number.
 *
 * The version number format is (major * 1000000) + (minor * 1000) + patch.
 * For example, the value for version 1.2.3 is 1002003.
 */
int dmap_version();

/**
 * Returns the library version as a string.
 */
const char *dmap_version_string();

const char *dmap_name_from_code(const char *code);
int dmap_parse(const dmap_settings *settings, const char *buf, size_t len);

#ifdef __cplusplus
}
#endif
#endif
