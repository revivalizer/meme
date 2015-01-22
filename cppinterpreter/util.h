#pragma once

#include "atom.h"

struct iter
{
	iter(atom_t* list) { this->cur = list; }

	atom_t* cur;

	atom_t* operator() ()
	{
		if (cur==nil) return nil;

		atom_t* c = cur;
		cur = cdr(cur);
		return car(c);
	}
};

atom_t* ReverseInPlace(atom_t* list);

uint32_t ListLength(atom_t* list);