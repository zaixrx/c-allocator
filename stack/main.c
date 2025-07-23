#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *alloc(unsigned int n);
void afree(void *p);

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s <string_to_be_allocated>\n", argv[0]);
		fprintf(stderr, "error: string_to_be_allocated is undefined!\n");
		return EXIT_FAILURE;
	}
	char *str = NULL; size_t size = strlen(argv[1]);
	for (int chunk = 0; (str = (char*)alloc(size)); ++chunk) {
		memcpy(str, "Hello, World!", size);
		printf("[%d]: %s of size %lu\n", chunk, str, size);
		// afree(str); TODO: uncomment this if you want to test freeing
		// too tired to think of a better example!
	}
	fprintf(stderr, "stack overflow!\n");
	return EXIT_SUCCESS;
}
