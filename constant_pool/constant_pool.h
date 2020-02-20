#ifndef CONSTPOOL__H
#define CONSTPOOL__H

typedef struct constant_pool constant_pool;

#include "class_compiler.h"

/* ---------- Fonctions --------- */

/*  Etat de la pool après la fonction
*   1. Methodref_info : (2, 3);
*   2. Class_info : 4;
*   3. NameAndType_info : (5, 6);
*   4. Utf8_info : "java/lang/Object";
*   5. Utf8_info : "<init>";
*   6. Utf8_info : "()V";
*   7. Class_info : 8;
*   8. Utf8_info : "class_name";
*/
extern constant_pool *constant_pool_init(char *name);

/* Etat de la pool relatif après ajout d'une méthode `int fact(int i);`
*   x + 1. Methodref_info : (7, x + 2);
*   x + 2. NameAndType_info : (x + 3, x + 4);
*   x + 3. Utf8_info : ”fact” ;
*   x + 4. Utf8_info : ”(I)I” ;
*/
/* Etat de la pool relatif après ajout d'un champ `int a;`
*   x + 1. Fieldref_info : (7, x + 2);
*   x + 2. NameAndType_info : (x + 3, x + 4);
*   x + 3. Utf8_info : "a" ;
*   x + 4. Utf8_info : "I" ;
*/
extern int constant_pool_method_entry(constant_pool *ptr, char *name, char *type);
extern int constant_pool_field_entry(constant_pool *ptr, char *name, char *type
        , u2 *name_index, u2 *type_index);

// Renvoie le nombre d'entrée de la structure,
// ce qui équivaut au numero de la dernière ligne écrite
extern u2 constant_pool_count(constant_pool *ptr);

extern u2 constant_pool_this(void);
extern u2 constant_pool_super(void);

extern size_t get_sizeof_entry(void *entry);

#endif
