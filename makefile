#Just enough makefile so I can get test output in sublime

CC= cc
CFLAGS=  -g

all: test

debug: test_netprot_header

test: test_netprot_header
	./test_netprot_header

test_netprot_header: test_netprot_header.c netprot_header.c seatest.c
	$(CC) $(CFLAGS) test_netprot_header.c netprot_header.c seatest.c -o test_netprot_header

clean:
	rm test_netprot_header