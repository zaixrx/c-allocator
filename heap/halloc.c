#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#define TODO(msg) \
	do { \
		fprintf(stderr, "TODO (%s, %d) at %s: %s\n", \
				__FILE__, __LINE__, __func__, msg); \
		exit(EXIT_FAILURE); \
 	} while (0)

#define PAGE_SIZE 8 << 9 // I only ever allocate with pages multiples
#define WORD_SIZE sizeof(uintptr_t)
#define WORD uintptr_t

typedef struct chunk {
	struct chunk *next;
	size_t size;
} Chunk;

static Chunk *free_chunks = NULL;

// `size` is assumed to be a multiple of WORD_SIZE
// upon success a pointer to a chunk struct followed
// by used memory of size `size` is returned otherwise NULL
Chunk *dchunk(size_t size) {
	Chunk *chunk = NULL;
	if ((chunk = sbrk(sizeof *chunk + size)) == (void*)-1) return NULL;
	chunk->size = size;
	chunk->next = NULL;
	return chunk;
}

size_t getceil(size_t size, size_t to) {
	if (size % to == 0) return size;
	return size + ((to) - (size % to));
}

// a call with size of 0 sets up an empty page!
void *halloc(size_t size) {
	size = getceil(size, WORD_SIZE);

	if (free_chunks == NULL) {
		// Out Of Memory
		if ((free_chunks = dchunk(getceil(size, PAGE_SIZE))) == NULL) {
			return NULL;
		}
	}

	Chunk *curr = free_chunks;
	for (Chunk *prev = NULL; curr; prev = curr, curr = curr->next) {
		if (curr->size >= size) {
			void *basedata = (void*)(curr + (sizeof *curr));
			if (curr->size == size) {
				// TODO: don't forget to free header part!
				if (prev == NULL && curr->next == NULL) {
					free_chunks = NULL;
				}
				printf("used exactly %zu\n", size);
				return basedata;
			}
			// use bitwise ops
			printf("used %zu out of %zu\n", size, curr->size);
			basedata   += curr->size - size % curr->size;
			curr->size -= size;
			return basedata;
		}
	}

	TODO("Allocate New Page(s) To Fit Object!");
}

int main(void) {
	char *arr[10] = {0};

	for (int i = 0; i < 10; ++i) {
		arr[i] = halloc(i);
	}

	return EXIT_SUCCESS;
}
