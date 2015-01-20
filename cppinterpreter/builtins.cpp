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

atom_t* BuiltinCons(atom_t* args)
{
	ZASSERT(iscons(args));

	if (cdr(args)!=nil) // this is neccesary because the nil object is a null ptr. This would be unneccesary with a special nil object, which was of nil type.
	{
		ZASSERT(iscons(cdr(args)));
		return new_cons(car(args), car(cdr(args)));
	}
	else
	{
		return new_cons(car(args), nil);
	}
}

atom_t* BuiltinCar(atom_t* args)
{
	ZASSERT(iscons(args));
	return car(car(args));
}

atom_t* BuiltinCdr(atom_t* args)
{
	ZASSERT(iscons(args));
	return cdr(car(args));
}

atom_t* BuiltinList(atom_t* args)
{
	ZASSERT(iscons(args));
	return args;
}
