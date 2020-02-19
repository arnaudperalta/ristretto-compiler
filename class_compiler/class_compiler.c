#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "class_compiler.h"
#include "constant_pool.h"
#include "field_pool.h"

u4 magic = 0xCAFEBABE;
u2 mineur = htons(0x0000);
u2 majeur = htons(0x0034);
u2 access_flags = htons(0x0009); // 0x0001 |= 0x0008
u2 interface_count = htons(0x0000);

struct class_compiler {
    char *class_name;
    constant_pool *cp;
    field_pool *fp;
};

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

int class_compiler_print(class_compiler *ptr) {
    char filename[strlen(ptr->class_name) + strlen(".class") + 1];
    sprintf(filename, "%s.class", ptr->class_name);
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        perror("Erreur ouverture du fichier.");
        return -1;
    }
    // Nombre magique
    fwrite(&magic, sizeof(magic), 1, f);
    // Versions
    fwrite((void *) &mineur, sizeof(mineur), 1, f);
    fwrite((void *) &majeur, sizeof(majeur), 1, f);
    // Nombre d'entrée de la constant pool
    //fwrite(&ptr->cp->entry_count + 1, sizeof(u2), 1, f);
    u2 zero = 0;
    fwrite(&zero, sizeof(u2), 1, f);
    // Constant-pool
    /*for (int i = 0; i < ptr->cp->entry_count; i++) {
        fwrite(ptr->cp->pool[i], get_sizeof_entry(ptr->cp->pool[i]), 1, f);
    }*/
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
    // Nombre de champs
    fwrite(&ptr->fp->field_count, sizeof(u2), 1, f);
    // Field pool
    /*for (int i = 0; i < ptr->fp->field_count; i++) {
        fwrite(&ptr->fp->pool[i], field_pool_sizeof(), 1, f);
    }*/
    // Nombre de méthodes
    u2 method_count = 0;
    fwrite(&method_count, sizeof(u2), 1, f);
    // Methodes pool
    // Nombre d'attributs
    u2 attribut_count = 0;
    fwrite(&attribut_count, sizeof(u2), 1, f);

    fclose(f);
    return 0;
}
