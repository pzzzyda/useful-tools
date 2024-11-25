#ifndef _UT_MEMSWAP_H
#define _UT_MEMSWAP_H

#include <stddef.h>
#include <stdint.h>

static inline void ut_memswap(void *a, void *b, size_t size)
{
	uint8_t tmp;
	size_t i = 0;
	for (; i < size; i++) {
		tmp = ((uint8_t *)a)[i];
		((uint8_t *)a)[i] = ((uint8_t *)b)[i];
		((uint8_t *)b)[i] = tmp;
	}
}

#endif /* ut_memswap.h */
