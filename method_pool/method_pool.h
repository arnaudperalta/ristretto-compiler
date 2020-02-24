#ifndef METHODPOOL__H
#define METHODPOOL__H

/* ---------- Structures --------- */

typedef struct method method;
typedef struct method_pool method_pool;

/* ---- Fonctions method pool ---- */

extern method_pool *method_pool_init(void);
extern int method_pool_entry(method_pool *ptr, u2 access, u2 name_index, 
    u2 type_index, u2 locals_count, u1 *code, int length);
extern void method_pool_add_clinit(method_pool *ptr, u1 instr);
// Retourne la taille en octets d'un élément de la field pool
extern size_t method_pool_sizeof(void);
extern u2 method_pool_count(method_pool *ptr);
extern void method_pool_fwrite(method_pool *ptr, size_t index, FILE *f);

/* ---- Fonctions method ---- */

extern method *method_create(char *params);
extern void method_instruction(method *ptr, u1 instr);
extern u1 *method_render(method *ptr);
extern int method_length(method *ptr);
extern int method_add_local(method *ptr, char *type, char *name);
extern u2 method_locals_count(method *ptr);
// On stocke dans type le type trouvé
extern int method_search_local(method *ptr, char *name, char *type);
extern void method_pool_end(method_pool *ptr);

#endif
