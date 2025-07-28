#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../halloc.h"

void *my_malloc(size_t size) {
	printf("used %zu\n", size);
	return malloc(size);
}

void my_free(void *ptr) {
	printf("free\n");
	free(ptr);
}

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
	if (argc >= 3 && strcmp(argv[2], "malloc") == 0) {
		alloc = my_malloc;
	}

	// TEST::START
	void *ptr[noa];
	for (size_t i = 0; i < noa; ++i) {
		if ((ptr[i] = alloc(i)) == NULL) {
			noa = i;
			printf("out of memory at %zu'th iteration!\n", i);
			break;
		}
	}
	printf("number of deallocations is now %zu\n", noa);
	for (size_t i = 0; i < noa; ++i) {
		hfree(ptr[i]);
	}
	// TEST::END

	return EXIT_SUCCESS;
}
