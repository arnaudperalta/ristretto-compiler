#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack_manager.h"

// Initialise dans une nouvelles structure méthode, elle sera rempli grace aux autres structures
void create_function(char *type, char *name, char *params) {
    char *locals = strdup(params);
    locals_count = 1;
    u2 method_ref;
    if (strcmp(name, "main") == 0) {
        type = strdup("([Ljava/lang/String;)V");
        method_ref = constant_pool_method_entry(cc->cp, name, type, &func_name_index, &func_type_index);
        to_build = method_create(cc->mp, type, name, "", method_ref);
    } else {
        char *par = malloc(strlen(params)); // On établie le type d'entrée de la fonction
        if (par == NULL) {
            perror("Erreur malloc");
            return;
        }
        par[0] = '\0';
        char *member = strtok(params, ",");
        while (member != NULL) {
            strncat(par, member, (int) (strchr(member, ';') - member));
            member = strtok(NULL, ",");
        }
        char *ret = strdup(type);
        type = malloc(strlen(params));
        if (type == NULL) {
            perror("Erreur malloc");
            return;
        }
        sprintf(type, "(%s)%s", par, ret);
        method_ref = constant_pool_method_entry(cc->cp, name, type, &func_name_index, &func_type_index);
        to_build = method_create(cc->mp, type, name, locals, method_ref);
    }
}

// La variable prendra la valeur présente sur le sommet de la pile
void add_variable_to_func(char *type, char *name) {
    // On ajoute la variable dans la liste des locals de la méthode
    int index = method_add_local(to_build, type, name);
    if (index < 0) {
        perror("Erreur add local");
        return;
    }
    // On ajoute dans la variable locale la valeur au sommet de la pile
    if (strcmp(type, "I") == 0) {
        // istore
        method_instruction(to_build, 0x36);
    } else if (strcmp(type, "F") == 0) {
        // fstore
        method_instruction(to_build, 0x38);
    }
    //index
    method_instruction(to_build, (u1) index);
}

void modify_local_to_func(char *name) {
    char *type = malloc(5);
    int index = method_search_local(to_build, name, type);
    if (index < 0) {
        perror("Erreur add local");
        return;
    }
    // On ajoute dans la variable locale la valeur au sommet de la pile
    if (strcmp(type, "I") == 0) {
        // istore
        method_instruction(to_build, 0x36);
    } else if (strcmp(type, "F") == 0) {
        // fstore
        method_instruction(to_build, 0x38);
    }
    //index
    method_instruction(to_build, (u1) index);
}

void modify_global_to_func(u2 index) {
    method_instruction(to_build, 0xb3);
    method_instruction(to_build, index >> 8);
    method_instruction(to_build, index);
}

// On vérifie le type du retour, on renvoie un erreur si ce n'est pas compatible
// avec la déclaration de la fonction
void add_return_to_func(char *type) {
    if (strcmp(type, "I") == 0) {
        // ireturn
        method_instruction(to_build, 0xac);
    } else if (strcmp(type, "F") == 0) {
        //freturn
        method_instruction(to_build, 0xae);
    } else if (strcmp(type, "V") == 0) {
        // return
        method_instruction(to_build, 0xb1);
    }
}

// Charge en bas de la pile la classe utile pour print, on doit le faire
// avant de charger en haut de la pile la valeur a afficher
void init_print_to_func(void) {
    // getstatic : Chargement de la classe System.out
    method_instruction(to_build, 0xb2);
    method_instruction(to_build, 0x00);
    method_instruction(to_build, 0x0b);
}

// Ajoute une instruction print a la fonction en cours d'édition
void add_print_to_func(char *type, char *name) {
    // création de la méthode dans le  constante pool avec le bon type
    char method_type[strlen(type) + 5];
    sprintf(method_type, "(%s)V", type);
    u2 index = constant_pool_print_entry(cc->cp, method_type, name);
    // Execution du print
    method_instruction(to_build, 0xb6);
    method_instruction(to_build, index >> 8);
    method_instruction(to_build, index);
}

// Si value est null, on stack la valeur du type par défaut
void stack_value_to_func(char *type, void *value) {
    if (strcmp(type, "Z") == 0) {
        int b;
        if (value == NULL) {
            b = 0;
        } else {
            b = *((int *) value);
        }
        if (b == 1) {
            method_instruction(to_build, 0x04);
        } else {
            method_instruction(to_build, 0x03);
        } 
    } else if (strcmp(type, "F") == 0 || strcmp(type, "I") == 0) {
        if (value == NULL) {
            int i = 0;
            value = &i;
        }
        u2 index = constant_pool_value_entry(cc->cp, type, value);
        // ldc dans le cas ou la variable est un int ou un float
        method_instruction(to_build, 0x13);
        // push de la constante en haut du stack
        method_instruction(to_build, index >> 8);
        method_instruction(to_build, index);
    }
}

void stack_operator_to_func(char *operator, char *type) {
    if (strcmp(operator, "+") == 0) {
        if (strcmp(type, "I") == 0) {
            //iadd 
            method_instruction(to_build, 0x60);
        } else if (strcmp(type, "F") == 0) {
            //fadd 
            method_instruction(to_build, 0x62);
        }
    } else if (strcmp(operator, "-") == 0) {
        if (strcmp(type, "I") == 0) {
            //isub
            method_instruction(to_build, 0x64);
        } else if (strcmp(type, "F") == 0) {
            //fsub
            method_instruction(to_build, 0x66);
        }
    } else if (strcmp(operator, "*") == 0) {
        if (strcmp(type, "I") == 0) {
            //isub
            method_instruction(to_build, 0x68);
        } else if (strcmp(type, "F") == 0) {
            //fsub
            method_instruction(to_build, 0x6a);
        }
    } else if (strcmp(operator, "/") == 0) {
        if (strcmp(type, "I") == 0) {
            //isub
            method_instruction(to_build, 0x6c);
        } else if (strcmp(type, "F") == 0) {
            //fsub
            method_instruction(to_build, 0x6e);
        }
    }
}

// Met une variable local en haut de la pile et renvoye le type de cette variable
char* stack_local_to_func(char *name) {
    char *type = malloc(5);
    if (type == NULL) {
        perror("Erreur malloc");
        return NULL;
    }
    int index = method_search_local(to_build, name, type);
    if (index < 0) {
        perror("Bad local index");
        return NULL;
    }
    if (strcmp(type, "I") == 0) {
        // iload
        method_instruction(to_build, 0x15);
    } else if (strcmp(type, "F") == 0) {
        // fload
        method_instruction(to_build, 0x17);
    }
    method_instruction(to_build, (u1) index);
    return type;
}

void stack_global_to_func(u2 index) {
    method_instruction(to_build, 0xb2);
    method_instruction(to_build, index >> 8);
    method_instruction(to_build, index);
}

// On ne prend que le nom de la fonction car on aura stacké les parametres avant
// de stacker l'appel
void stack_funcall_to_func(char *name) {
    u2 index = method_pool_get_index(cc->mp, name);
    if (index == 0) {
        perror("Cant stack function");
    }
    //invokestatic
    method_instruction(to_build, 0xb8);
    method_instruction(to_build, index >> 8);
    method_instruction(to_build, index);
}

void close_function(void) {
    method_pool_entry(cc->mp, 0x0009, func_name_index, func_type_index, method_locals_count(to_build), 
            method_render(to_build), method_length(to_build));
    to_build = clinit;
}