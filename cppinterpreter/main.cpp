#include "pch.h"

int main(int argc,  char** argv)
{
	argv; argc;

#ifndef STANDALONE
	RunUnitTests();
	//RunTest(cons(new_string(), new_string()));
#else
	auto r = Deserialize(Unpack(nullptr));
	return int(Eval(r, CreateGlobalEnvironment()));
#endif
}