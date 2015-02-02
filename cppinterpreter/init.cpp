#include "pch.h"

atom_t* make_free_symbol(char* str)
{
	auto s= new atom_t;
	type(s) = ATOM_SYMBOL;
	symbol(s) = zstrdup(str);
	return s;
}

void init_interpreter()
{
	nil = new atom_t;
	type(nil) = ATOM_NIL;

	t = make_free_symbol("t");

	sym_boolean = make_free_symbol("boolean");
	sym_number  = make_free_symbol("number");
	sym_string  = make_free_symbol("string");
	sym_symbol  = make_free_symbol("symbol");
	sym_cons    = make_free_symbol("cons");
	sym_lambda  = make_free_symbol("lambda");
	sym_macro   = make_free_symbol("macro");
	sym_builtin = make_free_symbol("builtin");
}