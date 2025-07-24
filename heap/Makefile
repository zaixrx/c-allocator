all: halloc
	./halloc

debug: halloc
	gdb ./halloc

halloc: halloc.c
	cc -o halloc halloc.c -g
