#ifndef _UT_STRING_H
#define _UT_STRING_H

#include "ut_type.h"

struct ut_string {
	char *ptr;
	size_t cap;
	size_t len;
};

const struct ut_type *ut_type_string(void);

struct ut_string *ut_string_new(const char *s);

void ut_string_delete(struct ut_string *self);

struct ut_string *ut_string_init(struct ut_string *self, const char *s);

void ut_string_drop(struct ut_string *self);

void ut_string_clear(struct ut_string *self);

int ut_string_reserve(struct ut_string *self, size_t additional);

int ut_string_append(struct ut_string *self, const char *s);

int ut_string_compare(const struct ut_string *self,
		      const struct ut_string *other);

size_t ut_string_hash(const struct ut_string *self);

#endif /* ut_string.h */
