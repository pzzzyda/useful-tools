#include "ut_hash_set.h"
#include "ut_hash_map.h"
#include <stdlib.h>
#include <string.h>

struct __ut_hash_map {
	ut_hash_entry_t **buckets;
	size_t count;
	size_t len;
	const struct ut_type *key;
	const struct ut_type *value;
};

struct __ut_hash_entry {
	ut_hash_entry_t *next;
	size_t hash;
	const struct ut_type *key;
	const struct ut_type *value;
};

struct __ut_hash_set {
	ut_hash_map_t map;
};

struct __ut_hash_set_iter {
	struct ut_iter base;
	struct ut_iter *inner;
};

static const struct ut_type __ut_null = {
	.size = 0,
	.drop = NULL,
	.compare = NULL,
	.hash = NULL,
};

ut_hash_set_t *ut_hash_set_new(const struct ut_type *element)
{
	ut_hash_set_t *self;

	if (!element || !element->size)
		return NULL;

	self = malloc(sizeof(ut_hash_set_t));
	if (!self)
		return NULL;

	self->map.buckets = calloc(8, sizeof(ut_hash_entry_t *));
	if (!self->map.buckets) {
		free(self);
		return NULL;
	}

	self->map.count = 8;
	self->map.len = 0;
	self->map.key = element;
	self->map.value = &__ut_null;
	return self;
}

void ut_hash_set_delete(ut_hash_set_t *self)
{
	ut_hash_map_delete(&self->map);
}

void ut_hash_set_clear(ut_hash_set_t *self)
{
	ut_hash_map_clear(&self->map);
}

int ut_hash_set_insert(ut_hash_set_t *self, const void *data)
{
	return ut_hash_map_insert(&self->map, data, data);
}

void ut_hash_set_remove(ut_hash_set_t *self, const void *data)
{
	ut_hash_map_remove(&self->map, data);
}

void *ut_hash_set_get(ut_hash_set_t *self, const void *data)
{
	return ut_hash_map_get_key_value(&self->map, data).key;
}

size_t ut_hash_set_length(const ut_hash_set_t *self)
{
	return self->map.len;
}

bool ut_hash_set_is_empty(const ut_hash_set_t *self)
{
	return self->map.len == 0;
}

static void *ut_hash_set_iter_next(struct __ut_hash_set_iter *self)
{
	struct ut_pair *kv = self->inner->next(self->inner);

	if (kv)
		return kv->key;
	else
		return NULL;
}

struct ut_iter *ut_hash_set_iter_new(ut_hash_set_t *set)
{
	struct __ut_hash_set_iter *self;

	if (!set)
		return NULL;

	self = malloc(sizeof(struct __ut_hash_set_iter));
	if (!self)
		return NULL;

	self->inner = ut_hash_map_iter_new((ut_hash_map_t *)set);
	if (!self->inner) {
		free(self);
		return NULL;
	}

	self->base.next = (void *)&ut_hash_set_iter_next;
	return (struct ut_iter *)self;
}

void ut_hash_set_iter_delete(struct ut_iter *self)
{
	ut_hash_map_iter_delete(((struct __ut_hash_set_iter *)self)->inner);
	free(self);
}
