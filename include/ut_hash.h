#ifndef _UT_HASH_H
#define _UT_HASH_H

#include <stddef.h>
#include <stdint.h>

/* Jenkins One-at-a-Time Hash. */
uint32_t ut_jenkins(const void *key, size_t len);

/* BKDR Hash. */
uint32_t ut_bkdr(const char *key);

#endif /* ut_hash.h */
