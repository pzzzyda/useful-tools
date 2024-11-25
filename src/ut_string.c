#include "ut_string.h"
#include "ut_errno.h"
#include "ut_hash.h"
#include <stdlib.h>
#include <string.h>

static const struct ut_type __ut_type_string = {
	.size = sizeof(struct ut_string),
	.drop = (void *)&ut_string_drop,
	.compare = (void *)&ut_string_compare,
	.hash = (void *)&ut_string_hash,
};

const struct ut_type *ut_type_string()
{
	return &__ut_type_string;
}

struct ut_string *ut_string_new(const char *s)
{
	struct ut_string *self = self = malloc(sizeof(struct ut_string));
	if (!self)
		return NULL;

	ut_string_init(self, s);
	return self;
}

void ut_string_delete(struct ut_string *self)
{
	ut_string_drop(self);
	free(self);
}

struct ut_string *ut_string_init(struct ut_string *self, const char *s)
{
	size_t len;

	self->ptr = NULL;
	self->cap = 0;
	self->len = 0;

	if (!s)
		return self;

	len = strlen(s);
	self->ptr = malloc(len + 1);
	if (!self->ptr)
		return NULL;

	memcpy(self->ptr, s, len + 1);
	self->cap = len;
	self->len = len;
	return self;
}

void ut_string_drop(struct ut_string *self)
{
	free(self->ptr);
	self->ptr = NULL;
	self->cap = 0;
	self->len = 0;
}

void ut_string_clear(struct ut_string *self)
{
	self->len = 0;
}

int ut_string_reserve(struct ut_string *self, size_t additional)
{
	char *new_ptr;
	size_t new_cap, min_cap;

	min_cap = self->len + additional;
	if (self->cap >= min_cap)
		return UT_OK;

	new_cap = !self->cap ? 8 : self->cap << 1;
	while (new_cap < min_cap)
		new_cap <<= 1;

	new_ptr = realloc(self->ptr, new_cap + 1);
	if (!new_ptr)
		return UT_ENOMEM;

	self->ptr = new_ptr;
	self->cap = new_cap;
	return UT_OK;
}

int ut_string_append(struct ut_string *self, const char *string)
{
	size_t len;

	if (!string)
		return UT_OK;

	len = strlen(string);
	if (self->len + len > self->cap && ut_string_reserve(self, len))
		return UT_ENOMEM;

	memcpy(&self->ptr[self->len], string, len + 1);
	self->len += len;
	return UT_OK;
}

int ut_string_compare(const struct ut_string *self,
		      const struct ut_string *other)
{
	return strcmp(self->ptr, other->ptr);
}

size_t ut_string_hash(const struct ut_string *self)
{
	return ut_bkdr(self->ptr);
}
