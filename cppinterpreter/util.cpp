#include "pch.h"

atom* ReverseInPlace(atom* list)
{
	// There is a little sublety with how this function works
	// if you have
	// a = <some list>
	// b = ReverseInPlace(a)
	// a will no longer refer to the head of the list, but to the cons element where a is defined,
	// which is now the end of the list!
	// This sideeffect is a little unfortunate
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
