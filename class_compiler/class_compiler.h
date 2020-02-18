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

// Renvoie le nombre d'entrée de la structure,
// ce qui équivaut au numero de la dernière ligne écrite
extern u2 constant_pool_count(constant_pool *ptr);

// Print dans un fichier <nom>.class le contenu correspondant à 
// la structure donnée en paramètre (données et son nom).
extern int class_compiler_print(constant_pool *ptr);


#endif
