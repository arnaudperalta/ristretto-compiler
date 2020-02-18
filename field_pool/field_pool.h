#ifndef FIELDPOOL__H
#define FIELDPOOL__H

typedef struct field_pool field_pool;

extern field_pool *field_pool_init(void);

extern int field_pool_entry(field_pool *ptr, u2 name_index, u2 type_index);

#endif
