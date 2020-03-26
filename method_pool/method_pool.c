#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "class_compiler.h"
#include "method_pool.h"

#define POOL_SIZE 512
#define BYTECODE_LENGTH 2048
#define MAX_LOCALS 50

// TODO: Récuperer l'index du bytecode lorsqu' on écrit un if
// puis l'orsqu'on atteint la fin du if, on édite ce morceau de byte cod

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

typedef struct local {
    char *name;
    char *type;
} local;

struct method {
    char *name;
    char *type;
    u2 ref_index;
    local **locals;
    u2 locals_count;
    u1 *bcode;
    int length;
};

struct method_pool {
    u2 method_count;
    method *clinit;
    Method_info **pool;
    int list_count;
    method **list; // on stockera les method dans une list pour ensuite les rechercher
    // lorsqu'on les appellera dans le code, dans ce nous renveirrons leur type de retour
};

// Créé un structure method qui acceuille du byte code
method *method_create(method_pool *ptr, char *type, char *name, char *params, u2 method_ref) {
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
    new->locals = malloc(sizeof(local *) * MAX_LOCALS);
    if (new->locals == NULL) {
        return NULL;
    }
    new->length = 0;
    new->locals_count = 0;
    new->name = name;
    new->type = type;
    new->ref_index = method_ref;
    char *save; // point de sauvegarde pour strtok_r
    char *str = strtok_r(params, ",", &save);
    while (str != NULL) {
        char *type = strtok(str, ";");
        char *name = strtok(NULL, ";");
        method_add_local(new, type, name);
        str = strtok_r(NULL, ",", &save);
    }
    method_pool_add_method(ptr, new);
    return new;
}

// On enregistre une variable locale dans la structure methode
int method_add_local(method *ptr, char *type, char *name) {
    local *l = malloc(sizeof(local));
    if (l == NULL) {
        return -1;
    }
    l->type = strdup(type);
    l->name = strdup(name);
    ptr->locals[ptr->locals_count] = l;
    ptr->locals_count++;
    return ptr->locals_count - 1;
}

// Ajoute une insctruction byte code codé sur un octet a la méthode
void method_instruction(method *ptr, u1 instr) {
    memcpy(&ptr->bcode[ptr->length], &instr, 1);
    ptr->length += 1;
}

void method_instruction_modify(method *ptr, u2 index, u1 instr) {
    ptr->bcode[index] = instr;
}

u1 *method_render(method *ptr) {
    return ptr->bcode;
}

int method_length(method *ptr) {
    return ptr->length;
}

char *method_type(method *ptr) {
    return ptr->type;
}

u2 method_locals_count(method *ptr) {
    return ptr->locals_count;
}

// Retourne l'indice de la variable locale recherchée, si rien trouvé on renvoie -1
int method_search_local(method *ptr, char *name, char *type) {
    for (int i = 0; i < ptr->locals_count; ++i) {
        if (strcmp(ptr->locals[i]->name, name) == 0) {
            strcpy(type, ptr->locals[i]->type);
            return i;
        }
    }
    return -1;
}

method_pool *method_pool_init(void) {
    method_pool *ptr = malloc(sizeof(method_pool));
    if (ptr == NULL) {
        perror("Erreur malloc");
        return NULL;
    }
    ptr->method_count = 0;
    ptr->pool = malloc(sizeof(Method_info *) * POOL_SIZE);
    ptr->list_count = 0;
    ptr->list = malloc(sizeof(method *) * POOL_SIZE);

    // Initialisation méthode instanciation(init)
    method *ini = method_create(ptr, "()V", "init", "", 1);
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
    ptr->clinit = method_create(ptr, "()V", "clinit", "", 1337);
    return ptr;
}

method *method_pool_get_clinit(method_pool *ptr) {
    return ptr->clinit;
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
    ptr->max_locals = locals_count + 10; // par sécurité
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

void method_pool_add_method(method_pool *ptr, method *new) {
    ptr->list[ptr->list_count] = new;
    ptr->list_count++;
}

// cherche un nom de méthode, si elle existe on renvoie son type
char *method_pool_search(method_pool *ptr, char *name) {
    for (int i = 0; i < ptr->list_count; i++) {
        if (strcmp(name, ptr->list[i]->name) == 0) {
            return ptr->list[i]->type;
        }
    }
    return NULL;
}

// Retourne l'index de son équivalent dans la constante pool
u2 method_pool_get_index(method_pool *ptr, char *name) {
    for (int i = 0; i < ptr->list_count; i++) {
        if (strcmp(name, ptr->list[i]->name) == 0) {
            return ptr->list[i]->ref_index;
        }
    }
    return 0;
}

// Terminaison de la method clinit en ajoutant un return
void method_pool_end(method_pool *ptr) {
    method_instruction(ptr->clinit, 0xb1);
    if (method_pool_entry(ptr, 0x0009, 10, 6, 1, method_render(ptr->clinit), method_length(ptr->clinit)) != 0) {
        perror("Methode ini");
    }
}
