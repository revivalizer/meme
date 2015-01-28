#include "pch.h"

environment_t* extend(environment_t* env, atom_t* bindings)
{
	//return cons(bindings, env);
	return new environment_t(cons(bindings, env->h), env->global);
}

void define(environment_t* env, atom_t* symbol, atom_t* expr)
{
	// Adds new nil definition to first frame
	car(env->h) = cons(cons(symbol, expr), car(env->h));
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
	auto globalEnv = new environment_t(cons(nil, nil), nullptr);
	globalEnv->global = globalEnv;

	define(globalEnv, new_symbol("-"),    new_builtin(BuiltinSub));
	define(globalEnv, new_symbol("*"),    new_builtin(BuiltinMul));
	define(globalEnv, new_symbol("cons"), new_builtin(BuiltinCons));
	define(globalEnv, new_symbol("car"),  new_builtin(BuiltinCar));
	define(globalEnv, new_symbol("cdr"),  new_builtin(BuiltinCdr));
	define(globalEnv, new_symbol("list"), new_builtin(BuiltinList));
	define(globalEnv, new_symbol("nil"),  get_nil());

	return globalEnv;	
}

environment_t* GetGlobalEnv(environment_t* env)
{
	return env->global;
}
