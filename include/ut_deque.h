#ifndef _UT_DEQUE_H
#define _UT_DEQUE_H

#include "ut_iter.h"
#include "ut_type.h"

typedef struct __ut_deque ut_deque_t;

ut_deque_t *ut_deque_new(const struct ut_type *element);

void ut_deque_delete(ut_deque_t *self);

void ut_deque_clear(ut_deque_t *self);

int ut_deque_reserve(ut_deque_t *self, size_t additional);

int ut_deque_push_front(ut_deque_t *self, const void *data);

int ut_deque_push_back(ut_deque_t *self, const void *data);

void ut_deque_pop_front(ut_deque_t *self);

void ut_deque_pop_back(ut_deque_t *self);

int ut_deque_insert(ut_deque_t *self, size_t index, const void *data);

void ut_deque_remove(ut_deque_t *self, size_t index);

void *ut_deque_get(ut_deque_t *self, size_t index);

void *ut_deque_front(ut_deque_t *self);

void *ut_deque_back(ut_deque_t *self);

size_t ut_deque_capacity(const ut_deque_t *self);

size_t ut_deque_length(const ut_deque_t *self);

bool ut_deque_is_empty(const ut_deque_t *self);

struct ut_iter *ut_deque_iter_new(ut_deque_t *deque);

void ut_deque_iter_delete(struct ut_iter *self);

#endif /* ut_deque.h */
