#define STACK_SIZE 1024
#define NULL (void*)0

static char allocbuf[STACK_SIZE];
static char *allocp = allocbuf;

extern inline void *alloc(unsigned int n) {
	return (allocp - allocbuf + n >= sizeof allocbuf) ? NULL : (void*)((allocp += n) - n);
}

extern inline void afree(void *p) {
	if ((void*)allocbuf <= p && p < (void*)allocbuf + sizeof allocbuf) allocp = p;
}
