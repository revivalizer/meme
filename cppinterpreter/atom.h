#pragma once

typedef struct atom atom_t;

typedef enum {
	ATOM_BOOLEAN,
	ATOM_NUMBER,
	ATOM_STRING,
	ATOM_SYMBOL,
	ATOM_CONS,
	ATOM_LAMBDA,
	ATOM_MACRO,
	ATOM_BUILTIN,
} atom_type_t;

typedef atom_t*(*builtin_func)(atom_t *);

struct atom {
	atom_type_t type;
	union {
		bool boolean;
		double number;
		char *string;
		char *symbol;
		struct { // cons
			atom_t *car, *cdr;
		};
		struct { // lambda & macro
			atom_t *parameters, *body, *env;
		};
		//atom_t *table;
		struct { // builtin
			builtin_func func;
		};
	};
};

typedef struct atom atom_t;

extern atom_t *nil;
ZINLINE bool no(atom* atom) { return atom == nil; }

ZINLINE bool& boolean(atom* atom) { return atom->boolean; }
atom_t* new_boolean(const bool boolean);

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

ZINLINE atom*& parameters(atom* atom)         { return atom->parameters; }
ZINLINE atom*& body(atom* atom)               { return atom->body; }
ZINLINE atom*& environment(atom* atom)        { return atom->env; }
atom_t *new_lambda(atom_t *parameters, atom_t *body, atom_t *env);
atom_t *new_macro(atom_t *parameters, atom_t *body, atom_t *env);

/*#define tag(atom) ((atom)->tag)
#define rep(atom) ((atom)->rep)
atom_t *new_tagged(atom_t *tag, atom_t *rep);

#define table(atom) ((atom)->table)
atom_t *new_table(atom_t *entries);
atom_t *coerce_table(atom_t *value);
*/

ZINLINE builtin_func& func(atom* atom) { return atom->func; }
atom_t *new_builtin(atom_t *(*func)(atom_t *));

// Type functions
ZINLINE atom_type_t& type(atom* atom) { return atom->type; }
ZINLINE bool isa(atom* atom, atom_type_t type_) { return type(atom)==type_; }
ZINLINE bool isboolean(atom* atom) { return isa(atom, ATOM_BOOLEAN); }
ZINLINE bool isnumber (atom* atom) { return isa(atom, ATOM_NUMBER); }
ZINLINE bool isstring (atom* atom) { return isa(atom, ATOM_STRING); }
ZINLINE bool issymbol (atom* atom) { return isa(atom, ATOM_SYMBOL); }
ZINLINE bool iscons   (atom* atom) { return isa(atom, ATOM_CONS); }
ZINLINE bool isbuiltin(atom* atom) { return isa(atom, ATOM_BUILTIN); }
ZINLINE bool islambda (atom* atom) { return isa(atom, ATOM_LAMBDA); }
ZINLINE bool ismacro  (atom* atom) { return isa(atom, ATOM_MACRO); }