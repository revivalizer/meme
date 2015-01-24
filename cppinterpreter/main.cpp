#include "pch.h"

int main(int argc,  char** argv)
{
	argv; argc;

#ifndef STANDALONE
	RunUnitTests();
	exec_alloc();
	exec_emit(0xC3);
	exec_run();
#else
	auto r = Deserialize(Unpack(nullptr));
	auto env = CreateGlobalEnvironment();
	auto v = int(Eval(r, env));
	gc_mark(env);
	gc_sweep();
	return v;
#endif
}