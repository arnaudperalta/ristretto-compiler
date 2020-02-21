#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "class_compiler.h"
#include "method_pool.h"

#define POOL_SIZE 512
#define BYTECODE_LENGTH 2048

typedef struct Method_info {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    // on met directement la structure Code_attribute car c'est le seul attributs
    u2 attribute_name_index;
    u4 attribute_length;
    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    u1 *code;
    u2 exception_table_length;
    u2 code_attributes_count;
} Method_info;

struct method {
    u1 *bcode;
    int length;
};

struct method_pool {
    u2 method_count;
    method *clinit;
    Method_info **pool;
};

// Créé un structure method qui acceuille du byte code
method *method_create(void) {
    method *new = malloc(sizeof (method));
    if (new == NULL) {
        perror("Erreur malloc");
        return NULL;
    }
    new->bcode = malloc(sizeof(u1) * BYTECODE_LENGTH);
    if (new->bcode == NULL) {
        perror("Erreur malloc");
        return NULL;
    }
    new->length = 0;
    return new;
}

// Ajoute une insctruction byte code codé sur un octet a la méthode
void method_instruction(method *ptr, u1 instr) {
    memcpy(&ptr->bcode[ptr->length], &instr, 1);
    ptr->length += 1;
}

u1 *method_render(method *ptr) {
    return ptr->bcode;
}

int method_length(method *ptr) {
    return ptr->length;
}

method_pool *method_pool_init(void) {
    method_pool *ptr = malloc(sizeof(method_pool));
    if (ptr == NULL) {
        perror("Erreur malloc");
        return NULL;
    }
    ptr->method_count = 0;
    ptr->pool = malloc(sizeof(Method_info *) * POOL_SIZE);

    // Initialisation méthode instanciation(init)
    method *ini = method_create();
    // 0: aload_0
    method_instruction(ini, 0x2a);
    // 1: invokespecial 0x0001
    method_instruction(ini, 0xb7);
    method_instruction(ini, 0x00);
    method_instruction(ini, 0x01);
    // 4: return
    method_instruction(ini, 0xb1);
    if (method_pool_entry(ptr, 0x0001, 5, 6, 1, method_render(ini), method_length(ini)) != 0) {
        perror("Methode ini");
        return NULL;
    }
    // Initialisation constructeur (clinit), on ne la met pas dans la pool
    ptr->clinit = method_create();
    return ptr;
}

void method_pool_add_clinit(method_pool *ptr, u1 instr) {
    method_instruction(ptr->clinit, instr);
}

Method_info *new_method(u2 access, u2 name_index, u2 type_index, u2 locals_count, u1 *code, int length) {
    Method_info *ptr = malloc(sizeof(Method_info));
    if (ptr == NULL) {
        return NULL;
    }
    ptr->access_flags = access;
    ptr->name_index = name_index;
    ptr->descriptor_index = type_index;
    ptr->attributes_count = 1;
    ptr->attribute_name_index = constant_pool_code();
    ptr->attribute_length = 
        sizeof(u2) * 4 + sizeof(u4) + length;
    ptr->max_stack = 50;
    ptr->max_locals = locals_count;
    ptr->code_length = length;
    ptr->code = code;
    ptr->exception_table_length = 0;
    ptr->code_attributes_count = 0;
    return ptr;
}

int method_pool_entry(method_pool *ptr, u2 access, u2 name_index, u2 type_index, u2 locals_count, u1 *code, int length) {
    if (ptr->method_count == POOL_SIZE) {
        perror("Max field pool entries.");
        return -1;
    }
    ptr->pool[ptr->method_count] = new_method(access, name_index, type_index, locals_count, code, length);
    ptr->method_count++;
    return 0;
}

u2 method_pool_count(method_pool *ptr) {
    return ptr->method_count;
}

void method_pool_fwrite(method_pool *ptr, size_t index, FILE *f) {
    if (index >= ptr->method_count) {
        perror("Invalid index");
        return;
    }
    Method_info *meth = ptr->pool[index];
    u2 access_flags = htons(meth->access_flags);
    fwrite(&access_flags, sizeof(u2), 1, f);
    u2 name_index = htons(meth->name_index);
    fwrite(&name_index, sizeof(u2), 1, f);
    u2 descriptor_index = htons(meth->descriptor_index);
    fwrite(&descriptor_index, sizeof(u2), 1, f);
    u2 attributes_count = htons(meth->attributes_count);
    fwrite(&attributes_count, sizeof(u2), 1, f);
    u2 attribute_name_index = htons(meth->attribute_name_index);
    fwrite(&attribute_name_index, sizeof(u2), 1, f);
    u4 attribute_length = htonl(meth->attribute_length);
    fwrite(&attribute_length, sizeof(u4), 1, f);
    u2 max_stack = htons(meth->max_stack);
    fwrite(&max_stack, sizeof(u2), 1, f);
    u2 max_locals = htons(meth->max_locals);
    fwrite(&max_locals, sizeof(u2), 1, f);
    u4 code_length = htonl(meth->code_length);
    fwrite(&code_length, sizeof(u4), 1, f);
    fwrite(meth->code, meth->code_length, 1, f);
    u2 exception_table_length = htons(meth->exception_table_length);
    fwrite(&exception_table_length, sizeof(u2), 1, f);
    u2 code_attributes_count = htons(meth->code_attributes_count);
    fwrite(&code_attributes_count, sizeof(u2), 1, f);
}

// Terminaison de la method pool en 
void method_pool_end(method_pool *ptr) {
    method_instruction(ptr->clinit, 0xb1);
    if (method_pool_entry(ptr, 0x0001, 10, 6, 1, method_render(ptr->clinit), method_length(ptr->clinit)) != 0) {
        perror("Methode ini");
    }
}
