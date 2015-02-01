#include "pch.h"

atom_t *nil;

atom_t *new_atom(const atom_type_t type) {
	atom_t *atom = new atom_t;

	atom->type   = type;
	atom->markId = 0;

	gc_track(atom);

	return atom;
}

atom_t *new_boolean(const bool value) {
	atom_t *atom = new_atom(ATOM_BOOLEAN);
	boolean(atom) = value;
	return atom;
}

atom_t *new_number(const double value) {
	atom_t *atom = new_atom(ATOM_NUMBER);
	number(atom) = value;
	return atom;
}

atom_t *new_string(const char *value) {
	atom_t *atom = new_atom(ATOM_STRING);
	string(atom) = zstrdup(value);
	return atom;
}

atom_t *new_symbol(const char *value) {
	atom_t *atom = new_atom(ATOM_SYMBOL);
	symbol(atom) = zstrdup(value);
	return atom;
}

atom_t *new_cons(atom_t *car_, atom_t *cdr_) {
	atom_t *atom = new_atom(ATOM_CONS);
	car(atom) = car_;
	cdr(atom) = cdr_;
	return atom;
}

atom_t *new_lambda(atom_t *parameters_, atom_t *body_, environment_t *env_) {
	atom_t *atom = new_atom(ATOM_LAMBDA);
	parameters(atom)  = parameters_;
	body(atom)        = body_;
	environment(atom) = env_;
	return atom;
}

atom_t *new_macro(atom_t *parameters_, atom_t *body_, environment_t *env_) {
	atom_t *atom = new_atom(ATOM_MACRO);
	parameters(atom)  = parameters_;
	body(atom)        = body_;
	environment(atom) = env_;
	return atom;
}

atom_t *new_builtin(atom_t *(*func_)(atom_t *)) {
	atom_t *atom = new_atom(ATOM_BUILTIN);
	func(atom) = func_;
	return atom;
}
