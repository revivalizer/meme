struct environment_t
{
	atom_t*        h; // list of frames
	environment_t* global; // pointer to global env. this is needed by some functions who evaluate args in global env.

	environment_t(atom_t* h, environment_t* global) { this->h = h; this->global = global; }
};

environment_t* extend(environment_t* env, atom_t* bindings);
void           define(environment_t* env, atom_t* symbol, atom_t* expr);
atom_t*&       lookup(environment_t* env, atom_t* symbol);
environment_t* GetGlobalEnv(environment_t* env);

environment_t* CreateGlobalEnvironment();