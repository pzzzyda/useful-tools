#ifndef _UT_ARRAY_H
#define _UT_ARRAY_H

#include "ut_iter.h"
#include "ut_type.h"

typedef struct __ut_array ut_array_t;

ut_array_t *ut_array_new(const struct ut_type *element);

void ut_array_delete(ut_array_t *self);

void ut_array_clear(ut_array_t *self);

int ut_array_reserve(ut_array_t *self, size_t additional);

int ut_array_push(ut_array_t *self, const void *data);

void ut_array_pop(ut_array_t *self);

int ut_array_insert(ut_array_t *self, size_t index, const void *data);

void ut_array_remove(ut_array_t *self, size_t index);

void *ut_array_get(ut_array_t *self, size_t index);

void *ut_array_first(ut_array_t *self);

void *ut_array_last(ut_array_t *self);

size_t ut_array_capacity(const ut_array_t *self);

size_t ut_array_length(const ut_array_t *self);

bool ut_array_is_empty(const ut_array_t *self);

struct ut_iter *ut_array_iter_new(ut_array_t *array);

void ut_array_iter_delete(struct ut_iter *self);

#endif /* ut_array.h */
