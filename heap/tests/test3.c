#include <stdio.h>
#include <stdlib.h>

#include "../halloc.h"

int main(void) {
	int *array = halloc(sizeof (int) * 5);
	if (array == NULL) {
		perror("halloc");
		return EXIT_FAILURE;
	}

	array[0] = 1;
	array[1] = 5;
	array[2] = 3;
	array[3] = -1;
	array[4] = 0;

	size_t i = 0;
	for (ChunkHeader *curr = get_used_chunks(); curr; curr = curr->next, ++i);

	printf("there are %zu used heap chunks!\n", i);

	hfree(array);

	return EXIT_SUCCESS;
}
