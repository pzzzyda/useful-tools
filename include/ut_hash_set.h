#ifndef _UT_HASH_SET_H
#define _UT_HASH_SET_H

#include "ut_iter.h"
#include "ut_type.h"

typedef struct __ut_hash_set ut_hash_set_t;

ut_hash_set_t *ut_hash_set_new(const struct ut_type *element);

void ut_hash_set_delete(ut_hash_set_t *self);

void ut_hash_set_clear(ut_hash_set_t *self);

int ut_hash_set_insert(ut_hash_set_t *self, const void *data);

void ut_hash_set_remove(ut_hash_set_t *self, const void *data);

void *ut_hash_set_get(ut_hash_set_t *self, const void *data);

size_t ut_hash_set_length(const ut_hash_set_t *self);

bool ut_hash_set_is_empty(const ut_hash_set_t *self);

struct ut_iter *ut_hash_set_iter_new(ut_hash_set_t *set);

void ut_hash_set_iter_delete(struct ut_iter *self);

#endif /* ut_hash_set.h */
