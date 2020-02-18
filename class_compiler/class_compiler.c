#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "class_compiler.h"
#include "constant_pool.h"
#include "field_pool.h"

u4 magic = 0xCAFEBABE;
u2 mineur = 0x0000;
u2 majeur = 0x0034;
u2 access_flags = 0x0009; // 0x0001 |= 0x0008
u2 interface_count = 0x0000;

struct class_compiler {
    constant_pool *cp;
    field_pool *fp;
};

class_compiler *class_compiler_init(char *name) {
    class_compiler *ptr = malloc(sizeof(class_compiler));
    if (ptr == NULL) {
        perror("erreur malloc");
        return NULL;
    }
    ptr->cp = constant_pool_init(name);
    ptr->fp = field_pool_init();
    return ptr;
}

int class_compiler_add_field(class_compiler *cc, char *name, char *type) {
    u2 name_index;
    u2 type_index;
    if (constant_pool_field_entry(cc->cp, name, type
        , &name_index, &type_index) != 0) {
        perror("Erreur field_entry constant pool");
        return -1;
    }
    if (field_pool_entry(cc->fp, name_index, type_index) != 0) {
        perror("Erreur field_entry field pool");
        return -1;
    }
    return 0;
}

int class_compiler_print(constant_pool *ptr) {
    FILE *f = fopen(ptr->class_name, "w");
    if (f == NULL) {
        perror("Erreur ouverture du fichier.");
        return -1;
    }
    // Nombre magique
    fwrite(&magic, sizeof(magic), 1, f);
    // Versions
    fwrite(&mineur, sizeof(mineur), 1, f);
    fwrite(&majeur, sizeof(majeur), 1, f);
    // Nombre d'entrée de la constant pool
    fwrite(&ptr->entry_count + 1, sizeof(u2), 1, f);
    // Constant-pool
    for (int i = 0; i < ptr->entry_count; i++) {
        fwrite(ptr->pool[i], get_sizeof_entry(ptr->pool[i]), 1, f);
    }
    // Flags d'accès
    fwrite(&access_flags, sizeof(access_flags), 1, f);
    // Index de la classe actuelle
    u2 this = constant_pool_this();
    fwrite(&this, sizeof(u2), 1, f);
    // Index de la super-classe
    u2 super = constant_pool_super();
    fwrite(&super, sizeof(u2), 1, f);
    // Nombres d'interface
    fwrite(&interface_count, sizeof(u2), 1, f);

    fclose(f);
    return 0;
}
