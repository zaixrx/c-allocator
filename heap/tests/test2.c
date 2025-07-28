// testing bfree

#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "../halloc.h"

#define MAX_ITERATIONS 0x69

static volatile bool running = true;
void sigint_handler(int as_useful_as_you) {
	(void)as_useful_as_you; // to shut the compiler warning
	running = false;
	printf("Closing...\n");
}

#define VLA_SIZE 1
#define VLA_WORDS 10
static WORD data[VLA_SIZE][VLA_WORDS];

unsigned int get_words_count(void) {
	printf("how many words do you want: ");
	static char str[1024];
	size_t i = 0;
	do {
		if ((str[i] = getc(stdin)) == '\n') break;
	} while (++i < sizeof(str)-1);
	str[i] = '\0';
	unsigned int num = atoi(str);
	printf("read number %u\n", num);
	return num;
}

int main(void) {
	signal(SIGINT, sigint_handler);

	halloc(0); // initialize alloctor

	size_t i;
	size_t alloc_size;

	for (i = 0; i < VLA_SIZE; ++i) {
		hbfree(data[i], sizeof data[i]);
	}

	do {
		alloc_size = get_words_count();
	} while (alloc_size == 0);
	alloc_size *= WORD_SIZE;

	for (i = 0; i < MAX_ITERATIONS; ++i) {
		if (halloc(alloc_size) == NULL) break;
	}

	printf("took %zu allocation if %zu bytes to stop!\n", i, alloc_size);

	return EXIT_SUCCESS;
}
