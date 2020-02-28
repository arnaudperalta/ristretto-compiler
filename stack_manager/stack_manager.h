#ifndef STACKMANA__H
#define STACKMANA__H

#include "class_compiler.h"

// On push dans la pile l'index de bytecode a modifié lorsqu'on atteindra
// la fin d'un if afin d'écrire le offset a sauté si le if est valide
typedef struct if_stack if_stack;

/* Données pour la construction de fonction */
method *to_build;
method *clinit; // Constructeur
u2 locals_count;
u2 func_name_index;
u2 func_type_index;
class_compiler *cc;
if_stack *is;

extern void create_function(char *type, char *name, char *params);
extern void add_variable_to_func(char *type, char *name);
extern void modify_local_to_func(char *name);
extern void modify_global_to_func(u2 index);
extern void init_condition(if_stack *ptr);
extern void finish_condition(if_stack *ptr);
extern void init_print_to_func(void);
extern void add_print_to_func(char *type, char *name);
extern void stack_value_to_func(char *type, void *value);
extern void stack_operator_to_func(char *operator, char *type);
extern char *stack_local_to_func(char *name);
extern void stack_funcall_to_func(char *name);
extern void stack_global_to_func(u2 index);
extern void add_return_to_func(char *type);
extern void close_function(void);

#endif
