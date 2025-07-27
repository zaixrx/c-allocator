#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "halloc.h"

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
		if (curr->size >= size + HEADER_SIZE) {
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
#ifdef DO_LOG
			printf("wrote to %p size %zu\n", new_chunk, size);
#endif
			new_chunk->size = size;
			new_chunk->next = NULL;

			curr->size -= size + HEADER_SIZE;

			return new_chunk + 1;
		}
	}

#ifndef USE_ONE_PAGE
	if ((curr = prev->next = dchunk(round_up_to(size, PAGE_SIZE) / WORD_SIZE)) == NULL) return NULL;

	goto back;
#endif

	return NULL;
}

extern void hfree(void *ptr) {
	ChunkHeader *header = (ChunkHeader*)ptr - 1;

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
#ifdef DO_LOG
		printf("merged RS, new size: %zu\n", header->size);
#endif
		done = true;
	}
	if (prev && (char*)prev + HEADER_SIZE + prev->size == (char*)header) {
		prev->size += HEADER_SIZE + header->size;
		header = prev;
#ifdef DO_LOG
		printf("merged LS, new size: %zu\n", prev->size);
#endif
		done = true;
	}

	if (done) return;

#ifdef DO_LOG
	printf("merely added chunk %p\n", header);
#endif

	if (prev) {
		header->next = prev->next;
		prev->next = header;
		return;
	}

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

// used to work nice with other kinds of know size memory
extern void hbfree(void *ptr, size_t size) {
	ChunkHeader *header = (ChunkHeader*)ptr;
	header->size = size - HEADER_SIZE;
	hfree(header + 1);
}
