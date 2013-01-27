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

char prefix[64] = {};
static const char hexchars[] = "0123456789abcdef";

void indent() {
	strcat(prefix, "  ");
}

void outdent() {
	size_t len = strlen(prefix);
	if (len >= 2) {
		prefix[len - 2] = '\0';
	}
}

void append(const char* line, ...) {
	va_list args;
	va_start(args, line);

	printf("%s", prefix);
	vprintf(line, args);
	printf("\n");

	va_end(args);
}

void on_dict_start(void *ctx, const char *code, const char *name) {
	append("%s:", name);
	indent();
}

void on_dict_end(void *ctx, const char *code, const char *name) {
	outdent();
}

void on_int32(void *ctx, const char *code, const char *name, int32_t value) {
	append("%s: %d", name, value);
}

void on_int64(void *ctx, const char *code, const char *name, int64_t value) {
	append("%s: %lld", name, value);
}

void on_uint32(void *ctx, const char *code, const char *name, uint32_t value) {
	if (strcmp(code, "mcnm") == 0) {
		char buf[5] = {
			(value >> 24) & 0xff,
			(value >> 16) & 0xff,
			(value >>  8) & 0xff,
			value & 0xff,
			0
		};
		append("%s: %s", name, buf);
		return;
	}

	append("%s: %u", name, value);
}

void on_uint64(void *ctx, const char *code, const char *name, uint64_t value) {
	append("%s: %llu", name, value);
}

void on_date(void *ctx, const char *code, const char *name, uint32_t value) {
	char buf[32];
	time_t timeval = value;
	struct tm* timestruct = gmtime(&timeval);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S +0000", timestruct);
	append("%s: %s", name, buf);
}

void on_string(void *ctx, const char *code, const char *name, const char *buf, size_t len) {
	char *str = (char *)malloc(len + 1);
	strncpy(str, buf, len);
	str[len] = '\0';
	append("%s: %s", name, str);
	free(str);
}

void on_data(void *ctx, const char *code, const char *name, const char *buf, size_t len) {
	size_t i;
	printf("%s%s: ", prefix, name);
	for (i = 0; i < len; i++) {
		putc(hexchars[buf[i] >> 4], stdout);
		putc(hexchars[buf[i] & 0x0f], stdout);
		putc(' ', stdout);
	}
	printf("\n");
}

int main(int argc, char* argv[]) {
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

	if (argc >= 2) {
		if (strcmp(argv[1], "--help") == 0) {
			printf("Usage: dmapprint [file]\n\n"
			"Accepts Digital Media Access Protocol input from a file or "
			"standard input and\nprints it in a human-readable format.\n");
			return 0;
		}

		const char *path = argv[1];

		struct stat s;
		if (stat(path, &s) != 0) {
			return 1;
		}

		size = s.st_size;
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
			size += result;
			buf = realloc(buf, size + bufIncrement);
			result = read(fileno(stdin), &buf[size], bufIncrement);
		} while (result > 0);
	}

	dmap_parse(&settings, buf, size);

	free(buf);

	return 0;
}
