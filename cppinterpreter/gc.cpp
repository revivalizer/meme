#include "pch.h"

atom_t* allocations[1024*1024];

uint32_t markId = 1;
uint32_t insertIdx = 0;

void gc_track(atom_t* atom)
{
	allocations[insertIdx++] = atom;
}

void gc_mark(environment_t* env)
{
	gc_mark(env->h);
}

void gc_mark(atom_t* atom)
{
	if (atom==nil)
		return;

	if (atom->markId==markId)
		return;

	atom->markId = markId;

	if (iscons(atom))
	{
		gc_mark(car(atom));
		gc_mark(cdr(atom));
	}

	if (islambda(atom) && ismacro(atom))
	{
		gc_mark(parameters(atom));
		gc_mark(body(atom));
		gc_mark(environment(atom));
	}
}

void gc_sweep()
{
	uint32_t r = 0;
	uint32_t w = 0;

	while (r < insertIdx)
	{
		atom_t* atom = allocations[r];

		if (atom->markId < markId)
		{
			if (issymbol(atom))
				delete symbol(atom);
			if (isstring(atom))
				delete string(atom);

			delete atom;
		}
		else
		{
			allocations[w++] = atom;
		}

		r++;
	}

	insertIdx = w;

	markId++;
}

void gc_stats()
{
	zmsg("Sweeps: %d", (markId-1));
	zmsg("Live atoms: %d", insertIdx);
}
