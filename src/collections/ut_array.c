#include "ut_array.h"
#include "ut_errno.h"
#include <stdlib.h>
#include <string.h>

struct __ut_array {
	uint8_t *ptr;
	size_t cap;
	size_t len;
	const struct ut_type *element;
};

struct __ut_array_iter {
	struct ut_iter base;
	ut_array_t *array;
	size_t curr;
};

static inline void *ut_array_index(ut_array_t *self, size_t index)
{
	return self->ptr + index * self->element->size;
}

static inline void ut_array_move(ut_array_t *self, size_t n, size_t from,
				 size_t to)
{
	memcpy(ut_array_index(self, to), ut_array_index(self, from),
	       n * self->element->size);
}

ut_array_t *ut_array_new(const struct ut_type *element)
{
	ut_array_t *self;

	if (!element || !element->size)
		return NULL;

	self = malloc(sizeof(ut_array_t));
	if (!self)
		return NULL;

	self->ptr = NULL;
	self->cap = 0;
	self->len = 0;
	self->element = element;
	return self;
}

void ut_array_delete(ut_array_t *self)
{
	ut_array_clear(self);
	free(self->ptr);
	free(self);
}

void ut_array_clear(ut_array_t *self)
{
	if (!self->len)
		return;

	if (self->element->drop) {
		while (self->len) {
			self->element->drop(ut_array_last(self));
			self->len--;
		}
	} else {
		self->len -= self->len;
	}
}

int ut_array_reserve(ut_array_t *self, size_t additional)
{
	size_t min_cap, new_cap;
	uint8_t *new_ptr;

	min_cap = self->len + additional;
	if (self->cap >= min_cap)
		return UT_OK;

	new_cap = !self->cap ? 8 : self->cap << 1;
	while (new_cap < min_cap)
		new_cap <<= 1;

	new_ptr = realloc(self->ptr, new_cap * self->element->size);
	if (!new_ptr)
		return UT_ENOMEM;

	self->ptr = new_ptr;
	self->cap = new_cap;
	return UT_OK;
}

int ut_array_push(ut_array_t *self, const void *data)
{
	if (!data)
		return UT_EINVAL;

	if (self->len >= self->cap && ut_array_reserve(self, 1))
		return UT_ENOMEM;

	memcpy(ut_array_index(self, self->len), data, self->element->size);
	self->len++;
	return UT_OK;
}

void ut_array_pop(ut_array_t *self)
{
	if (!self->len)
		return;

	if (self->element->drop)
		self->element->drop(ut_array_last(self));
	self->len--;
}

int ut_array_insert(ut_array_t *self, size_t index, const void *data)
{
	if (!data)
		return UT_EINVAL;

	if (index > self->len)
		return UT_ERANGE;

	if (index == self->len)
		return ut_array_push(self, data);

	if (self->len >= self->cap && ut_array_reserve(self, 1))
		return UT_ENOMEM;

	ut_array_move(self, self->len - index, index, index + 1);
	memcpy(ut_array_index(self, index), data, self->element->size);
	self->len++;
	return UT_OK;
}

void ut_array_remove(ut_array_t *self, size_t index)
{
	if (index >= self->len)
		return;

	if (index == self->len - 1) {
		ut_array_pop(self);
		return;
	}

	if (self->element->drop)
		self->element->drop(ut_array_index(self, index));
	ut_array_move(self, self->len - index - 1, index + 1, index);
	self->len--;
}

void *ut_array_get(ut_array_t *self, size_t index)
{
	if (index >= self->len)
		return NULL;

	return ut_array_index(self, index);
}

void *ut_array_first(ut_array_t *self)
{
	return ut_array_get(self, 0);
}

void *ut_array_last(ut_array_t *self)
{
	return ut_array_get(self, self->len - 1);
}

size_t ut_array_capacity(const ut_array_t *self)
{
	return self->cap;
}

size_t ut_array_length(const ut_array_t *self)
{
	return self->len;
}

bool ut_array_is_empty(const ut_array_t *self)
{
	return self->len == 0;
}

static void *ut_array_iter_next(struct __ut_array_iter *self)
{
	if (self->curr >= self->array->len)
		return NULL;

	return ut_array_index(self->array, self->curr++);
}

struct ut_iter *ut_array_iter_new(ut_array_t *array)
{
	struct __ut_array_iter *self;

	if (!array)
		return NULL;

	self = malloc(sizeof(struct __ut_array_iter));
	if (!self)
		return NULL;

	self->base.next = (void *)&ut_array_iter_next;
	self->array = array;
	self->curr = 0;
	return &self->base;
}

void ut_array_iter_delete(struct ut_iter *self)
{
	free(self);
}
