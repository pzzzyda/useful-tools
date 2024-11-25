#ifndef _UT_ITER_H
#define _UT_ITER_H

struct ut_iter {
	void *(*next)(struct ut_iter *);
};

#endif /* ut_iter.h */
