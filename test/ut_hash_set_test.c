#include "ut_hash_set.h"
#include <stdio.h>
#include <stdlib.h>

static void print_int_hash_set(ut_hash_set_t *set)
{
	int *element;
	struct ut_iter *iter;

	iter = ut_hash_set_iter_new(set);
	element = iter->next(iter);
	printf("[ ");
	while (element) {
		printf("%d ", *element);
		element = iter->next(iter);
	}
	printf("]\n");
	ut_hash_set_iter_delete(iter);
}

static void test1()
{
	int *p;
	size_t i;
	ut_hash_set_t *set;
	int raw_data[] = { 874, 638, 5321, 96, 705, 1423, 3689 };

	set = ut_hash_set_new(ut_type_int());

	for (i = 0; i < sizeof(raw_data) / sizeof(raw_data[0]); i++)
		ut_hash_set_insert(set, &raw_data[i]);

	for (i = 0; i < sizeof(raw_data) / sizeof(raw_data[0]); i++) {
		p = ut_hash_set_get(set, &raw_data[i]);
		if (*p != raw_data[i]) {
			printf("Error: %d != %d\n", *p, raw_data[i]);
			abort();
		}
	}

	print_int_hash_set(set);

	ut_hash_set_delete(set);
}

int main()
{
	test1();
	return 0;
}
