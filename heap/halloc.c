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
// #define DO_LOG

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

void hfree(void *ptr) {
	ChunkHeader *chunk = (ChunkHeader*)ptr - 1;

	ChunkHeader *curr, *prev = NULL;
	for (curr = free_chunks; curr; curr = (prev = curr)->next) {
		// TODO: when merging left and right side you should stop!
		if ((char*)curr + HEADER_SIZE + curr->size == (char*)chunk) {
			curr->size += HEADER_SIZE + chunk->size;
			chunk = curr;
			printf("merged LS, new size: %zu\n", curr->size);
		}

		if ((char*)chunk + HEADER_SIZE + chunk->size == (char*)curr) {
			chunk->next  = curr->next;
			chunk->size += HEADER_SIZE + curr->size;
			// first element
			if (prev == NULL) {
				free_chunks = chunk;
			} else {
				prev->next = chunk;
			}
			printf("marged RS, new size: %zu\n", chunk->size);
		}
	}

	chunk->next = free_chunks;
	free_chunks = chunk;
}

int main(void) {
	char stdout_buf[1024];
	setvbuf(stdout, stdout_buf, _IOLBF, sizeof stdout_buf);
	
	// TODO: fix 88 bug
	void *object_a = halloc(WORD_SIZE * 10);
	void *object_b = halloc(WORD_SIZE * 10);

	hfree(object_a);
	hfree(object_b);

	return 0;
}
