#include "ut_deque.h"
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

struct __ut_deque {
	size_t head;
	ut_array_t buf;
};

struct __ut_deque_iter {
	struct ut_iter base;
	ut_deque_t *deque;
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

static inline void *ut_deque_index(ut_deque_t *self, size_t index)
{
	index = (self->head + index) % self->buf.cap;
	return ut_array_index(&self->buf, index);
}

static inline void ut_deque_head_sub(ut_deque_t *self)
{
	self->head += self->buf.cap;
	self->head -= !!self->buf.len;
	self->head %= self->buf.cap;
}

static inline void ut_deque_head_add(ut_deque_t *self)
{
	self->head += !!self->buf.len;
	self->head %= self->buf.cap;
}

ut_deque_t *ut_deque_new(const struct ut_type *element)
{
	ut_deque_t *self;

	if (!element || !element->size)
		return NULL;

	self = malloc(sizeof(ut_deque_t));
	if (!self)
		return NULL;

	self->head = 0;
	self->buf.ptr = NULL;
	self->buf.cap = 0;
	self->buf.len = 0;
	self->buf.element = element;
	return self;
}

void ut_deque_delete(ut_deque_t *self)
{
	ut_deque_clear(self);
	free(self->buf.ptr);
	free(self);
}

void ut_deque_clear(ut_deque_t *self)
{
	if (!self->buf.len)
		return;

	if (self->buf.element->drop) {
		while (self->buf.len) {
			self->buf.element->drop(ut_deque_back(self));
			self->buf.len--;
		}
	} else {
		self->buf.len -= self->buf.len;
	}
}

int ut_deque_reserve(ut_deque_t *self, size_t additional)
{
	size_t old_cap, diff, n;

	old_cap = self->buf.cap;
	if (ut_array_reserve(&self->buf, additional))
		return UT_ENOMEM;

	if (self->head + self->buf.len > old_cap) {
		/*
		 *             H
		 * [ o o o . . o o o ] . . . . . . . . ]
		 *
		 *                             H
		 * [ o o o . . . . . . . . . . o o o ]
		 */
		n = old_cap - self->head;
		diff = self->buf.cap - old_cap;
		ut_array_move(&self->buf, n, self->head, self->head + diff);
		self->head += diff;
	}
	return UT_OK;
}

int ut_deque_push_front(ut_deque_t *self, const void *data)
{
	if (!data)
		return UT_EINVAL;

	if (self->buf.len >= self->buf.cap && ut_deque_reserve(self, 1))
		return UT_ENOMEM;

	ut_deque_head_sub(self);
	memcpy(ut_deque_index(self, 0), data, self->buf.element->size);
	self->buf.len++;
	return UT_OK;
}

int ut_deque_push_back(ut_deque_t *self, const void *data)
{
	if (!data)
		return UT_EINVAL;

	if (self->buf.len >= self->buf.cap && ut_deque_reserve(self, 1))
		return UT_ENOMEM;

	memcpy(ut_deque_index(self, self->buf.len), data,
	       self->buf.element->size);
	self->buf.len++;
	return UT_OK;
}

void ut_deque_pop_front(ut_deque_t *self)
{
	if (!self->buf.len)
		return;

	if (self->buf.element->drop)
		self->buf.element->drop(ut_deque_front(self));
	self->buf.len--;
	ut_deque_head_add(self);
}

void ut_deque_pop_back(ut_deque_t *self)
{
	if (!self->buf.len)
		return;

	if (self->buf.element->drop)
		self->buf.element->drop(ut_deque_back(self));
	self->buf.len--;
}

int ut_deque_insert(ut_deque_t *self, size_t index, const void *data)
{
	size_t tail, n, from, to;

	if (!data)
		return UT_EINVAL;

	if (index > self->buf.len)
		return UT_ERANGE;

	if (index == self->buf.len)
		return ut_deque_push_back(self, data);

	if (index == 0)
		return ut_deque_push_front(self, data);

	/* Convert to a physical index. */
	index = (self->head + index) % self->buf.cap;

	tail = (self->head + self->buf.len) % self->buf.cap;

	if (index < tail) {
		/*
		 *           H     I     T
		 * Case 1: [ o o o o o o . . ]
		 *
		 *             I     T   H
		 *   or    [ o o o o . . o o ]
		 */
		n = tail - index;
		from = index;
		to = index + 1;
		index = index;
	} else {
		/*
		 *               T   H   I
		 * Case 2: [ o o . . o o o o ]
		 */
		n = index - self->head;
		from = self->head;
		to = self->head - 1;
		index = index - 1;
		self->head -= 1;
	}
	ut_array_move(&self->buf, n, from, to);
	memcpy(ut_array_index(&self->buf, index), data,
	       self->buf.element->size);
	self->buf.len++;
	return UT_OK;
}

void ut_deque_remove(ut_deque_t *self, size_t index)
{
	size_t tail, n, from, to;

	if (index >= self->buf.len)
		return;

	if (index == self->buf.len - 1) {
		ut_deque_pop_back(self);
		return;
	}

	if (index == 0) {
		ut_deque_pop_front(self);
		return;
	}

	if (self->buf.element->drop)
		self->buf.element->drop(ut_deque_index(self, index));

	/* Convert to a physical index. */
	index = (self->head + index) % self->buf.cap;

	tail = (self->head + self->buf.len) % self->buf.cap;

	if (index < tail) {
		/*
		 *           H     I     T
		 * Case 1: [ o o o o o o . . ]
		 *
		 *             I     T   H
		 *         [ o o o o . . o o ]
		 */
		n = tail - index - 1;
		from = index + 1;
		to = index;
	} else {
		/*
		 *               T   H   I
		 * Case 2: [ o o . . o o o o ]
		 */
		n = index - self->head;
		from = self->head;
		to = self->head + 1;
		self->head += 1;
	}
	ut_array_move(&self->buf, n, from, to);
	self->buf.len--;
}

void *ut_deque_get(ut_deque_t *self, size_t index)
{
	if (index >= self->buf.len)
		return NULL;

	return ut_deque_index(self, index);
}

void *ut_deque_front(ut_deque_t *self)
{
	return ut_deque_get(self, 0);
}

void *ut_deque_back(ut_deque_t *self)
{
	return ut_deque_get(self, self->buf.len - 1);
}

size_t ut_deque_capacity(const ut_deque_t *self)
{
	return self->buf.cap;
}

size_t ut_deque_length(const ut_deque_t *self)
{
	return self->buf.len;
}

bool ut_deque_is_empty(const ut_deque_t *self)
{
	return self->buf.len == 0;
}

static void *ut_deque_iter_next(struct __ut_deque_iter *self)
{
	if (self->curr >= self->deque->buf.len)
		return NULL;

	return ut_deque_index(self->deque, self->curr++);
}

struct ut_iter *ut_deque_iter_new(ut_deque_t *deque)
{
	struct __ut_deque_iter *self;

	if (!deque)
		return NULL;

	self = malloc(sizeof(struct __ut_deque_iter));
	if (!self)
		return NULL;

	self->base.next = (void *)&ut_deque_iter_next;
	self->deque = deque;
	self->curr = 0;
	return &self->base;
}

void ut_deque_iter_delete(struct ut_iter *self)
{
	free(self);
}
