#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "class_compiler.h"
#include "field_pool.h"

#define POOL_SIZE 512

typedef struct global {
    char *name;
    char *type;
    u2 ref_index;
} global;

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
    global **list;
};

field_pool *field_pool_init(void) {
    field_pool *ptr = malloc(sizeof(field_pool));
    if (ptr == NULL) {
        perror("Erreur malloc");
        return NULL;
    }
    ptr->field_count = 0;
    ptr->pool = malloc(sizeof(Field_info *) * POOL_SIZE);
    ptr->list = malloc(sizeof (global *) * POOL_SIZE); 
    return ptr;
}

// Créé une variable global associé a un field
global *new_global(char *type, char *name, u2 ref_index) {
    global *ptr = malloc(sizeof(global));
    if (ptr == NULL) {
        return NULL;
    }
    ptr->name = name;
    ptr->type = type;
    ptr->ref_index = ref_index;
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

int field_pool_entry(field_pool *ptr, char *type, char *name, u2 name_index, u2 type_index, u2 ref_index) {
    if (ptr->field_count == POOL_SIZE) {
        perror("Max field pool entries.");
        return -1;
    }
    ptr->pool[ptr->field_count] = new_field(name_index, type_index);
    ptr->list[ptr->field_count] = new_global(type, name, ref_index);
    ptr->field_count++;
    return 0;
}

u2 field_pool_count(field_pool *ptr) {
    return ptr->field_count;
}

// Rercher une variable globale, si elle existe on renvoie son index dans la constante pool
u2 field_pool_search(field_pool *ptr, char *name, char *type) {
    for (int i = 0; i < ptr->field_count; i++) {
        if (strcmp(ptr->list[i]->name, name) == 0) {
            strcpy(type, ptr->list[i]->type);
            return ptr->list[i]->ref_index;
        }
    }
    return 0;
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
