#include "pch.h"

atom_t* Apply(atom_t* fn, atom_t* args);

bool IsTrue(atom_t* expr)
{
	if (isboolean(expr))
		return boolean(expr);
	else if (isnumber(expr))
		return number(expr)!=0.0;
	else if (isstring(expr))
		return !zstrequal(string(expr), "");
	else if (iscons(expr))
		return car(expr)!=nil;
	else
		return true;
}

atom_t* Unquote(atom_t* expr, environment_t* env)
{
	if (iscons(expr))
	{
		auto h = car(expr);
		auto t = cdr(expr);

		if (issymbol(h) && zstrequal(symbol(h), "unquote"))
		{
			ZASSERT(!iscons(t) || cdr(t)==nil) // cannot have more than one argument to unquote
			return Eval(car(t), env);
		}
		else
		{
			auto result = cons(h, nil);

			auto tailiter = iter(t);
			
			while (auto e = tailiter())
			{
				result = cons(Unquote(e, env), result);
			}

			return ReverseInPlace(result);
		}
	}

	return expr;
}

atom_t* EvalArgList(atom_t* args, environment_t* env);

atom_t* Eval(atom_t* expr, environment_t* env)
{
	if (isnumber(expr)) 
		return expr;
	else if (isstring(expr))
		return expr;
	else if (issymbol(expr))
		return lookup(env, expr);
	else if (iscons(expr) && car(expr)==nil)
		return expr; // empty list evaluates to itself
	else if (iscons(expr))
	{
		atom_t* fn = car(expr);
		atom_t* args = cdr(expr);
		
		// special forms
		if (issymbol(fn)) 
		{
			if (zstrequal(symbol(fn), "if"))
			{
				ZASSERT(ListLength(args)==3)

				auto cond = car(args);
				auto then = car(cdr(args));
				auto else_ = car(cdr(cdr(args)));

				if (IsTrue(Eval(cond, env)))
					return Eval(then, env);
				else
					return Eval(else_, env);
			}
			else if (zstrequal(symbol(fn), "let"))
			{
				ZASSERT(ListLength(args)==2)

				auto bindings = car(args);
				auto body = car(cdr(args));

				ZASSERT(iscons(bindings))
				ZASSERT(body!=nil)

				auto evalbindings = nil;
				auto bindingiter = iter(bindings);

				while (auto binding = bindingiter())
				{
					evalbindings = cons(cons(car(binding), Eval(car(cdr(binding)), env)), evalbindings);
				}

				return Eval(body, extend(env, evalbindings));
			}
			else if (zstrequal(symbol(fn), "letrec"))
			{
				ZASSERT(ListLength(args)==2)

				auto bindings = car(args);
				auto body = car(cdr(args));

				ZASSERT(iscons(bindings))
				ZASSERT(body!=nil)

				// generate environment with dummy bindings
				auto dummybindings = nil;
				auto bindingiter = iter(bindings);

				while (auto binding = bindingiter())
				{
					dummybindings = cons(cons(car(binding), nil), dummybindings);
				}

				dummybindings = ReverseInPlace(dummybindings);

				auto dummyenvironment = extend(env, dummybindings);

				// evaluate bindings in dummy environment, and update dummy environment
//				auto evalbindings = nil;
				bindingiter = iter(bindings);
				auto dummybindingiter = iter(dummybindings);

				while (auto binding = bindingiter())
				{
					auto dummybinding = dummybindingiter();
					cdr(dummybinding) = Eval(car(cdr(binding)), dummyenvironment);
					//evalbindings = cons(cons(car(binding), Eval(car(cdr(binding)), env)), evalbindings);
				}

				return Eval(body, dummyenvironment);
			}
			else if (zstrequal(symbol(fn), "let*"))
			{
				ZASSERT(ListLength(args)==2)

				auto bindings = car(args);
				auto body = car(cdr(args));

				ZASSERT(iscons(bindings))
				ZASSERT(body!=nil)

				auto evalbindings = nil;
				auto bindingiter = iter(bindings);

				auto extendedenv = env;

				while (auto binding = bindingiter())
				{
					evalbindings = cons(cons(car(binding), Eval(car(cdr(binding)), extendedenv)), evalbindings);
					extendedenv  = extend(env, evalbindings);
				}

				return Eval(body, extendedenv);
			}
			else if (zstrequal(symbol(fn), "quote"))
			{
				ZASSERT(cdr(args)==nil)
				return Unquote(car(args), env);
			}
			else if (zstrequal(symbol(fn), "eval"))
			{
				ZASSERT(cdr(args)==nil)
				auto unquotedExpr = Eval(car(args), env);
				return Eval(unquotedExpr, GetGlobalEnv(env));
			}
			else if (zstrequal(symbol(fn), "lambda"))
			{
				ZASSERT(ListLength(args)==2)

				auto parameters = car(args);
				auto body = car(cdr(args));

				return new_lambda(parameters, body, env);
			}
			else if (zstrequal(symbol(fn), "macro"))
			{
				ZASSERT(ListLength(args)==2)

				auto parameters = car(args);
				auto body = car(cdr(args));

				return new_macro(parameters, body, env);
			}
			else if (zstrequal(symbol(fn), "set!"))
			{
				ZASSERT(ListLength(args)==2)
				ZASSERT(issymbol(car(args)));

				lookup(env, car(args)) = Eval(car(cdr(args)), env);
				return nil;
			}
			else if (zstrequal(symbol(fn), "define"))
			{
				ZASSERT(ListLength(args)==2)
				ZASSERT(issymbol(car(args)));

				define(env, car(args));
				lookup(env, car(args)) = Eval(car(cdr(args)), env);
				return nil;
			}
			else if (zstrequal(symbol(fn), "begin"))
			{
				auto argiter = iter(args);

				auto last = nil;

				while (auto arg = argiter())
				{
					last = Eval(arg, env);
				}

				return last;
			}
		}

		// Eval fn
		fn = Eval(fn, env);

		if (ismacro(fn))
		{
			ZASSERT(ListLength(args)==ListLength(parameters(fn)))

			auto unevaluatedBindings = nil;
			auto parameteriter = iter(parameters(fn));
			auto argiter = iter(args);

			while (auto parameter = parameteriter())
			{
				auto arg = argiter();
				unevaluatedBindings = cons(cons(parameter, arg), unevaluatedBindings);
			}

			environment_t* env   = environment(fn); // creating environment
			environment_t* env_  = env;             // calling environment
			environment_t* env__ = extend(env, unevaluatedBindings); // creating environment extended with unevaluated bound arguments
			return Eval(Eval(body(fn), env__), env_); // body is evaluated in the environment where it was defined, extended with UNEVALUATED arguments, then evaluated in calling environment

		}
		else
		{
			return Apply(fn, EvalArgList(args, env));
		}
	}

	return nil;
}

atom_t* EvalArgList(atom_t* args, environment_t* env)
{
	// Eval args
	auto argiter = iter(args);
	auto evalargs = nil;

	while (auto arg = argiter())
	{
		evalargs = cons(Eval(arg, env), evalargs);
	}

	return ReverseInPlace(evalargs);
}

atom_t* Apply(atom_t* fn, atom_t* args)
{
	ZASSERT(isbuiltin(fn) || islambda(fn));
	if (isbuiltin(fn))
	{
		return (func(fn))(args);
	}
	else if (islambda(fn))
	{
		ZASSERT(ListLength(args)==ListLength(parameters(fn)))

		auto evalbindings = nil;
		auto parameteriter = iter(parameters(fn));
		auto argiter = iter(args);

		while (auto parameter = parameteriter())
		{
			auto arg = argiter();
			evalbindings = cons(cons(parameter, arg), evalbindings);
		}

		return Eval(body(fn), extend(environment(fn), ReverseInPlace(evalbindings))); // body is evaluated in the environment where it was defined, extended with arguments evaluated in the calling environment
	}

	return nil;
}

bool StructuralEquality(atom* expr1, atom* expr2)
{
	if (expr1==nil && expr2==nil)
		return true;

	if (expr1==nil || expr2==nil)
		return false;

	if (type(expr1)!=type(expr2))
		return false;

	if (isnumber(expr1) && number(expr1)==number(expr2))
		return true;

	if (isstring(expr1) && zstrequal(string(expr1), string(expr2)))
		return true;

	if (issymbol(expr1) && zstrequal(symbol(expr1), symbol(expr2)))
		return true;

	if (iscons(expr1))
	{
		if (!StructuralEquality(car(expr1), car(expr2)))
			return false;

		return StructuralEquality(cdr(expr1), cdr(expr2));
	}

	return false;
}
