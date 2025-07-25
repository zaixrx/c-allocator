#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TODO(msg) \
	do { \
		fprintf(stderr, "TODO (%s, %d) at %s: %s\n", \
				__FILE__, __LINE__, __func__, msg); \
		abort(); \
 	} while (0)

#define PAGE_SIZE 1 << 12 // Allocate 1 mega-byte page
#define WORD_SIZE sizeof(void*)
#define WORD void*

typedef struct chunk {
	struct chunk *next;
	size_t size;
} ChunkHeader;

#define HEADER_SIZE (sizeof(ChunkHeader))

static ChunkHeader *free_chunks = NULL;

// `size` is assumed to be a multiple of WORD_SIZE
// upon success a pointer to a chunk struct followed
// by used memory of size `size` is returned otherwise NULL
static ChunkHeader *dchunk(size_t words_count) {
	size_t size = words_count * WORD_SIZE;

	ChunkHeader *chunk = sbrk(size + HEADER_SIZE);

	if (chunk == (void*)-1) return NULL;

	chunk->size = size,
	chunk->next = NULL;

	return chunk;
}

size_t round_up_to(size_t size, size_t to) {
	return size + ((to) - (size % to));
}

// a call with size of 0 sets up an empty page!
void *halloc(size_t size) {
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
			printf("used %zu out of %zu\n", size, curr->size);

			if (curr->size == size) {
				// there is only one chunk
				if (prev == NULL && curr->next == NULL)
					free_chunks = NULL;
				return curr + HEADER_SIZE;
			}

			ChunkHeader *new_chunk = (ChunkHeader*)((char*)curr + (curr->size - size));
			new_chunk->size = size;
			new_chunk->next = NULL;

			curr->size -= size + HEADER_SIZE;

			return new_chunk + HEADER_SIZE;
		}
	}

	if ((curr = prev->next = dchunk(round_up_to(size, PAGE_SIZE) / WORD_SIZE)) == NULL) return NULL;

	goto back;
}

void hfree(void *ptr) {
	// TODO: handle merging memory chunks
	ChunkHeader *chunk = (ChunkHeader*)((char*)ptr - HEADER_SIZE);
	chunk->next = free_chunks;
	free_chunks = chunk;

	TODO("Implement Merging!");
}

static char stdout_buf[1024];

int main(void) {
	setvbuf(stdout, stdout_buf, _IOLBF, sizeof stdout_buf);
	
#define ARR_SIZE 10000
	char *arr[ARR_SIZE] = {0};

	for (int i = 0; i < ARR_SIZE; ++i) {
		arr[i] = halloc(i);
	}

	for (int i = 0; i < ARR_SIZE; ++i ) {
		hfree(arr[i]);
	}

	return 0;
}
