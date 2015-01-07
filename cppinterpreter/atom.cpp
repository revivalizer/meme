#include "libs/base/base.h"
#include "atom.h"

atom_t *nil = nullptr;

atom_t *new_atom(const atom_type_t type) {
	atom_t *result = new atom_t;
	result->type = type;
	return result;
}

atom_t *new_number(const double value) {
	atom_t *result = new_atom(ATOM_NUMBER);
	number(result) = value;
	return result;
}

atom_t *new_string(const char *value) {
	atom_t *result = new_atom(ATOM_STRING);
	string(result) = zstrdup(value);
	return result;
}

atom_t *new_symbol(const char *value) {
	atom_t *result = new_atom(ATOM_SYMBOL);
	symbol(result) = zstrdup(value);
	return result;
}

atom_t *new_cons(atom_t *car, atom_t *cdr) {
	atom_t *result = new_atom(ATOM_CONS);
	car(result) = car;
	cdr(result) = cdr;
	return result;
}

atom_t *new_lambda(atom_t *args, atom_t *body, atom_t *env) {
	atom_t *result = new_atom(ATOM_LAMBDA);
	args(result) = args;
	body(result) = body;
	env(result) = env;
	return result;
}

atom_t *new_macro(atom_t *args, atom_t *body, atom_t *env) {
	atom_t *result = new_atom(ATOM_MACRO);
	args(result) = args;
	body(result) = body;
	env(result) = env;
	return result;
}

/*atom_t *new_tagged(atom_t *tag, atom_t *rep) {
	atom_t *result = new_atom(ATOM_TAGGED);
	tag(result) = tag;
	rep(result) = rep;
	return result;
}
*/

/*atom_t *new_table(atom_t *entries) {
	atom_t *result = new_atom(ATOM_TABLE);
	table(result) = entries;
	return result;
}
*/

atom_t *new_builtin(atom_t *(*func)(atom_t *)) {
	atom_t *result = new_atom(ATOM_BUILTIN);
	func(result) = func;
	return result;
}

atom_t *symbol_table;

atom_t *intern(const char *symbol) {
	atom_t *i;
	for (i = symbol_table; !no(i); i = cdr(i))
		if (!zstrcmp(symbol, symbol(car(i))))
			return car(i);
	return car(symbol_table = cons(new_symbol(symbol), symbol_table));
}