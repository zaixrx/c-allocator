#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../halloc.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s <number_of_allocations> <halloc | malloc>?(halloc)\n", argv[0]);
		return 1;
	}

	size_t noa = atoi(argv[1]);
	if (noa <= 0) {
		fprintf(stderr, "number_of_allocations must be greater than zero mine frienda!\n");
		return 1;
	}

	void *(*alloc)(size_t) = halloc;
	void  (*freep)(void*)  = hfree;
	if (argc >= 3 && strcmp(argv[2], "malloc") == 0) {
		alloc = malloc;
		freep = free;
	}

	// TEST::START
	void *ptr[noa];
	for (size_t i = 0; i < noa; ++i) {
		if ((ptr[i] = alloc(i+1)) == NULL) {
			noa = i;
			perror("alloc");
			break;
		}
		// printf("INFO: allocted %zu bytes\n", i+1);
	}
	printf("\n----------\n");
	printf("\nnumber of deallocations is now %zu\n", noa);
	printf("\n----------\n");
	for (size_t i = 1; i < noa; ++i) {
		freep(ptr[i]);
	}
	// TEST::END

	return EXIT_SUCCESS;
}
