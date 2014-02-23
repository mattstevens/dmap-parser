#include "dmap_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>

typedef enum {
	UINT8   = 1,
	INT8    = 2,
	UINT16  = 3,
	INT16   = 4,
	UINT32  = 5,
	INT32   = 6,
	UINT64  = 7,
	INT64   = 8,
	STRING  = 9,
	DATE    = 10,
	VERSION = 11,
	LIST    = 12
} CONTENT_CODE_TYPE;

static char prefix[64] = {0};
static const char hexchars[] = "0123456789abcdef";
static int rawcodes = 0;

static void indent() {
	strcat(prefix, "  ");
}

static void outdent() {
	size_t len = strlen(prefix);
	if (len >= 2) {
		prefix[len - 2] = '\0';
	}
}

#if defined(__GNUC__)
__attribute__((format(printf, 1, 2)))
#endif
static void append(const char *line, ...) {
	va_list args;
	va_start(args, line);

	printf("%s", prefix);
	vprintf(line, args);
	printf("\n");

	va_end(args);
}

static inline const char *display_name(const char *code, const char *name) {
	return rawcodes ? code : name;
}

static void on_dict_start(void *ctx, const char *code, const char *name) {
	append("%s:", display_name(code, name));
	indent();
}

static void on_dict_end(void *ctx, const char *code, const char *name) {
	outdent();
}

static void on_int32(void *ctx, const char *code, const char *name, int32_t value) {
	append("%s: %d", display_name(code, name), value);
}

static void on_int64(void *ctx, const char *code, const char *name, int64_t value) {
	append("%s: %lld", display_name(code, name), value);
}

static void on_uint32(void *ctx, const char *code, const char *name, uint32_t value) {
	if (strcmp(code, "mcnm") == 0) {
		char buf[5] = {
			(char)((value >> 24) & 0xff),
			(char)((value >> 16) & 0xff),
			(char)((value >>  8) & 0xff),
			(char)(value & 0xff),
			0
		};
		append("%s: %s", display_name(code, name), buf);
		return;
	} else if (strcmp(code, "mcty") == 0) {
		const char *description = NULL;
		switch ((CONTENT_CODE_TYPE)value) {
			case UINT8:
				description = "unsigned 8-bit integer";
				break;
			case INT8:
				description = "8-bit integer";
				break;
			case UINT16:
				description = "unsigned 16-bit integer";
				break;
			case INT16:
				description = "16-bit integer";
				break;
			case UINT32:
				description = "unsigned 32-bit integer";
				break;
			case INT32:
				description = "32-bit integer";
				break;
			case UINT64:
				description = "unsigned 64-bit integer";
				break;
			case INT64:
				description = "64-bit integer";
				break;
			case STRING:
				description = "string";
				break;
			case DATE:
				description = "date";
				break;
			case VERSION:
				description = "version";
				break;
			case LIST:
				description = "list";
				break;
		}

		if (description != NULL) {
			append("%s: %s (%u)", display_name(code, name), description, value);
			return;
		}
	}

	append("%s: %u", display_name(code, name), value);
}

static void on_uint64(void *ctx, const char *code, const char *name, uint64_t value) {
	append("%s: %llu", display_name(code, name), value);
}

static void on_date(void *ctx, const char *code, const char *name, uint32_t value) {
	char buf[32];
	time_t timeval = value;
	struct tm *timestruct = gmtime(&timeval);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S +0000", timestruct);
	append("%s: %s", display_name(code, name), buf);
}

static void on_string(void *ctx, const char *code, const char *name, const char *buf, size_t len) {
	char *str = (char *)malloc(len + 1);
	strncpy(str, buf, len);
	str[len] = '\0';
	append("%s: %s", display_name(code, name), str);
	free(str);
}

static void on_data(void *ctx, const char *code, const char *name, const char *buf, size_t len) {
	size_t i;
	printf("%s%s: ", prefix, display_name(code, name));
	for (i = 0; i < len; i++) {
		putc(hexchars[(unsigned char)buf[i] >> 4], stdout);
		putc(hexchars[(unsigned char)buf[i] & 0x0f], stdout);
		putc(' ', stdout);
	}
	printf("\n");
}

static void print_usage(void) {
	printf(""
	"Usage: dmapprint [options] [file]\n"
	"\n"
	"Reads Digital Media Access Protocol (DMAP) input from a file or standard input\n"
	"and prints it in a human-readable format.\n"
	"\n"
	"Options:\n"
	"  -h, --help       Show this help message and exit\n"
	"  -r, --raw-codes  Don't map content codes to human-readable names\n"
	"      --version    Show the version and exit \n");
}

int main(int argc, char *argv[]) {
	dmap_settings settings = {
		.on_dict_start = on_dict_start,
		.on_dict_end   = on_dict_end,
		.on_int32      = on_int32,
		.on_int64      = on_int64,
		.on_uint32     = on_uint32,
		.on_uint64     = on_uint64,
		.on_date       = on_date,
		.on_string     = on_string,
		.on_data       = on_data,
		.ctx           = 0
	};

	char *buf = NULL;
	size_t size = 0;
	ssize_t result = 0;
	int optchar;

	static struct option longopts[] = {
		{ "help",         no_argument,        NULL,          'h' },
		{ "raw-codes",    no_argument,        NULL,          'r' },
		{ "version",      no_argument,        NULL,          'v' },
		{ NULL,           0,                  NULL,           0  }
	};

	while ((optchar = getopt_long(argc, argv, "hr", longopts, NULL)) != -1) {
		switch (optchar) {
			case 'h':
				print_usage();
				return 0;
			case 'r':
				rawcodes = 1;
				break;
			case 'v':
				printf("dmapprint %s\n", dmap_version_string());
				return 0;
			case 0:
				break;
			case '?':
				return 1;
			default:
				fprintf(stderr, "unhandled option -%c\n", optchar);
				return 1;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc >= 1) {
		const char *path = argv[0];

		struct stat s;
		if (stat(path, &s) != 0 || s.st_size < 0) {
			return 1;
		}

		size = (size_t)s.st_size;
		buf = malloc(size);

		int fd = open(path, O_RDONLY);
		if (fd == -1)
			return 1;

		do {
			result = read(fd, buf, size);
		} while (result == -1 && errno == EINTR);

		close(fd);

		if (result == -1)
			return 1;
	} else {
		size_t bufIncrement = 60 * 1024;

		do {
			size += (size_t)result;
			buf = realloc(buf, size + bufIncrement);
			result = read(fileno(stdin), &buf[size], bufIncrement);
		} while (result > 0);
	}

	dmap_parse(&settings, buf, size);

	free(buf);

	return 0;
}
