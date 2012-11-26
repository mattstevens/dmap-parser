CC?=clang
AR?=ar
CFLAGS += -Wall -Werror

testapp: dmap_parser.o test.c
	$(CC) $(CFLAGS) -o testapp dmap_parser.o test.c

dmap_parser.o: dmap_parser.c dmap_parser.h Makefile
	$(CC) $(CFLAGS) -c dmap_parser.c -o $@

test: testapp
	@ ./testapp

clean:
	rm *.o testapp

.PHONY: clean test
