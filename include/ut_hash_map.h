#ifndef _UT_HASH_MAP_H
#define _UT_HASH_MAP_H

#include "ut_iter.h"
#include "ut_pair.h"
#include "ut_type.h"

typedef struct __ut_hash_map ut_hash_map_t;

typedef struct __ut_hash_entry ut_hash_entry_t;

ut_hash_map_t *ut_hash_map_new(const struct ut_type *key,
			       const struct ut_type *value);

void ut_hash_map_delete(ut_hash_map_t *self);

void ut_hash_map_clear(ut_hash_map_t *self);

int ut_hash_map_insert(ut_hash_map_t *self, const void *key, const void *value);

void ut_hash_map_remove(ut_hash_map_t *self, const void *key);

void *ut_hash_map_get(ut_hash_map_t *self, const void *key);

struct ut_pair ut_hash_map_get_key_value(ut_hash_map_t *self, const void *key);

size_t ut_hash_map_length(const ut_hash_map_t *self);

bool ut_hash_map_is_empty(const ut_hash_map_t *self);

struct ut_iter *ut_hash_map_iter_new(ut_hash_map_t *map);

void ut_hash_map_iter_delete(struct ut_iter *self);

#endif /* ut_hash_map.h */
