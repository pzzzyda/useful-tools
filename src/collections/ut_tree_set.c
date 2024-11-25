#include "ut_tree_set.h"
#include "ut_tree_map.h"
#include <stdlib.h>

struct __ut_tree_map {
	ut_tree_entry_t *root;
	size_t len;
	const struct ut_type *key;
	const struct ut_type *value;
};

struct __ut_tree_entry {
	int color;
	ut_tree_entry_t *left;
	ut_tree_entry_t *right;
	ut_tree_entry_t *parent;
	const struct ut_type *key;
	const struct ut_type *value;
};

struct __ut_tree_set {
	ut_tree_map_t map;
};

struct __ut_tree_set_iter {
	struct ut_iter base;
	struct ut_iter *inner;
};

static const struct ut_type __ut_null = {
	.size = 0,
	.drop = NULL,
	.compare = NULL,
	.hash = NULL,
};

ut_tree_set_t *ut_tree_set_new(const struct ut_type *element)
{
	ut_tree_set_t *self;

	if (!element || !element->size)
		return NULL;

	self = malloc(sizeof(ut_tree_set_t));
	if (!self)
		return NULL;

	self->map.root = NULL;
	self->map.len = 0;
	self->map.key = element;
	self->map.value = &__ut_null;
	return self;
}

void ut_tree_set_delete(ut_tree_set_t *self)
{
	ut_tree_map_delete(&self->map);
}

void ut_tree_set_clear(ut_tree_set_t *self)
{
	ut_tree_map_clear(&self->map);
}

int ut_tree_set_insert(ut_tree_set_t *self, const void *data)
{
	return ut_tree_map_insert(&self->map, data, data);
}

void ut_tree_set_remove(ut_tree_set_t *self, const void *data)
{
	ut_tree_map_remove(&self->map, data);
}

void *ut_tree_set_get(ut_tree_set_t *self, const void *data)
{
	return ut_tree_map_get_key_value(&self->map, data).key;
}

size_t ut_tree_set_length(const ut_tree_set_t *self)
{
	return self->map.len;
}

bool ut_tree_set_is_empty(const ut_tree_set_t *self)
{
	return self->map.len == 0;
}

static void *ut_tree_set_iter_next(struct __ut_tree_set_iter *self)
{
	struct ut_pair *kv = self->inner->next(self->inner);

	if (kv)
		return kv->key;
	else
		return NULL;
}

struct ut_iter *ut_tree_set_iter_new(ut_tree_set_t *set)
{
	struct __ut_tree_set_iter *self;

	if (!set)
		return NULL;

	self = malloc(sizeof(struct __ut_tree_set_iter));
	if (!self)
		return NULL;

	self->inner = ut_tree_map_iter_new((ut_tree_map_t *)set);
	if (!self->inner) {
		free(self);
		return NULL;
	}

	self->base.next = (void *)&ut_tree_set_iter_next;
	return (struct ut_iter *)self;
}

void ut_tree_set_iter_delete(struct ut_iter *self)
{
	ut_tree_map_iter_delete(((struct __ut_tree_set_iter *)self)->inner);
	free(self);
}
