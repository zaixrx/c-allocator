#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "halloc.h"

#define TODO(msg) \
	do { \
		fprintf(stderr, "TODO (%s, %d) at %s: %s\n", \
				__FILE__, __LINE__, __func__, msg); \
		abort(); \
 	} while (0)

#define PAGE_SIZE 1 << 20 // Allocate 1 mega-byte page
#define WORD_SIZE sizeof(uintptr_t)
#define WORD uintptr_t
// #define DO_LOG

typedef struct chunk {
	struct chunk *next;
	size_t size;
} ChunkHeader;

#define HEADER_SIZE (sizeof(ChunkHeader))

static ChunkHeader *free_chunks = NULL;

static ChunkHeader *dchunk(size_t words) {
	size_t size = words * WORD_SIZE;

	ChunkHeader *chunk = sbrk(size + HEADER_SIZE);

	if (chunk == (void*)-1) return NULL;

	chunk->size = size,
	chunk->next = NULL;

	return chunk;
}

size_t round_up_to(size_t size, size_t to) {
	if (size > 0 && size % to == 0) return size;
	return size + ((to) - (size % to));
}

// a call with size of 0 sets up an empty page!
extern void *halloc(size_t size) {
	size = round_up_to(size, WORD_SIZE);

	if (free_chunks == NULL) {
		// Out Of Memory errno for more details. <:
		if ((free_chunks = dchunk(round_up_to(size, PAGE_SIZE) / WORD_SIZE)) == NULL) {
			return NULL;
		}
	}

	ChunkHeader *curr = free_chunks, *prev = NULL;
	for (; curr; prev = curr, curr = curr->next) {
back:
		if (curr->size >= size) {
#ifdef DO_LOG
			printf("used %zu out of %zu\n", size, curr->size);
#endif
			if (curr->size == size) {
				// there is only one chunk
				if (prev == NULL && curr->next == NULL)
					free_chunks = NULL;
				return curr + 1;
			}

			ChunkHeader *new_chunk = (ChunkHeader*)((char*)curr + (curr->size - size));
			printf("wrote to %p size %zu\n", new_chunk, size);
			new_chunk->size = size;
			new_chunk->next = NULL;

			curr->size -= size + HEADER_SIZE;

			return new_chunk + 1;
		}
	}

	if ((curr = prev->next = dchunk(round_up_to(size, PAGE_SIZE) / WORD_SIZE)) == NULL) return NULL;

	goto back;
}

extern void hfree(void *ptr) {
	ChunkHeader *header = (ChunkHeader*)ptr - 1;
	header->next = NULL;

	if (free_chunks == NULL) {
		free_chunks = header; return;
	}

	ChunkHeader *prev, *curr;
	for (prev = NULL, curr = free_chunks; curr && (char*)curr <= (char*)ptr; curr = (prev = curr)->next);

	bool done = false;
	if (curr && (char*)header + HEADER_SIZE + header->size == (char*)curr) {
		header->next  = curr->next;
		header->size += HEADER_SIZE + curr->size;
		// first element
		if (prev == NULL) {
			free_chunks = header;
		} else {
			prev->next  = header;
		}
		printf("merged RS, new size: %zu\n", header->size);
		done = true;
	}
	if (prev && (char*)prev + HEADER_SIZE + prev->size == (char*)header) {
		prev->size += HEADER_SIZE + header->size;
		header = prev;
		printf("merged LS, new size: %zu\n", prev->size);
		done = true;
	}

	if (done) return;

	if (prev) {
		header->next = prev;
		prev->next = header;

		printf("added in the middle?\n");
		return;
	}

	printf("added at first!\n");
	header->next = free_chunks;
	free_chunks = header;
}

extern void *hcalloc(size_t size) {
	uintptr_t *data = halloc(size);
	if (data == NULL) return NULL;
	size = round_up_to(size, WORD_SIZE);
	for (WORD i = 0; i < (size / WORD_SIZE); ++i) {
		*data = 0x0; // meow hehe
	}
	return data;
}

// used to work nice with other kinds of memory
// one usage would be:
// void *foreign_memory = malloc(69);
// hbfree(foreign_memory); // now this memory can be reused!
extern void hbfree(void *ptr, size_t size) {
	ChunkHeader *header = (ChunkHeader*)((char*)ptr - HEADER_SIZE);
	header->size = size;
	hfree(ptr);
}

// int main(void) {
// 	char stdout_buf[1024];
// 	setvbuf(stdout, stdout_buf, _IOLBF, sizeof stdout_buf);
// 	
// 	void *object_a = hcalloc(WORD_SIZE * 10);
// 	void *object_b = hcalloc(WORD_SIZE * 10);
// 
// 	hfree(object_a);
// 	hfree(object_b);
// 
// 	TODO("Exercise 8-7. malloc accepts a size request without checking its plausibility; free believes"
// 	     "that the block it is asked to free contains a valid size field. Improve these routines so they make"
// 	     "more pains with error checking.");
// 
// 	return 0;
// }
