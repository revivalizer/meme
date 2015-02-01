#include "pch.h"
#include "unittest.h"

int main(int argc,  char** argv)
{
	argv; argc;

#ifndef STANDALONE
	init_interpreter();
	RunUnitTests();
#else
	init_interpreter();
	auto r = Deserialize(Unpack(nullptr));
	auto env = CreateGlobalEnvironment();
	auto v = int(Eval(r, env));
	gc_mark(env);
	gc_sweep();
	return v;
#endif
}