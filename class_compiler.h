#ifndef CLASSCOMPIL__H
#define CLASSCOMPIL__H

#define CONSTANT_Class 7
#define CONSTANT_Fieldref 9
#define CONSTANT_Methodref 10
#define CONSTANT_InterfaceMethodref 11
#define CONSTANT_String	8
#define CONSTANT_Integer 3
#define CONSTANT_Float 4
#define CONSTANT_Long 5
#define CONSTANT_Double	6
#define CONSTANT_NameAndType 12
#define CONSTANT_Utf8 1
#define CONSTANT_MethodHandle 15
#define CONSTANT_MethodType	16
#define CONSTANT_InvokeDynamic 18


/* ---------- Alias de type --------- */

typedef unsigned char u1;
typedef unsigned short u2;
typedef unsigned int u4;

/* ---------- Structures ---------- */

typedef struct constant_pool constant_pool;

struct _Utf8_info {
    u1 tag;
    u2 length;
    u1 *bytes;
} const Utf8_info_default = { CONSTANT_Utf8, 0 };
typedef struct _Utf8_info Utf8_info;

struct _Class_info {
    u1 tag;
    u2 name_index;
} const Class_info_default = { CONSTANT_Class, 0 };
typedef struct _Class_info Class_info;

struct _NameAndType_info {
    u1 tag;
    u2 name_index;
    u2 descriptor_index;
} const NameAndType_info_default = { CONSTANT_NameAndType, 0, 0 };
typedef struct _NameAndType_info NameAndType_info;

struct _Fieldref_info {
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
} const Fieldref_info_default = { CONSTANT_Fieldref, 0, 0 };
typedef struct _Fieldref_info Fieldref_info;

struct _Methodref_info {
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
} const Methodref_info_default = { CONSTANT_Methodref, 0, 0 };
typedef struct _Methodref_info Methodref_info;

struct _String_info {
    u1 tag;
    u2 string_index;
} const String_info_default = { CONSTANT_String, 0 };
typedef struct _String_info String_info;

struct _Integer_info {
    u1 tag;
    u4 bytes;
} const Integer_info_default = { CONSTANT_Integer, 0 };
typedef struct _Integer_info Integer_info;

struct _Float_info {
    u1 tag;
    u4 bytes;
} const Float_info_default = { CONSTANT_Float, 0 };
typedef struct _Float_info Float_info;

/* ---------- Fonctions --------- */

/*  Etat de la pool après la fonction
*   1. Methodref_info : (2, 3);
*   2. Class_info : 4;
*   3. NameAndType_info : (5, 6);
*   4. Utf8_info : "java/lang/Object";
*   5. Utf8_info : "<init>";
*   6. Utf8_info : "()V";
*   7. Class_info : 8;
*   8. Utf8_info : "class_name";
*/
extern constant_pool *constant_pool_init(char *class_name);

/* Etat de la pool relatif après ajout d'une méthode `int fact(int i);`
*   x + 1. Methodref_info : (7, x + 2);
*   x + 2. NameAndType_info : (x + 3, x + 4);
*   x + 3. Utf8_info : ”fact” ;
*   x + 4. Utf8_info : ”(I)I” ;
*/
/* Etat de la pool relatif après ajout d'un champ `int a;`
*   x + 1. Fieldref_info : (7, x + 2);
*   x + 2. NameAndType_info : (x + 3, x + 4);
*   x + 3. Utf8_info : "a" ;
*   x + 4. Utf8_info : "I" ;
*/
extern int constant_pool_entry(constant_pool *ptr, void *entry);
extern int constant_pool_method_entry(constant_pool *ptr, char *name, char *type);
extern int constant_pool_field_entry(constant_pool *ptr, char *name, char *type);

// Renvoie le nombre d'entrée de la structure,
// ce qui équivaut au numero de la dernière ligne écrite
extern u2 constant_pool_count(constant_pool *ptr);

// Affiche l'état de la structure
extern void constant_pool_debug(constant_pool *ptr);

extern NameAndType_info *new_nameandtype(u2 name, u2 desc);
extern Class_info *new_class(u2 l);
extern Utf8_info *new_utf8(char *string);
extern Methodref_info *new_methodref(u2 index, u2 name);

#endif
