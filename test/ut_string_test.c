#include "ut_string.h"
#include <stdio.h>

static void test1()
{
	struct ut_string *s = ut_string_new("Hello");
	puts(s->ptr);
	ut_string_append(s, ", World!");
	puts(s->ptr);
	ut_string_delete(s);
}

int main()
{
	test1();
	return 0;
}
