void gc_track(atom_t* atom);
void gc_mark(atom_t* atom);
void gc_mark(environment_t* env);
void gc_sweep();
void gc_stats();