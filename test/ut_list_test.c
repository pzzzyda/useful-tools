#include "ut_list.h"
#include <stdio.h>

static void print_int_list(ut_list_t *l)
{
	struct ut_iter *iter = ut_list_iter_new(l);
	int *p = iter->next(iter);
	printf("[ ");
	while (p) {
		printf("%d ", *p);
		p = iter->next(iter);
	}
	printf("]\n");
	ut_list_iter_delete(iter);
}

static void test1()
{
	ut_list_t *l = ut_list_new(ut_type_int());

	for (int i = 0; i < 5; i++)
		ut_list_push_back(l, &i);
	for (int i = 0; i < 5; i++)
		ut_list_push_front(l, &i);

	print_int_list(l);

	ut_list_delete(l);
}

int main()
{
	test1();
	return 0;
}
