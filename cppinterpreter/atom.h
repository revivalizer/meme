#pragma once

typedef struct atom_t atom_t;
struct environment_t;

typedef atom_t*(*builtin_func)(atom_t *);

typedef enum {
	ATOM_BOOLEAN,
	ATOM_NUMBER,
	ATOM_STRING,
	ATOM_SYMBOL,
	ATOM_CONS,
	ATOM_LAMBDA,
	ATOM_MACRO,
	ATOM_BUILTIN,
	ATOM_NIL,
} atom_type_t;

typedef struct atom_t {
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
			atom_t *parameters, *body;
			environment_t* env;
		};
		//atom_t *table;
		struct { // builtin
			builtin_func func;
		};
	};
	uint32_t markId;
} atom_t;

extern atom_t *nil;
ZINLINE bool no(atom_t* atom) { return atom == nullptr || atom == nil; }
atom_t* get_nil();

ZINLINE bool& boolean(atom_t* atom) { return atom->boolean; }
atom_t* new_boolean(const bool boolean);

ZINLINE double& number(atom_t* atom) { return atom->number; }
atom_t *new_number(const double value);

ZINLINE char*& string(atom_t* atom) { return atom->string; }
atom_t *new_string(const char *value);

ZINLINE char*& symbol(atom_t* atom) { return atom->symbol; }
atom_t *new_symbol(const char *value);

ZINLINE atom_t*& car(atom_t* atom) { return atom->car; }
ZINLINE atom_t*& cdr(atom_t* atom) { return atom->cdr; }
atom_t *new_cons(atom_t *car, atom_t *cdr);
ZINLINE atom_t* cons(atom_t* car, atom_t* cdr) { return new_cons(car, cdr); }

ZINLINE atom_t*&        parameters(atom_t* atom)         { return atom->parameters; }
ZINLINE atom_t*&        body(atom_t* atom)               { return atom->body; }
ZINLINE environment_t*& environment(atom_t* atom)        { return atom->env; }
atom_t *new_lambda(atom_t *parameters, atom_t *body, environment_t *env);
atom_t *new_macro(atom_t *parameters, atom_t *body, environment_t *env);

ZINLINE builtin_func& func(atom_t* atom) { return atom->func; }
atom_t *new_builtin(atom_t *(*func)(atom_t *));

// Type functions
ZINLINE atom_type_t& type(atom_t* atom) { return atom->type; }
ZINLINE bool isa(atom_t* atom, atom_type_t type_) { return type(atom)==type_; }
ZINLINE bool isboolean(atom_t* atom) { return isa(atom, ATOM_BOOLEAN); }
ZINLINE bool isnumber (atom_t* atom) { return isa(atom, ATOM_NUMBER); }
ZINLINE bool isstring (atom_t* atom) { return isa(atom, ATOM_STRING); }
ZINLINE bool issymbol (atom_t* atom) { return isa(atom, ATOM_SYMBOL); }
ZINLINE bool iscons   (atom_t* atom) { return isa(atom, ATOM_CONS); }
ZINLINE bool isbuiltin(atom_t* atom) { return isa(atom, ATOM_BUILTIN); }
ZINLINE bool islambda (atom_t* atom) { return isa(atom, ATOM_LAMBDA); }
ZINLINE bool ismacro  (atom_t* atom) { return isa(atom, ATOM_MACRO); }