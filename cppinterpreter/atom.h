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
			atom_t*(*func)(atom_t *);
		};
	};
};

extern atom_t *nil;
#define no(atom) ((atom) == nil)

#define number(atom) ((atom)->number)
atom_t *new_number(const double value);
//atom_t *coerce_numer(atom_t *value);

#define string(atom) ((atom)->string)
atom_t *new_string(const char *value);
//atom_t *coerce_string(atom_t *value);

//extern atom_t *symbol_table;

#define symbol(atom) ((atom)->symbol)
atom_t *new_symbol(const char *value);
atom_t *coerce_symbol(atom_t *value);
atom_t *intern(const char *symbol);

#define car(atom) ((atom)->car)
#define cdr(atom) ((atom)->cdr)
atom_t *new_cons(atom_t *car, atom_t *cdr);
atom_t *coerce_cons(atom_t *value);
#define cons(car, cdr) new_cons((car), (cdr))

#define args(atom) ((atom)->args)
#define body(atom) ((atom)->body)
#define env(atom) ((atom)->env)
atom_t *new_lambda(atom_t *args, atom_t *body, atom_t *env);
atom_t *new_macro(atom_t *args, atom_t *body, atom_t *env);

/*#define tag(atom) ((atom)->tag)
#define rep(atom) ((atom)->rep)
atom_t *new_tagged(atom_t *tag, atom_t *rep);

#define table(atom) ((atom)->table)
atom_t *new_table(atom_t *entries);
atom_t *coerce_table(atom_t *value);
*/

#define func(atom) ((atom)->func)
atom_t *new_builtin(atom_t *(*func)(atom_t *));
