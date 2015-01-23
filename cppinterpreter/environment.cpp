#include "pch.h"

environment_t* extend(environment_t* env, atom_t* bindings)
{
	//return cons(bindings, env);
	return new environment_t(cons(bindings, env->h), env->global);
}

void define(environment_t* env, atom_t* symbol)
{
	// Adds new nil definition to first frame
	car(env->h) = cons(cons(symbol, nil), car(env->h));
}

atom_t*& lookup(environment_t* env, atom_t* s)
{
	// assuming symbol for now
	auto str = symbol(s);

	auto nextFrame = iter(env->h);

	while (auto frame = nextFrame())
	{
		auto nextBinding = iter(frame);

		while (auto binding = nextBinding())
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

	auto globalEnv = new environment_t(cons(env, nil), nullptr);
	globalEnv->global = globalEnv;
	return globalEnv;	
}

environment_t* GetGlobalEnv(environment_t* env)
{
	return env->global;
}
