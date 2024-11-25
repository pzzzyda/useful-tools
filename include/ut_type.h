#ifndef _UT_TYPE_H
#define _UT_TYPE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct ut_type {
	size_t size;
	void (*drop)(void *);
	int (*compare)(const void *, const void *);
	size_t (*hash)(const void *);
};

const struct ut_type *ut_type_char(void);
const struct ut_type *ut_type_short(void);
const struct ut_type *ut_type_int(void);
const struct ut_type *ut_type_long(void);

const struct ut_type *ut_type_uchar(void);
const struct ut_type *ut_type_ushort(void);
const struct ut_type *ut_type_uint(void);
const struct ut_type *ut_type_ulong(void);

const struct ut_type *ut_type_float(void);
const struct ut_type *ut_type_double(void);

#endif /* ut_type.h */
