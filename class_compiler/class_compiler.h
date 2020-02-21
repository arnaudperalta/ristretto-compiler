#ifndef CLASSCOMPIL__H
#define CLASSCOMPIL__H

/* ---------- Alias de type --------- */

typedef unsigned char u1;
typedef unsigned short u2;
typedef unsigned int u4;

#include "constant_pool.h"

typedef struct class_compiler class_compiler;

/* ---------- Fonctions --------- */

extern class_compiler *class_compiler_init(char *name);

extern int class_compiler_add_field(class_compiler *cc, char *name, char *type);

// Print dans un fichier <nom>.class le contenu correspondant à 
// la structure donnée en paramètre (données et son nom).
extern int class_compiler_print(class_compiler *ptr);

extern void constant_pool_fwrite(constant_pool *ptr, int index, FILE *f);

extern void add_instr_constructor(class_compiler *cc, u1 instr);

extern void close_method_pool(class_compiler *cc);

#endif
