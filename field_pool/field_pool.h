#ifndef FIELDPOOL__H
#define FIELDPOOL__H

#include <stddef.h>
#include <stdio.h>

typedef struct field_pool field_pool;

extern field_pool *field_pool_init(void);

extern int field_pool_entry(field_pool *ptr, char *type, char *name, u2 name_index, u2 type_index, u2 ref_index);

// Retourne la taille en octets d'un élément de la field pool
extern size_t field_pool_sizeof(void);

extern u2 field_pool_count(field_pool *ptr);

// le type de la variable sera stocké dans le champ type
extern u2 field_pool_search(field_pool *ptr, char *name, char *type);

extern void field_pool_fwrite(field_pool *ptr, size_t index, FILE *f);

#endif
