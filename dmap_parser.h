#ifndef dmap_parser_h
#define dmap_parser_h
#ifdef __cplusplus
extern "C" {
#endif

const char* dmap_name_from_code(const char* code);
int dmap_parse(const char* buf, int len);

#ifdef __cplusplus
}
#endif
#endif
