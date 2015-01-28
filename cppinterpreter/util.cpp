#include "pch.h"

atom_t* ReverseInPlace(atom_t* list)
{
	// There is a little sublety with how this function works
	// if you have
	// a = <some list>
	// b = ReverseInPlace(a)
	// a will no longer refer to the head of the list, but to the cons element where a is defined,
	// which is now the end of the list!
	// This sideeffect is a little unfortunate

	atom_t* result = nil;
	atom_t* cur = list;

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
	ZASSERT(list!=nil && list!=nullptr)

	if (iscons(list))
	{
		uint32_t length = 0;

		auto nextElement = iter(list);

		while (nextElement())
			length++;

		return length;
	}
	else
	{
		return 0;
	}
}
