CC=cc
CFLAGS=-Wextra \
       -Wall \
       -g

all: test1.out test2.out test3.out

test1.out: test1.c ../halloc.c
	$(CC) $(CFLAGS) -o $@ $^

test2.out: test2.c ../halloc.c
	$(CC) $(CFLAGS) -o $@ $^

test3.out: test3.c ../halloc.c
	$(CC) $(CFLAGS) -o $@ $^
