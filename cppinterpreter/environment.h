struct environment_t
{
	atom_t* h;
	environment_t* global;

	environment_t(atom_t* h, environment_t* global) { this->h = h; this->global = global; }
};

environment_t* extend(environment_t* env, atom_t* bindings);
void define(environment_t* env, atom_t* symbol);
atom_t*& lookup(environment_t* env, atom* symbol);
environment_t* GetGlobalEnv(environment_t* env);

environment_t* CreateGlobalEnvironment();