CC?=clang
AR?=ar
CFLAGS += -Wall -Wextra -Werror -Os
APPCFLAGS = $(CFLAGS) -Wno-unused-parameter

test: testapp
	@ ./testapp

dmap_parser.o: dmap_parser.c dmap_parser.h Makefile
	$(CC) $(CFLAGS) -c dmap_parser.c -o $@

testapp: dmap_parser.o test.c
	$(CC) $(APPCFLAGS) -o testapp dmap_parser.o test.c

dmapprint: dmap_parser.o dmapprint.c
	$(CC) $(APPCFLAGS) -o dmapprint dmap_parser.o dmapprint.c

clean:
	rm *.o testapp dmapprint

.PHONY: clean test
