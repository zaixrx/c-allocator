#ifndef _HALLOC_H
#define _HALLOC_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define TODO(msg) \
	do { \
		fprintf(stderr, "TODO (%s, %d) at %s: %s\n", \
				__FILE__, __LINE__, __func__, msg); \
		abort(); \
 	} while (0)

#define PAGE_SIZE 1 << 12
#define WORD_SIZE sizeof(uintptr_t)
#define WORD uintptr_t

// TODO: get rid of this after testing!
// #define USE_ONE_PAGE
// #define DO_LOG

typedef struct chunk {
	struct chunk *next;
	size_t size;
} ChunkHeader;

#define HEADER_SIZE (sizeof(ChunkHeader))

extern void *halloc(size_t size);
extern void hfree(void *ptr);
extern void *hcalloc(size_t size);
extern void hbfree(void *ptr, size_t size);
extern ChunkHeader *get_used_chunks();
#endif
