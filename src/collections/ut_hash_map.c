#include "ut_hash_map.h"
#include "ut_errno.h"
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

struct __ut_hash_map_iter {
	struct ut_iter base;
	ut_hash_map_t *map;
	size_t index;
	ut_hash_entry_t *curr;
	struct ut_pair kv;
};

static inline void *ut_hash_entry_key(ut_hash_entry_t *self)
{
	return (uint8_t *)self + sizeof(ut_hash_entry_t);
}

static inline void *ut_hash_entry_value(ut_hash_entry_t *self)
{
	return (uint8_t *)ut_hash_entry_key(self) + self->key->size;
}

static ut_hash_entry_t *ut_hash_entry_new(const struct ut_type *key,
					  const void *key_data,
					  const struct ut_type *value,
					  const void *value_data)
{
	ut_hash_entry_t *self;
	size_t size;

	size = sizeof(ut_hash_entry_t) + key->size + value->size;
	self = malloc(size);
	if (!self)
		return NULL;

	self->key = key;
	self->value = value;
	self->next = NULL;
	self->hash = key->hash(key_data);
	memcpy(ut_hash_entry_key(self), key_data, key->size);
	memcpy(ut_hash_entry_value(self), value_data, value->size);
	return self;
}

static void ut_hash_entry_delete(ut_hash_entry_t *self)
{
	if (self->key->drop)
		self->key->drop(ut_hash_entry_key(self));
	if (self->value->drop)
		self->value->drop(ut_hash_entry_value(self));
	free(self);
}

static void ut_hash_entry_update(ut_hash_entry_t *self, const void *value)
{
	if (self->value->drop)
		self->value->drop(ut_hash_entry_value(self));
	memcpy(ut_hash_entry_value(self), value, self->value->size);
}

static inline bool ut_hash_map_need_grow(ut_hash_map_t *self)
{
	return self->len >= (self->count * 3) >> 2;
}

static ut_hash_entry_t **ut_hash_map_get_entry(ut_hash_map_t *self,
					       const void *key)
{
	size_t hash = self->key->hash(key);
	ut_hash_entry_t **curr = &self->buckets[hash & (self->count - 1)];

	while (*curr) {
		if ((*curr)->hash == hash) {
			if (!self->key->compare(ut_hash_entry_key(*curr), key))
				break;
		}
		curr = &(*curr)->next;
	}
	return curr;
}

static void ut_hash_map_rehash(ut_hash_map_t *self,
			       ut_hash_entry_t **new_buckets, size_t new_count)
{
	ut_hash_entry_t *curr, *next;
	size_t i, index;

	for (i = 0; i < self->count; i++) {
		curr = self->buckets[i];
		while (curr) {
			next = curr->next;
			index = curr->hash & (new_count - 1);
			curr->next = new_buckets[index];
			new_buckets[index] = curr;
			curr = next;
		}
	}

	free(self->buckets);
	self->buckets = new_buckets;
	self->count = new_count;
}

static int ut_hash_map_grow(ut_hash_map_t *self)
{
	ut_hash_entry_t **new_buckets;
	size_t new_count;

	new_count = self->count << 1;
	new_buckets = calloc(new_count, sizeof(ut_hash_entry_t *));

	if (!new_buckets)
		return UT_ENOMEM;

	ut_hash_map_rehash(self, new_buckets, new_count);
	return UT_OK;
}

ut_hash_map_t *ut_hash_map_new(const struct ut_type *key,
			       const struct ut_type *value)
{
	ut_hash_map_t *self;

	if (!key || !key->size || !value)
		return NULL;

	self = malloc(sizeof(ut_hash_map_t));
	if (!self)
		return NULL;

	self->buckets = calloc(8, sizeof(ut_hash_entry_t *));
	if (!self->buckets) {
		free(self);
		return NULL;
	}

	self->count = 8;
	self->len = 0;
	self->key = key;
	self->value = value;
	return self;
}

void ut_hash_map_delete(ut_hash_map_t *self)
{
	ut_hash_map_clear(self);
	free(self->buckets);
	free(self);
}

void ut_hash_map_clear(ut_hash_map_t *self)
{
	ut_hash_entry_t *curr, *next;
	size_t i;

	if (!self->len)
		return;

	for (i = 0; i < self->count; i++) {
		curr = self->buckets[i];
		while (curr) {
			next = curr->next;
			ut_hash_entry_delete(curr);
			curr = next;
		}
	}
}

int ut_hash_map_insert(ut_hash_map_t *self, const void *key, const void *value)
{
	ut_hash_entry_t **entry;

	if (!key || !value)
		return UT_EINVAL;

	entry = ut_hash_map_get_entry(self, key);

	if (*entry) {
		ut_hash_entry_update(*entry, value);
		return UT_OK;
	} else {
		*entry = ut_hash_entry_new(self->key, key, self->value, value);
		if (!*entry)
			return UT_ENOMEM;

		self->len++;
		if (ut_hash_map_need_grow(self))
			ut_hash_map_grow(self);

		return UT_OK;
	}
}

void ut_hash_map_remove(ut_hash_map_t *self, const void *key)
{
	ut_hash_entry_t **entry, *next;

	if (!key)
		return;

	entry = ut_hash_map_get_entry(self, key);

	if (*entry) {
		next = (*entry)->next;
		ut_hash_entry_delete(*entry);
		*entry = next;
	}
}

void *ut_hash_map_get(ut_hash_map_t *self, const void *key)
{
	return ut_hash_map_get_key_value(self, key).value;
}

struct ut_pair ut_hash_map_get_key_value(ut_hash_map_t *self, const void *key)
{
	ut_hash_entry_t **entry;
	struct ut_pair kv = { NULL, NULL };

	if (!key)
		return kv;

	entry = ut_hash_map_get_entry(self, key);

	if (*entry) {
		kv.key = ut_hash_entry_key(*entry);
		kv.value = ut_hash_entry_value(*entry);
	}

	return kv;
}

size_t ut_hash_map_length(const ut_hash_map_t *self)
{
	return self->len;
}

bool ut_hash_map_is_empty(const ut_hash_map_t *self)
{
	return self->len == 0;
}

static void ut_hash_map_iter_next_bucket(struct __ut_hash_map_iter *self)
{
	ut_hash_map_t *map = self->map;

	while (self->index < map->count && !map->buckets[self->index])
		self->index++;

	if (self->index < self->map->count)
		self->curr = self->map->buckets[self->index++];
	else
		self->curr = NULL;
}

static void *ut_hash_map_iter_next(struct __ut_hash_map_iter *self)
{
	if (!self->curr)
		return NULL;

	self->kv.key = ut_hash_entry_key(self->curr);
	self->kv.value = ut_hash_entry_value(self->curr);

	if (self->curr->next)
		self->curr = self->curr->next;
	else
		ut_hash_map_iter_next_bucket(self);

	return &self->kv;
}

struct ut_iter *ut_hash_map_iter_new(ut_hash_map_t *map)
{
	struct __ut_hash_map_iter *self;

	if (!map)
		return NULL;

	self = malloc(sizeof(struct __ut_hash_map_iter));
	if (!self)
		return NULL;

	self->base.next = (void *)&ut_hash_map_iter_next;
	self->map = map;
	self->index = 0;
	ut_hash_map_iter_next_bucket(self);
	return (struct ut_iter *)self;
}

void ut_hash_map_iter_delete(struct ut_iter *self)
{
	free(self);
}
