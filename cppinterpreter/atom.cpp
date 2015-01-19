#include "pch.h"

atom_t *nil = nullptr;

atom_t *new_atom(const atom_type_t type) {
	atom_t *result = new atom_t;
	result->type = type;
	return result;
}

atom_t *new_boolean(const bool value) {
	atom_t *result = new_atom(ATOM_BOOLEAN);
	boolean(result) = value;
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

atom_t *new_cons(atom_t *car_, atom_t *cdr_) {
	atom_t *result = new_atom(ATOM_CONS);
	car(result) = car_;
	cdr(result) = cdr_;
	return result;
}

atom_t *new_lambda(atom_t *parameters_, atom_t *body_, atom_t *env_) {
	atom_t *result = new_atom(ATOM_LAMBDA);
	parameters(result) = parameters_;
	body(result) = body_;
	environment(result) = env_;
	return result;
}

atom_t *new_macro(atom_t *parameters_, atom_t *body_, atom_t *env_) {
	atom_t *result = new_atom(ATOM_MACRO);
	parameters(result) = parameters_;
	body(result) = body_;
	environment(result) = env_;
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

atom_t *new_builtin(atom_t *(*func_)(atom_t *)) {
	atom_t *result = new_atom(ATOM_BUILTIN);
	func(result) = func_;
	return result;
}

atom_t *symbol_table;

atom_t *intern(const char *symbol_) {
	atom_t *i;
	for (i = symbol_table; !no(i); i = cdr(i))
		if (!zstrcmp(symbol_, symbol(car(i))))
			return car(i);
	return car(symbol_table = cons(new_symbol(symbol_), symbol_table));
}