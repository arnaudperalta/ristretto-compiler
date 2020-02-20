#include <stdlib.h>
#include <stdio.h>
#include "class_compiler.h"
#include "field_pool.h"

#define POOL_SIZE 512

struct Field_info {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
} const Field_info_default = { 0x0009, 0, 0, 0 };
typedef struct Field_info Field_info;

struct field_pool {
    u2 field_count;
    Field_info **pool;
};

field_pool *field_pool_init(void) {
    field_pool *ptr = malloc(sizeof(field_pool));
    if (ptr == NULL) {
        perror("Erreur malloc");
        return NULL;
    }
    ptr->field_count = 0;
    ptr->pool = malloc(sizeof(Field_info *) * POOL_SIZE);
    return ptr;
}

Field_info *new_field(u2 name_index, u2 type_index) {
    Field_info *ptr = malloc(sizeof (Field_info));
    if (ptr == NULL) {
        return NULL;
    }
    *ptr = Field_info_default;
    ptr->name_index = name_index;
    ptr->descriptor_index = type_index;
    return ptr;
}

int field_pool_entry(field_pool *ptr, u2 name_index, u2 type_index) {
    if (ptr->field_count == POOL_SIZE) {
        perror("Max field pool entries.");
        return -1;
    }
    ptr->pool[ptr->field_count] = new_field(name_index, type_index);
    ptr->field_count++;
    return 0;
}

u2 field_pool_count(field_pool *ptr) {
    return ptr->field_count;
}

void field_pool_fwrite(field_pool *ptr, size_t index, FILE *f) {
    if (index >= field_pool_count(ptr)) {
        perror("Invalid index");
        return;
    }
    Field_info *field = ptr->pool[index];
    u2 access_flags = htons(field->access_flags);
    fwrite(&access_flags, sizeof(u2), 1, f);
    u2 name_index = htons(field->name_index);
    fwrite(&name_index, sizeof(u2), 1, f);
    u2 descriptor_index = htons(field->descriptor_index);
    fwrite(&descriptor_index, sizeof(u2), 1, f);
    u2 attributes_count = htons(field->attributes_count);
    fwrite(&attributes_count, sizeof(u2), 1, f);
}
