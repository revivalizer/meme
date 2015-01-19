#include "pch.h"

int main(int argc,  char** argv)
{
	argv; argc;

#ifndef STANDALONE
	RunUnitTests();
#else
	auto r = Deserialize(Unpack(nullptr));
	return int(Eval(r, CreateGlobalEnvironment()));
#endif
}