CFLAGS += -Wall -Wextra -Werror -Os
APPCFLAGS = $(CFLAGS) -Wno-unused-parameter

test: testapp
	@ ./testapp

checkcodes: checkapp
	curl --silent --header "Viewer-Only-Client: 1" http://127.0.0.1:3689/content-codes | ./checkapp

dmap_parser.o: dmap_parser.c dmap_parser.h Makefile
	$(CC) $(CFLAGS) -c dmap_parser.c -o $@

testapp: dmap_parser.o test.c
	$(CC) $(APPCFLAGS) -o testapp dmap_parser.o test.c

checkapp: dmap_parser.o checkcodes.c
	$(CC) $(APPCFLAGS) -o checkapp dmap_parser.o checkcodes.c

dmapprint: dmap_parser.o dmapprint.c
	$(CC) $(APPCFLAGS) -o dmapprint dmap_parser.o dmapprint.c

clean:
	rm -f *.o checkapp testapp dmapprint

.PHONY: checkcodes clean test
