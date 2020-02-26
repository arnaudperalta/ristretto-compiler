#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "class_compiler.h"
#include "constant_pool.h"
#include "field_pool.h"
#include "method_pool.h"

u4 magic = htonl(0xCAFEBABE);
u2 mineur = htons(0x0000);
u2 majeur = htons(0x0034);
u2 access_flags = htons(0x0009); // 0x0001 |= 0x0008
u2 interface_count = htons(0x0000);

void write_in_class(FILE *f, void *arg, size_t size);

class_compiler *class_compiler_init(char *name) {
    class_compiler *ptr = malloc(sizeof(class_compiler));
    if (ptr == NULL) {
        perror("Erreur malloc");
        return NULL;
    }
    ptr->class_name = malloc(strlen(name) + 1);
    if (ptr->class_name == NULL) {
        perror("Erreur malloc");
        return NULL;
    }
    strcpy(ptr->class_name, name);
    ptr->cp = constant_pool_init(name);
    ptr->fp = field_pool_init();
    ptr->mp = method_pool_init();
    return ptr;
}

int class_compiler_add_field(class_compiler *cc, char *name, char *type, void *data, 
        u2 *ref_index, u2 *data_index) {
    u2 name_index;
    u2 type_index;
    *ref_index = constant_pool_field_entry(cc->cp, name, type, data, &name_index, &type_index, data_index);
    if (*ref_index < 0) {
        perror("Erreur field_entry constant pool");
        return -1;
    }
    if (field_pool_entry(cc->fp, type, name, name_index, type_index, *ref_index) != 0) {
        perror("Erreur field_entry field pool");
        return -1;
    }
    return *ref_index;
}

int class_compiler_print(class_compiler *ptr) {
    char filename[strlen(ptr->class_name) + strlen(".class") + 1];
    sprintf(filename, "%s.class", ptr->class_name);
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        perror("Erreur ouverture du fichier.");
        return -1;
    }
    // Nombre magique
    write_in_class(f, &magic, sizeof(magic));
    // Versions
    write_in_class(f, &mineur, sizeof(mineur));
    write_in_class(f, &majeur, sizeof(majeur));
    // Nombre d'entrée de la constant pool
    u2 entry_count = htons(constant_pool_count(ptr->cp) + 1);
    write_in_class(f, &entry_count, sizeof(u2));
    // Constant-pool
    for (int i = 0; i < constant_pool_count(ptr->cp); i++) {
        constant_pool_fwrite(ptr->cp, i, f);
    }
    // Flags d'accès
    write_in_class(f, &access_flags, sizeof(access_flags));
    // Index de la classe actuelle
    u2 this = htons(constant_pool_this());
    write_in_class(f, &this, sizeof(u2));
    // Index de la super-classe
    u2 super = htons(constant_pool_super());
    write_in_class(f, &super, sizeof(u2));
    // Nombres d'interface
    write_in_class(f, &interface_count, sizeof(u2));
    // Nombre de champs
    u2 field_count = htons(field_pool_count(ptr->fp));
    write_in_class(f, &field_count, sizeof(u2));
    // Field pool
    for (int i = 0; i < field_pool_count(ptr->fp); i++) {
        field_pool_fwrite(ptr->fp, i, f);
    }
    // Nombre de méthodes
    u2 method_count = htons(method_pool_count(ptr->mp));
    write_in_class(f, &method_count, sizeof(u2));
    // Methodes pool
    for (int i = 0; i < method_pool_count(ptr->mp); i++) {
        method_pool_fwrite(ptr->mp, i, f);
    }
    // Nombre d'attributs
    u2 attribut_count = 0;
    write_in_class(f, &attribut_count, sizeof(u2));

    fclose(f);
    return 0;
}

void write_in_class(FILE *f, void *arg, size_t size) {
    fwrite(arg, size, 1, f);
}

void close_method_pool(class_compiler *cc) {
    method_pool_end(cc->mp);
}
