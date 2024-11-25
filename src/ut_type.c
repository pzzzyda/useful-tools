#include "ut_type.h"
#include "ut_hash.h"
#include <math.h>

#define UT_FLOAT_EPSILON 1e-6
#define UT_DOUBLE_EPSILON 1e-10

static int ut_char_compare(const signed char *self, const signed char *other)
{
	return *self - *other;
}

static size_t ut_char_hash(const signed char *self)
{
	return ut_jenkins(self, sizeof(*self));
}

static int ut_short_compare(const signed short *self, const signed short *other)
{
	return *self - *other;
}

static size_t ut_short_hash(const signed short *self)
{
	return ut_jenkins(self, sizeof(*self));
}

static int ut_int_compare(const signed int *self, const signed int *other)
{
	return *self - *other;
}

static size_t ut_int_hash(const signed int *self)
{
	return ut_jenkins(self, sizeof(*self));
}

static int ut_long_compare(const signed long *self, const signed long *other)
{
	return *self - *other;
}

static size_t ut_long_hash(const signed long *self)
{
	return ut_jenkins(self, sizeof(*self));
}

static int ut_uchar_compare(const unsigned char *self,
			    const unsigned char *other)
{
	unsigned char a = *self;
	unsigned char b = *other;

	return a > b ? 1 : a == b ? 0 : -1;
}

static size_t ut_uchar_hash(const unsigned char *self)
{
	return ut_jenkins(self, sizeof(*self));
}

static int ut_ushort_compare(const unsigned short *self,
			     const unsigned short *other)
{
	unsigned short a = *self;
	unsigned short b = *other;

	return a > b ? 1 : a == b ? 0 : -1;
}

static size_t ut_ushort_hash(const unsigned short *self)
{
	return ut_jenkins(self, sizeof(*self));
}

static int ut_uint_compare(const unsigned int *self, const unsigned int *other)
{
	unsigned int a = *self;
	unsigned int b = *other;

	return a > b ? 1 : a == b ? 0 : -1;
}

static size_t ut_uint_hash(const unsigned int *self)
{
	return ut_jenkins(self, sizeof(*self));
}

static int ut_ulong_compare(const unsigned long *self,
			    const unsigned long *other)
{
	unsigned long a = *self;
	unsigned long b = *other;

	return a > b ? 1 : a == b ? 0 : -1;
}

static size_t ut_ulong_hash(const unsigned long *self)
{
	return ut_jenkins(self, sizeof(*self));
}

static int ut_float_compare(const float *self, const float *other)
{
	float a = *self;
	float b = *other;

	if (fabs(a - b) < UT_FLOAT_EPSILON)
		return 0;

	return (a < b) ? -1 : 1;
}

static size_t ut_float_hash(const float *self)
{
	return ut_jenkins(self, sizeof(*self));
}

static int ut_double_compare(const double *self, const double *other)
{
	double a = *self;
	double b = *other;

	if (fabs(a - b) < UT_DOUBLE_EPSILON)
		return 0;

	return (a < b) ? -1 : 1;
}

static size_t ut_double_hash(const double *self)
{
	return ut_jenkins(self, sizeof(*self));
}

static const struct ut_type __ut_type_char = {
	.size = sizeof(signed char),
	.drop = NULL,
	.compare = (void *)&ut_char_compare,
	.hash = (void *)&ut_char_hash,
};

static const struct ut_type __ut_type_short = {
	.size = sizeof(signed short),
	.drop = NULL,
	.compare = (void *)&ut_short_compare,
	.hash = (void *)&ut_short_hash,
};

static const struct ut_type __ut_type_int = {
	.size = sizeof(signed int),
	.drop = NULL,
	.compare = (void *)&ut_int_compare,
	.hash = (void *)&ut_int_hash,
};

static const struct ut_type __ut_type_long = {
	.size = sizeof(signed long),
	.drop = NULL,
	.compare = (void *)&ut_long_compare,
	.hash = (void *)&ut_long_hash,
};

static const struct ut_type __ut_type_uchar = {
	.size = sizeof(unsigned char),
	.drop = NULL,
	.compare = (void *)&ut_uchar_compare,
	.hash = (void *)&ut_uchar_hash,
};

static const struct ut_type __ut_type_ushort = {
	.size = sizeof(unsigned short),
	.drop = NULL,
	.compare = (void *)&ut_ushort_compare,
	.hash = (void *)&ut_ushort_hash,
};

static const struct ut_type __ut_type_uint = {
	.size = sizeof(unsigned int),
	.drop = NULL,
	.compare = (void *)&ut_uint_compare,
	.hash = (void *)&ut_uint_hash,
};

static const struct ut_type __ut_type_ulong = {
	.size = sizeof(unsigned long),
	.drop = NULL,
	.compare = (void *)&ut_ulong_compare,
	.hash = (void *)&ut_ulong_hash,
};

static const struct ut_type __ut_type_float = {
	.size = sizeof(float),
	.drop = NULL,
	.compare = (void *)&ut_float_compare,
	.hash = (void *)&ut_float_hash,
};

static const struct ut_type __ut_type_double = {
	.size = sizeof(double),
	.drop = NULL,
	.compare = (void *)&ut_double_compare,
	.hash = (void *)&ut_double_hash,
};

const struct ut_type *ut_type_char()
{
	return &__ut_type_char;
}

const struct ut_type *ut_type_short()
{
	return &__ut_type_short;
}

const struct ut_type *ut_type_int()
{
	return &__ut_type_int;
}

const struct ut_type *ut_type_long()
{
	return &__ut_type_long;
}

const struct ut_type *ut_type_uchar()
{
	return &__ut_type_uchar;
}

const struct ut_type *ut_type_ushort()
{
	return &__ut_type_ushort;
}

const struct ut_type *ut_type_uint()
{
	return &__ut_type_uint;
}

const struct ut_type *ut_type_ulong()
{
	return &__ut_type_ulong;
}

const struct ut_type *ut_type_float()
{
	return &__ut_type_float;
}

const struct ut_type *ut_type_double()
{
	return &__ut_type_double;
}
