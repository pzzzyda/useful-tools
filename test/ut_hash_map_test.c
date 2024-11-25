#include "ut_hash_map.h"
#include "ut_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_hash_map1(ut_hash_map_t *map)
{
	struct ut_iter *iter;
	struct ut_pair *pair;
	struct ut_string *pkey;
	int *pvalue;

	iter = ut_hash_map_iter_new(map);
	pair = iter->next(iter);
	while (pair) {
		pkey = pair->key;
		pvalue = pair->value;
		printf("(%s, %d)\n", pkey->ptr, *pvalue);
		pair = iter->next(iter);
	}

	ut_hash_map_iter_delete(iter);
}

static void abort_if_not_equal1(ut_hash_map_t *map, char *key, int value)
{
	size_t len = strlen(key);
	struct ut_string s = {
		.ptr = key,
		.cap = len,
		.len = len,
	};
	int *pvalue = ut_hash_map_get(map, &s);
	if (!pvalue || *pvalue != value) {
		printf("Error! No %s or the value of %s is not %d!\n", s.ptr,
		       s.ptr, value);
		abort();
	}
}

static void test1()
{
	ut_hash_map_t *map;

	struct ut_string tmp;

	map = ut_hash_map_new(ut_type_string(), ut_type_int());

	ut_hash_map_insert(map, ut_string_init(&tmp, "Apple"), &(int){ 10 });
	ut_hash_map_insert(map, ut_string_init(&tmp, "Grape"), &(int){ 20 });
	ut_hash_map_insert(map, ut_string_init(&tmp, "Orange"), &(int){ 30 });
	ut_hash_map_insert(map, ut_string_init(&tmp, "Pear"), &(int){ 40 });
	ut_hash_map_insert(map, ut_string_init(&tmp, "Banana"), &(int){ 50 });
	ut_hash_map_insert(map, ut_string_init(&tmp, "Cherry"), &(int){ 60 });
	ut_hash_map_insert(map, ut_string_init(&tmp, "Lemon"), &(int){ 70 });
	ut_hash_map_insert(map, ut_string_init(&tmp, "Pineapple"),
			   &(int){ 80 });
	ut_hash_map_insert(map, ut_string_init(&tmp, "Watermelon"),
			   &(int){ 90 });

	abort_if_not_equal1(map, "Apple", 10);
	abort_if_not_equal1(map, "Grape", 20);
	abort_if_not_equal1(map, "Orange", 30);
	abort_if_not_equal1(map, "Pear", 40);
	abort_if_not_equal1(map, "Banana", 50);
	abort_if_not_equal1(map, "Cherry", 60);
	abort_if_not_equal1(map, "Lemon", 70);
	abort_if_not_equal1(map, "Pineapple", 80);
	abort_if_not_equal1(map, "Watermelon", 90);

	print_hash_map1(map);
	ut_hash_map_delete(map);
}

int main()
{
	test1();
	return 0;
}
