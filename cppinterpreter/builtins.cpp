#include "pch.h"

atom_t* BuiltinSub(atom_t* args)
{
	if (args==nil)
		return new_number(0.0);

	ZASSERT(isnumber(car(args)));

	if (cdr(args)==nil)
	{
		return new_number(-number(car(args)));
	}

	double res = number(car(args));
	auto argiter = iter(cdr(args));

	while (auto arg = argiter())
	{
		ZASSERT(isnumber(arg))
		res -= number(arg);
	}

	return new_number(res);

}

atom_t* BuiltinMul(atom_t* args)
{
	double res = 1.0;

	auto argiter = iter(args);

	while (auto arg = argiter())
	{
		ZASSERT(isnumber(arg))
		res *= number(arg);
	}

	return new_number(res);
}
