#include "ut_hash.h"

uint32_t ut_jenkins(const void *key, size_t len)
{
	uint32_t hash = 0;
	const uint8_t *byte = key;
	for (size_t i = 0; i < len; i++) {
		hash += byte[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

uint32_t ut_bkdr(const char *key)
{
	uint32_t hash = 0;
	while (*key) {
		hash = hash * 131 + *key;
		key++;
	}
	return hash;
}
