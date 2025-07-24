#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

#define PAGE_SIZE 1 << 12

#define print_size(type, type_as_str) \
	printf("size of %s: %lu byte(s)\n", (type_as_str), sizeof (type));

int main(void) {
	// print_size(signed, "signed");
	// print_size(unsigned, "unsigned");

	// print_size(short, "short");
	// print_size(int, "int");
	// print_size(long, "long");
	// print_size(long long, "long long");
	// print_size(long long int, "long long int");

	// print_size(void*, "pointer");

	/*
	struct rlimit rlimits = {0};

	getrlimit(RLIMIT_STACK, &rlimits);
	printf("soft_limit: %lu, hard_limit: %lu\n", rlimits.rlim_cur, rlimits.rlim_max);

	char buf[1000000]; // 1MB

	getrlimit(RLIMIT_STACK, &rlimits);
	printf("soft_limit: %lu, hard_limit: %lu\n", rlimits.rlim_cur, rlimits.rlim_max);
	*/

	// void *fbr = sbrk(0);
	// sbrk(PAGE_SIZE);
	// void *ebr = sbrk(0);
	// printf("start: %p, end: %p\n", fbr, ebr);

#define ARR_SIZE 10000
	char *arr[ARR_SIZE] = {0};

	for (int i = 0; i < ARR_SIZE; ++i) {
		printf("allocated %d bytes rnrn\n", i);
		arr[i] = malloc(i);
	}

	for (int i = 0; i < ARR_SIZE; ++i) {
		printf("freed %d bytes rnrn\n", i);
		free(arr[i]);
	}

	return EXIT_SUCCESS;
}
