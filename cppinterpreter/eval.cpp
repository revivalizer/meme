#include "pch.h"

atom* Eval(atom* expr)
{
	if (isnumber(expr)) 
		return expr;
	else if (isstring(expr))
		return expr;

	return nil;
}

bool StructuralEquality(atom* expr1, atom* expr2)
{
	if (type(expr1)!=type(expr2))
		return false;

	if (isnumber(expr1) && number(expr1)==number(expr2))
		return true;

	if (isstring(expr1) && zstrequal(string(expr1), string(expr2)))
		return true;

	return false;
}
