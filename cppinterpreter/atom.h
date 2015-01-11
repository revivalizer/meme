#pragma once

typedef struct atom atom_t;

typedef enum {
	ATOM_NUMBER,
	ATOM_STRING,
	ATOM_SYMBOL,
	ATOM_CONS,
	ATOM_LAMBDA,
	ATOM_MACRO,
	ATOM_BUILTIN,
} atom_type_t;

typedef atom_t*(*func22)(atom_t *);

struct atom {
	atom_type_t type;
	union {
		double number;
		char *string;
		char *symbol;
		struct { // cons
			atom_t *car, *cdr;
		};
		struct { // lambda & macro
			atom_t *args, *body, *env;
		};
		//atom_t *table;
		struct { // builtin
			func22 func;
		};
	};
};

extern atom_t *nil;
ZINLINE bool no(atom* atom) { return atom == nil; }

ZINLINE double& number(atom* atom) { return atom->number; }
atom_t *new_number(const double value);

ZINLINE char*& string(atom* atom) { return atom->string; }
atom_t *new_string(const char *value);

//extern atom_t *symbol_table;

ZINLINE char*& symbol(atom* atom) { return atom->symbol; }
atom_t *new_symbol(const char *value);
atom_t *coerce_symbol(atom_t *value);
atom_t *intern(const char *symbol);

ZINLINE atom*& car(atom* atom) { return atom->car; }
ZINLINE atom*& cdr(atom* atom) { return atom->cdr; }
atom_t *new_cons(atom_t *car, atom_t *cdr);
atom_t *coerce_cons(atom_t *value);
ZINLINE atom* cons(atom* car, atom* cdr) { return new_cons(car, cdr); }

ZINLINE atom*& args(atom* atom) { return atom->args; }
ZINLINE atom*& body(atom* atom) { return atom->body; }
ZINLINE atom*& env(atom* atom) { return atom->env; }
atom_t *new_lambda(atom_t *args, atom_t *body, atom_t *env);
atom_t *new_macro(atom_t *args, atom_t *body, atom_t *env);

/*#define tag(atom) ((atom)->tag)
#define rep(atom) ((atom)->rep)
atom_t *new_tagged(atom_t *tag, atom_t *rep);

#define table(atom) ((atom)->table)
atom_t *new_table(atom_t *entries);
atom_t *coerce_table(atom_t *value);
*/

ZINLINE func22& func(atom* atom) { return atom->func; }
atom_t *new_builtin(atom_t *(*func)(atom_t *));
