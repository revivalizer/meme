#include "pch.h"

atom* InPlaceReverse(atom* list)
{
	iter listiter(list);

	atom* cur = nil;

	while (atom* e = listiter())
	{
		cdr(e) = cur;
		cur = e;
	}

	return cur;
}

