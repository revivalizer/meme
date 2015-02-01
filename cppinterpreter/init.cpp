#include "pch.h"

void init_interpreter()
{
	nil = new atom_t;
	type(nil) = ATOM_NIL;
}