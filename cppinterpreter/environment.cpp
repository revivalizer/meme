#include "pch.h"

environment_t* extend(environment_t* env, atom_t* bindings)
{
	return cons(bindings, env);
}

atom_t* lookup(environment_t* env, atom* s)
{
	// assuming symbol for now
	auto str = symbol(s);

	auto extensioniter = iter(env);

	while (auto extension = extensioniter())
	{
		auto bindingiter = iter(extension);

		while (auto binding = bindingiter())
		{
			if (zstrequal(str, symbol(car(binding))))
				return cdr(binding);
		}
	}

	zcriticalerror("No binding for '%s'", str);
	return nil;
}

environment_t* CreateGlobalEnvironment()
{
	atom_t* env = nil;
	env = cons(cons(new_symbol("-"),    new_builtin(BuiltinSub)), env);
	env = cons(cons(new_symbol("*"),    new_builtin(BuiltinMul)), env);
	env = cons(cons(new_symbol("cons"), new_builtin(BuiltinCons)), env);
	env = cons(cons(new_symbol("car"),  new_builtin(BuiltinCar)), env);
	env = cons(cons(new_symbol("cdr"),  new_builtin(BuiltinCdr)), env);
	env = cons(cons(new_symbol("list"), new_builtin(BuiltinList)), env);

	return cons(env, nil);
}