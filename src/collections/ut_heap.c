#include "ut_heap.h"
#include "ut_array.h"
#include "ut_memswap.h"
#include <stdlib.h>

struct __ut_array {
	uint8_t *ptr;
	size_t cap;
	size_t len;
	const struct ut_type *element;
};

struct __ut_heap {
	ut_array_t buf;
};

static inline void *ut_array_index(ut_array_t *self, size_t index)
{
	return self->ptr + index * self->element->size;
}

static inline int ut_array_do_compare(ut_array_t *self, size_t a, size_t b)
{
	return self->element->compare(ut_array_index(self, a),
				      ut_array_index(self, b));
}

static inline void ut_array_do_swap(ut_array_t *self, size_t a, size_t b)
{
	ut_memswap(ut_array_index(self, a), ut_array_index(self, b),
		   self->element->size);
}

static void ut_array_sift_down(ut_array_t *self, size_t i)
{
	size_t l, r, m;

	while (i * 2 + 1 < self->len) {
		l = i * 2 + 1;
		r = i * 2 + 2;
		m = i;

		if (ut_array_do_compare(self, l, m) > 0)
			m = l;

		if (r < self->len) {
			if (ut_array_do_compare(self, r, m) > 0)
				m = r;
		}

		if (m != i) {
			ut_array_do_swap(self, m, i);
			i = m;
		} else {
			break;
		}
	}
}

ut_heap_t *ut_heap_new(const struct ut_type *element)
{
	ut_heap_t *self;

	if (!element || !element->size)
		return NULL;

	self = malloc(sizeof(ut_heap_t));
	if (!self)
		return NULL;

	self->buf.ptr = NULL;
	self->buf.cap = 0;
	self->buf.len = 0;
	self->buf.element = element;
	return self;
}

void ut_heap_delete(ut_heap_t *self)
{
	ut_array_delete(&self->buf);
}

void ut_heap_clear(ut_heap_t *self)
{
	ut_array_clear(&self->buf);
}

int ut_heap_reserve(ut_heap_t *self, size_t additional)
{
	return ut_array_reserve(&self->buf, additional);
}

int ut_heap_push(ut_heap_t *self, const void *data)
{
	size_t i;
	int e;

	e = ut_array_push(&self->buf, data);
	if (e)
		return e;

	if (self->buf.len > 1) {
		i = self->buf.len >> 1;
		for (;;) {
			ut_array_sift_down(&self->buf, i);
			if (i-- == 0)
				break;
		}
	}

	return e;
}

void ut_heap_pop(ut_heap_t *self)
{
	if (self->buf.len > 1)
		ut_array_do_swap(&self->buf, 0, self->buf.len - 1);

	ut_array_pop(&self->buf);

	if (self->buf.len > 1)
		ut_array_sift_down(&self->buf, 0);
}

void *ut_heap_peek(ut_heap_t *self)
{
	return ut_array_first(&self->buf);
}

size_t ut_heap_capacity(const ut_heap_t *self)
{
	return self->buf.cap;
}

size_t ut_heap_length(const ut_heap_t *self)
{
	return self->buf.len;
}

bool ut_heap_is_empty(const ut_heap_t *self)
{
	return self->buf.len == 0;
}

struct ut_iter *ut_heap_iter_new(ut_heap_t *heap)
{
	return ut_array_iter_new(&heap->buf);
}

void ut_heap_iter_delete(struct ut_iter *self)
{
	ut_array_iter_delete(self);
}
