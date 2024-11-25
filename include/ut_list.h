#ifndef _UT_LIST_H
#define _UT_LIST_H

#include "ut_iter.h"
#include "ut_type.h"

typedef struct __ut_list_entry ut_list_entry_t;

typedef struct __ut_list ut_list_t;

ut_list_t *ut_list_new(const struct ut_type *element);

void ut_list_delete(ut_list_t *self);

void ut_list_clear(ut_list_t *self);

int ut_list_push_front(ut_list_t *self, const void *data);

int ut_list_push_back(ut_list_t *self, const void *data);

void ut_list_pop_front(ut_list_t *self);

void ut_list_pop_back(ut_list_t *self);

void *ut_list_front(ut_list_t *self);

void *ut_list_back(ut_list_t *self);

size_t ut_list_length(const ut_list_t *self);

bool ut_list_is_empty(const ut_list_t *self);

struct ut_iter *ut_list_iter_new(ut_list_t *list);

void ut_list_iter_delete(struct ut_iter *self);

#endif /* ut_list.h */
