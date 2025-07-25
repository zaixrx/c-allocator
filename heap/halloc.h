#ifndef _HALLOC_H
#define _HALLOC_H
#include <stddef.h>
extern void *halloc(size_t size);
extern void hfree(void *ptr);
extern void *hcalloc(size_t size);
extern void hbfree(void *ptr, size_t size);
#endif
