#include "pch.h"

atom_t* BuiltinSub(atom_t* args)
{
	if (no(args))
		return new_number(0.0);

	ZASSERT(isnumber(car(args)));

	if (no(cdr(args)))
	{
		return new_number(-number(car(args)));
	}

	double res = number(car(args));
	auto nextArg = iter(cdr(args));

	while (auto arg = nextArg())
	{
		ZASSERT(isnumber(arg))
		res -= number(arg);
	}

	return new_number(res);
}

atom_t* BuiltinAdd(atom_t* args)
{
	if (no(args))
		return new_number(0.0);

	double res = 0.0;
	auto nextArg = iter(args);

	while (auto arg = nextArg())
	{
		ZASSERT(isnumber(arg))
		res += number(arg);
	}

	return new_number(res);
}

atom_t* BuiltinMul(atom_t* args)
{
	double res = 1.0;

	auto nextArg = iter(args);

	while (auto arg = nextArg())
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
	if (no(args))
		return nil;

	ZASSERT(iscons(args));
	return car(car(args));
}

atom_t* BuiltinCdr(atom_t* args)
{
	if (no(args))
		return nil;

	ZASSERT(iscons(args));
	return cdr(car(args));
}

atom_t* BuiltinList(atom_t* args)
{
	ZASSERT(iscons(args));
	return args;
}

atom_t* BuiltinType(atom_t* args)
{
	switch (type(car(args)))
	{
		case ATOM_BOOLEAN: return sym_boolean; break;
		case ATOM_NUMBER:  return sym_number; break;
		case ATOM_STRING:  return sym_string; break;
		case ATOM_SYMBOL:  return sym_symbol; break;
		case ATOM_CONS:    return sym_cons; break;
		case ATOM_LAMBDA:  return sym_lambda; break;
		case ATOM_MACRO:   return sym_macro; break;
		case ATOM_BUILTIN: return sym_builtin; break;
		case ATOM_NIL:     return nil; break;
	}

	ZBREAK()
	return nil;
}

atom_t* BuiltinIs(atom_t* args)
{
	ZASSERT(ListLength(args)==2)
	auto a1 = car(args);
	auto a2 = car(cdr(args));

	if (type(a1)==type(a2))
	{
		switch (type(a1))
		{
			case ATOM_NUMBER:  return (number(a1)==number(a2) ? t : nil);
			case ATOM_STRING:  return (zstrequal(string(a1), string(a2)) ? t : nil);
			case ATOM_SYMBOL:  return (zstrequal(string(a1), string(a2)) ? t : nil);
			case ATOM_BUILTIN: return (func(a1)==func(a2)) ? t : nil;
		}
	}

	return nil;
}
