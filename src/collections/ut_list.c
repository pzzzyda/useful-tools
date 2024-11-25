#include "ut_list.h"
#include "ut_errno.h"
#include <stdlib.h>
#include <string.h>

struct __ut_list {
	ut_list_entry_t *head;
	ut_list_entry_t *tail;
	size_t len;
	const struct ut_type *element;
};

struct __ut_list_entry {
	ut_list_entry_t *prev;
	ut_list_entry_t *next;
	const struct ut_type *element;
};

struct __ut_list_iter {
	struct ut_iter base;
	ut_list_entry_t *curr;
};

static inline void *ut_list_entry_data(ut_list_entry_t *self)
{
	return (uint8_t *)self + sizeof(ut_list_entry_t);
}

static ut_list_entry_t *ut_list_entry_new(const struct ut_type *element,
					  const void *data)
{
	ut_list_entry_t *self;
	size_t size;

	size = sizeof(ut_list_entry_t) + element->size;
	self = malloc(size);
	if (!self)
		return NULL;

	self->prev = NULL;
	self->next = NULL;
	self->element = element;
	memcpy(ut_list_entry_data(self), data, element->size);
	return self;
}

static void ut_list_entry_delete(ut_list_entry_t *self)
{
	if (self->element->drop)
		self->element->drop(ut_list_entry_data(self));
	free(self);
}

ut_list_t *ut_list_new(const struct ut_type *element)
{
	ut_list_t *self;

	if (!element || !element->size)
		return NULL;

	self = malloc(sizeof(ut_list_t));
	if (!self)
		return NULL;

	self->head = NULL;
	self->tail = NULL;
	self->len = 0;
	self->element = element;
	return self;
}

void ut_list_delete(ut_list_t *self)
{
	ut_list_clear(self);
	free(self);
}

void ut_list_clear(ut_list_t *self)
{
	if (!self->len)
		return;

	while (self->head) {
		self->tail = self->head->next;
		ut_list_entry_delete(self->head);
		self->head = self->tail;
		self->len--;
	}
}

int ut_list_push_front(ut_list_t *self, const void *data)
{
	ut_list_entry_t *entry;

	if (!data)
		return UT_EINVAL;

	entry = ut_list_entry_new(self->element, data);
	if (!entry)
		return UT_ENOMEM;
	entry->next = self->head;
	if (self->head)
		self->head->prev = entry;
	self->head = entry;
	if (!self->len)
		self->tail = entry;
	self->len++;
	return UT_OK;
}

int ut_list_push_back(ut_list_t *self, const void *data)
{
	ut_list_entry_t *entry;

	if (!data)
		return UT_EINVAL;

	entry = ut_list_entry_new(self->element, data);
	if (!entry)
		return UT_ENOMEM;
	entry->prev = self->tail;
	if (self->tail)
		self->tail->next = entry;
	self->tail = entry;
	if (!self->len)
		self->head = entry;
	self->len++;
	return UT_OK;
}

void ut_list_pop_front(ut_list_t *self)
{
	ut_list_entry_t *entry;

	if (!self->len)
		return;

	entry = self->head;
	self->head = self->head->next;
	if (self->head)
		self->head->prev = NULL;
	self->len--;
	if (!self->len)
		self->tail = NULL;
	ut_list_entry_delete(entry);
}

void ut_list_pop_back(ut_list_t *self)
{
	ut_list_entry_t *entry;

	if (!self->len)
		return;

	entry = self->tail;
	self->tail = self->tail->prev;
	if (self->tail)
		self->tail->next = NULL;
	self->len--;
	if (!self->len)
		self->head = NULL;
	ut_list_entry_delete(entry);
}

void *ut_list_front(ut_list_t *self)
{
	if (!self->len)
		return NULL;

	return ut_list_entry_data(self->head);
}

void *ut_list_back(ut_list_t *self)
{
	if (!self->len)
		return NULL;

	return ut_list_entry_data(self->tail);
}

size_t ut_list_length(const ut_list_t *self)
{
	return self->len;
}

bool ut_list_is_empty(const ut_list_t *self)
{
	return self->len == 0;
}

static void *ut_list_iter_next(struct __ut_list_iter *self)
{
	void *data;

	if (!self->curr)
		return NULL;

	data = ut_list_entry_data(self->curr);
	self->curr = self->curr->next;
	return data;
}

struct ut_iter *ut_list_iter_new(ut_list_t *list)
{
	struct __ut_list_iter *self;

	if (!list)
		return NULL;

	self = malloc(sizeof(struct __ut_list_iter));
	if (!self)
		return NULL;

	self->base.next = (void *)&ut_list_iter_next;
	self->curr = list->head;
	return &self->base;
}

void ut_list_iter_delete(struct ut_iter *self)
{
	free(self);
}
