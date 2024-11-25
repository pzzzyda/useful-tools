#ifndef _UT_HEAP_H
#define _UT_HEAP_H

#include "ut_iter.h"
#include "ut_type.h"

typedef struct __ut_heap ut_heap_t;

ut_heap_t *ut_heap_new(const struct ut_type *element);

void ut_heap_delete(ut_heap_t *self);

void ut_heap_clear(ut_heap_t *self);

int ut_heap_reserve(ut_heap_t *self, size_t additional);

int ut_heap_push(ut_heap_t *self, const void *data);

void ut_heap_pop(ut_heap_t *self);

void *ut_heap_peek(ut_heap_t *self);

size_t ut_heap_capacity(const ut_heap_t *self);

size_t ut_heap_length(const ut_heap_t *self);

bool ut_heap_is_empty(const ut_heap_t *self);

struct ut_iter *ut_heap_iter_new(ut_heap_t *heap);

void ut_heap_iter_delete(struct ut_iter *self);

#endif /* ut_heap.h */
