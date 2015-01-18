#pragma once

#include "atom.h"

struct iter
{
	iter(atom* list) { this->cur = list; }

	atom* cur;

	atom* operator() ()
	{
		if (cur==nil) return nil;

		atom* c = cur;
		cur = cdr(cur);
		return car(c);
	}
};

atom* ReverseInPlace(atom* list);

uint32_t ListLength(atom_t* list);