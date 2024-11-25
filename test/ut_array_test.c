#include "ut_array.h"
#include <stdio.h>
#include <stdlib.h>

static void print_int_array(ut_array_t *a)
{
	size_t i = 0;
	size_t len = ut_array_length(a);
	printf("[ ");
	for (; i < len; i++) {
		int *p = ut_array_get(a, i);
		printf("%d ", *p);
	}
	printf("]\n");
}

static void abort_if_not_equal1(ut_array_t *a, int *data)
{
	size_t i = 0;
	size_t len = ut_array_length(a);

	for (; i < len; i++) {
		int *p = ut_array_get(a, i);
		if (*p != data[i]) {
			puts("Error!");
			print_int_array(a);
			puts("and");
			printf("[ ");
			for (i = 0; i < len; i++)
				printf("%d ", data[i]);
			puts("]\nare not equal!\n");
			abort();
		}
	}
}

static void test1()
{
	ut_array_t *a = ut_array_new(ut_type_int());

	for (int i = 0; i < 4; i++)
		ut_array_push(a, &i);
	abort_if_not_equal1(a, (int[]){ 0, 1, 2, 3 });

	/* Insert at the front. */
	ut_array_insert(a, 0, &(int){ 100 });
	abort_if_not_equal1(a, (int[]){ 100, 0, 1, 2, 3 });

	/* Insert at the back. */
	ut_array_insert(a, ut_array_length(a), &(int){ 100 });
	abort_if_not_equal1(a, (int[]){ 100, 0, 1, 2, 3, 100 });

	/* Insert in the middle. */
	ut_array_insert(a, 3, &(int){ 100 });
	abort_if_not_equal1(a, (int[]){ 100, 0, 1, 100, 2, 3, 100 });

	ut_array_push(a, &(int){ 0 });

	/* Insert at the back when the array is full. */
	ut_array_insert(a, ut_array_length(a), &(int){ 100 });
	abort_if_not_equal1(a, (int[]){ 100, 0, 1, 100, 2, 3, 100, 0, 100 });

	ut_array_delete(a);
}

static void test2()
{
	ut_array_t *a = ut_array_new(ut_type_int());

	for (int i = 0; i < 10; i++)
		ut_array_push(a, &i);

	struct ut_iter *iter = ut_array_iter_new(a);
	int *p = iter->next(iter);
	while (p) {
		printf("%d ", *p);
		p = iter->next(iter);
	}
	putchar('\n');

	ut_array_iter_delete(iter);

	ut_array_delete(a);
}

int main()
{
	test1();
	test2();
	return 0;
}
