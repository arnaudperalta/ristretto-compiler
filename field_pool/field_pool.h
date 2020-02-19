#ifndef FIELDPOOL__H
#define FIELDPOOL__H

typedef struct field_pool {
    u2 field_count;
    void **pool;
} field_pool;

extern field_pool *field_pool_init(void);

extern int field_pool_entry(field_pool *ptr, u2 name_index, u2 type_index);

// Retourne la taille en octets d'un élément de la field pool
extern size_t field_pool_sizeof(void);

#endif
