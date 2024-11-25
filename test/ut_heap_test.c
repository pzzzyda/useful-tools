#include "ut_heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void test1()
{
	ut_heap_t *heap;
	struct ut_iter *iter;
	int element, *pelement;
	size_t i;

	heap = ut_heap_new(ut_type_int());

	srand(time(NULL));
	for (i = 0; i < 20; i++) {
		element = rand() % 100;
		ut_heap_push(heap, &element);
	}

	iter = ut_heap_iter_new(heap);
	pelement = iter->next(iter);
	printf("[ ");
	while (pelement) {
		printf("%d ", *pelement);
		pelement = iter->next(iter);
	}
	printf("]\n");
	ut_heap_iter_delete(iter);

	printf("[ ");
	while (!ut_heap_is_empty(heap)) {
		pelement = ut_heap_peek(heap);
		printf("%d ", *pelement);
		ut_heap_pop(heap);
	}
	printf("]\n");

	ut_heap_delete(heap);
}

int main()
{
	test1();
	return 0;
}
