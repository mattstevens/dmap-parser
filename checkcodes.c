#include "dmap_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

/* Parses a dmap.contentcodesresponse checking for new content codes */

static int in_response = 0;
static int in_type = 0;

struct contentcode {
	char code[5];
	char name[256];
	uint32_t type;
};

static struct contentcode current_type = {};

enum CODE_TYPE {
	UINT8 = 1,
	INT8 = 2,
	UINT16 = 3,
	INT16 = 4,
	UINT32 = 5,
	INT32 = 6,
	UINT64 = 7,
	INT64 = 8,
	STRING = 9,
	DATE = 10,
	VERSION = 11,
	LIST = 12
};

static const char *dmap_type_for_type(uint32_t type) {
	switch ((enum CODE_TYPE)type) {
		case UINT8:
		case UINT16:
		case UINT32:
		case UINT64:
			return "DMAP_UINT,";

		case INT8:
		case INT16:
		case INT32:
		case INT64:
			return "DMAP_INT, ";

		case STRING:
			return "DMAP_STR, ";

		case DATE:
			return "DMAP_DATE,";

		case VERSION:
			return "DMAP_VERS,";

		case LIST:
			return "DMAP_DICT,";
	}

	return "UNKNOWN,";
}

static void on_dict_start(void *ctx, const char *code, const char *name) {
	if (in_response) {
		in_type = 1;
	} else {
		if (strcmp(code, "mccr") == 0) {
			in_response = 1;
		}
	}
}

static void on_dict_end(void *ctx, const char *code, const char *name) {
	if (in_type) {
		in_type = 0;
		if (current_type.code[0] && current_type.name[0]) {
			const char *state = NULL;
			const char *current_name = dmap_name_from_code(current_type.code);
			if (!current_name) {
				state = "NEW    ";
			} else if (strcmp(current_type.name, current_name) != 0) {
				state = "UPDATED";
			}

			if (state) {
				printf("%s { \"%s\", %s \"%s\" },\n", state, current_type.code, dmap_type_for_type(current_type.type), current_type.name);
			}
		}
		memset(&current_type, 0, sizeof(current_type));
	} else if (in_response) {
		in_response = 0;
	}
}

static void on_uint32(void *ctx, const char *code, const char *name, uint32_t value) {
	if (strcmp(code, "mcnm") == 0) {
		current_type.code[0] = (value >> 24) & 0xff;
		current_type.code[1] = (value >> 16) & 0xff;
		current_type.code[2] = (value >>  8) & 0xff;
		current_type.code[3] = value & 0xff;
		return;
	} else if (strcmp(code, "mcty") == 0) {
		current_type.type = value;
	}
}

static void on_string(void *ctx, const char *code, const char *name, const char *buf, size_t len) {
	if (strcmp(code, "mcna") == 0) {
		strncpy(current_type.name, buf, len);
	}
}

int main(int argc, char* argv[]) {
	dmap_settings settings = {
		.on_dict_start = on_dict_start,
		.on_dict_end   = on_dict_end,
		.on_uint32     = on_uint32,
		.on_string     = on_string,
		.ctx           = 0
	};

	char *buf = NULL;
	size_t size = 0;
	ssize_t result = 0;

	size_t bufIncrement = 60 * 1024;

	do {
		size += result;
		buf = realloc(buf, size + bufIncrement);
		result = read(fileno(stdin), &buf[size], bufIncrement);
	} while (result > 0);

	dmap_parse(&settings, buf, size);

	free(buf);

	return 0;
}
