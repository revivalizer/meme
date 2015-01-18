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

uint32_t ListLength(atom_t* list)
{
	if (iscons(list))
	{
		if (car(list)==nil)
			return 0;

		uint32_t length = 0;

		auto listiter = iter(list);

		while (listiter())
			length++;

		return length;
	}
	else
	{
		return 0;
	}
}
