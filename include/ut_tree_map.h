#ifndef _UT_TREE_MAP_H
#define _UT_TREE_MAP_H

#include "ut_iter.h"
#include "ut_pair.h"
#include "ut_type.h"

typedef struct __ut_tree_map ut_tree_map_t;

typedef struct __ut_tree_entry ut_tree_entry_t;

ut_tree_map_t *ut_tree_map_new(const struct ut_type *key,
			       const struct ut_type *value);

void ut_tree_map_delete(ut_tree_map_t *self);

void ut_tree_map_clear(ut_tree_map_t *self);

int ut_tree_map_insert(ut_tree_map_t *self, const void *key, const void *value);

void ut_tree_map_remove(ut_tree_map_t *self, const void *key);

void *ut_tree_map_get(ut_tree_map_t *self, const void *key);

struct ut_pair ut_tree_map_get_key_value(ut_tree_map_t *self, const void *key);

size_t ut_tree_map_length(const ut_tree_map_t *self);

bool ut_tree_map_is_empty(const ut_tree_map_t *self);

struct ut_iter *ut_tree_map_iter_new(ut_tree_map_t *map);

void ut_tree_map_iter_delete(struct ut_iter *self);

#endif /* ut_tree_map.h */
