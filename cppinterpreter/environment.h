typedef atom_t environment_t;

environment_t* extend(environment_t* env, atom_t* bindings);
atom_t*& lookup(environment_t* env, atom* symbol);
environment_t* GetRootEnv(environment_t* env);

environment_t* CreateGlobalEnvironment();