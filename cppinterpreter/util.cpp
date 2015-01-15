#include "pch.h"

atom* ReverseInPlace(atom* list)
{
	iter listiter(list);

	atom* result = nil;
	atom* cur = list;

	while (cur)
	{
		auto next = cdr(cur);
		cdr(cur) = result;
		result = cur;
		cur = next;
	}

	return result;
}

