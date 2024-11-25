#include "ut_deque.h"
#include <stdio.h>
#include <stdlib.h>

static void print_int_deque(ut_deque_t *d)
{
	size_t i = 0;
	size_t len = ut_deque_length(d);
	printf("[ ");
	for (; i < len; i++) {
		int *p = ut_deque_get(d, i);
		printf("%d ", *p);
	}
	printf("]\n");
}

static void abort_if_not_equal1(ut_deque_t *d, int *data)
{
	size_t i = 0;
	size_t len = ut_deque_length(d);

	for (; i < len; i++) {
		int *p = ut_deque_get(d, i);
		if (*p != data[i]) {
			puts("Error!");
			print_int_deque(d);
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
	ut_deque_t *d = ut_deque_new(ut_type_int());

	for (int i = 0; i < 4; i++)
		ut_deque_push_back(d, &i);
	abort_if_not_equal1(d, (int[]){ 0, 1, 2, 3 });

	/* Insert at the front. */
	ut_deque_insert(d, 0, &(int){ 100 });
	abort_if_not_equal1(d, (int[]){ 100, 0, 1, 2, 3 });

	/* Insert at the back. */
	ut_deque_insert(d, ut_deque_length(d), &(int){ 100 });
	abort_if_not_equal1(d, (int[]){ 100, 0, 1, 2, 3, 100 });

	/* Insert in the middle. */
	ut_deque_insert(d, 3, &(int){ 100 });
	abort_if_not_equal1(d, (int[]){ 100, 0, 1, 100, 2, 3, 100 });

	ut_deque_push_back(d, &(int){ 0 });

	/* Insert at the back when the deque is full. */
	ut_deque_insert(d, ut_deque_length(d), &(int){ 100 });
	abort_if_not_equal1(d, (int[]){ 100, 0, 1, 100, 2, 3, 100, 0, 100 });

	ut_deque_delete(d);
}

static void test2()
{
	ut_deque_t *d = ut_deque_new(ut_type_int());

	/*
	 *             H
	 * [ 0 . . . . 2 1 0]
	 */
	for (int i = 0; i < 1; i++)
		ut_deque_push_back(d, &i);
	for (int i = 0; i < 3; i++)
		ut_deque_push_front(d, &i);
	abort_if_not_equal1(d, (int[]){ 2, 1, 0, 0 });

	/* Insert at the front. */
	/*
	 *            H
	 * [ 0 . . . 100 2 1 0 ]
	 */
	ut_deque_insert(d, 0, &(int){ 100 });
	abort_if_not_equal1(d, (int[]){ 100, 2, 1, 0, 0 });

	/* Insert at the back. */
	/*
	 *              H
	 * [ 0 100 . . 100 2 1 0 ]
	 */
	ut_deque_insert(d, ut_deque_length(d), &(int){ 100 });
	abort_if_not_equal1(d, (int[]){ 100, 2, 1, 0, 0, 100 });

	/*
	 * Case 1:
	 *            H
	 * [ 0 100 . 100 2 1 100 0 ]
	 */
	ut_deque_insert(d, 3, &(int){ 100 });
	abort_if_not_equal1(d, (int[]){ 100, 2, 1, 100, 0, 0, 100 });

	/*
	 * Case 2:
	 *              H
	 * [ 0 100 100 100 2 1 100 0 ]
	 */
	ut_deque_insert(d, ut_deque_length(d) - 1, &(int){ 100 });
	abort_if_not_equal1(d, (int[]){ 100, 2, 1, 100, 0, 0, 100, 100 });

	/* Insert at the back when the deque is full. */
	/*
	 *                                H
	 * [ 0 100 100 100 . . . . . . . 100 2 1 100 0 ]
	 */
	ut_deque_insert(d, ut_deque_length(d), &(int){ 100 });
	abort_if_not_equal1(d, (int[]){ 100, 2, 1, 100, 0, 0, 100, 100, 100 });

	ut_deque_delete(d);
}

static void test3()
{
	ut_deque_t *d = ut_deque_new(ut_type_int());

	for (int i = 0; i < 5; i++)
		ut_deque_push_back(d, &i);
	for (int i = 0; i < 5; i++)
		ut_deque_push_front(d, &i);

	struct ut_iter *iter = ut_deque_iter_new(d);
	int *p = iter->next(iter);
	while (p) {
		printf("%d ", *p);
		p = iter->next(iter);
	}
	putchar('\n');

	ut_deque_iter_delete(iter);

	ut_deque_delete(d);
}

int main()
{
	test1();
	test2();
	test3();
	return 0;
}
