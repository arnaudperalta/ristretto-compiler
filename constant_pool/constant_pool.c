#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "constant_pool.h"
#include "class_compiler.h"

#define POOL_SIZE 512
#define CONSTANT_Class 7
#define CONSTANT_Fieldref 9
#define CONSTANT_Methodref 10
#define CONSTANT_String	8
#define CONSTANT_Integer 3
#define CONSTANT_Float 4
#define CONSTANT_NameAndType 12
#define CONSTANT_Utf8 1

u2 index_this = 7;
u2 index_super = 2;
u2 index_code = 9;

struct constant_pool {
    unsigned short entry_count;
    void **pool;
};

struct Utf8_info {
    u1 tag;
    u2 length;
    u1 *bytes;
} const Utf8_info_default = { CONSTANT_Utf8, 0 };
typedef struct Utf8_info Utf8_info;

struct Class_info {
    u1 tag;
    u2 name_index;
} const Class_info_default = { CONSTANT_Class, 0 };
typedef struct Class_info Class_info;

struct NameAndType_info {
    u1 tag;
    u2 name_index;
    u2 descriptor_index;
} const NameAndType_info_default = { CONSTANT_NameAndType, 0, 0 };
typedef struct NameAndType_info NameAndType_info;

struct Fieldref_info {
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
} const Fieldref_info_default = { CONSTANT_Fieldref, 0, 0 };
typedef struct Fieldref_info Fieldref_info;

struct Methodref_info {
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
} const Methodref_info_default = { CONSTANT_Methodref, 0, 0 };
typedef struct Methodref_info Methodref_info;

struct String_info {
    u1 tag;
    u2 string_index;
} const String_info_default = { CONSTANT_String, 0 };
typedef struct String_info String_info;

struct Integer_info {
    u1 tag;
    u4 bytes;
} const Integer_info_default = { CONSTANT_Integer, 0 };
typedef struct Integer_info Integer_info;

struct Float_info {
    u1 tag;
    u4 bytes;
} const Float_info_default = { CONSTANT_Float, 0 };
typedef struct Float_info Float_info;

int constant_pool_entry(constant_pool *ptr, void *entry);
NameAndType_info *new_nameandtype(u2 name, u2 desc);
Class_info *new_class(u2 l);
Utf8_info *new_utf8(char *string);
Methodref_info *new_methodref(u2 index, u2 name);
Fieldref_info *new_fieldref(u2 index, u2 name);
Integer_info *new_integer(u4 bytes);
Float_info *new_float(u4 bytes);
String_info *new_string(u2 string_index);

constant_pool *constant_pool_init(char *name) {
    constant_pool *ptr = malloc(sizeof (constant_pool));
    if (ptr == NULL) {
        perror("Erreur malloc");
        return NULL;
    }
    ptr->entry_count = 0;
    ptr->pool = malloc((sizeof (void *)) * POOL_SIZE);
    if (ptr->pool == NULL)
        return NULL;

    // 1. Methodref_info : (2, 3);
    if (constant_pool_entry(ptr, new_methodref(index_super, 3)) < 0) {
        perror("entry cons erreur");
        return NULL;
    }
    // 2. Class_info : 4;
    if (constant_pool_entry(ptr, new_class(4)) < 0) {
        perror("entry ci erreur");
        return NULL;
    }
    // 3. NameAndType_info : (5, 6);
    if (constant_pool_entry(ptr, new_nameandtype(5, 6)) < 0) {
        perror("entry nat erreur");
        return NULL;
    }
    // 4. Utf8_info : "java/lang/Object";
    if (constant_pool_entry(ptr, new_utf8("java/lang/Object")) < 0) {
        perror("entry class erreur");
        return NULL;
    }
    // 5. Utf8_info : "<init>";
    if (constant_pool_entry(ptr, new_utf8("<init>")) < 0) {
        perror("entry class erreur");
        return NULL;
    }
    // 6. Utf8_info : "()V";
    if (constant_pool_entry(ptr, new_utf8("()V")) < 0) {
        perror("entry class erreur");
        return NULL;
    }
    // 7. Class_info : 8;
    if (constant_pool_entry(ptr, new_class(8)) < 0) {
        perror("entry ci erreur");
        return NULL;
    }
    //8. Utf8_info : "class_name";
    if (constant_pool_entry(ptr, new_utf8(name)) < 0) {
        perror("entry class erreur");
        return NULL;
    }
    //9. Utf8_info : "Code";
    if (constant_pool_entry(ptr, new_utf8("Code")) < 0) {
        perror("entry class erreur");
        return NULL;
    }
    //10. Utf8_info : "<clinit>";
    if (constant_pool_entry(ptr, new_utf8("<clinit>")) < 0) {
        perror("entry class erreur");
        return NULL;
    }
    //11. Fieldref_info : (12, 14);
    if (constant_pool_entry(ptr, new_fieldref(12, 14)) < 0) {
        perror("entry class erreur");
        return NULL;
    }
    //12. Class_info : 13;
    if (constant_pool_entry(ptr, new_class(13)) < 0) {
        perror("entry class erreur");
        return NULL;
    }
    //13. Utf8 : "java/lang/System"
    if (constant_pool_entry(ptr, new_utf8("java/lang/System")) < 0) {
        perror("entry class erreur");
        return NULL;
    }
    //14. NameAndType : (15, 16);
    if (constant_pool_entry(ptr, new_nameandtype(15, 16)) < 0) {
        perror("entry nat erreur");
        return NULL;
    }
    //15. Utf8 : "out"
    if (constant_pool_entry(ptr, new_utf8("out")) < 0) {
        perror("entry class erreur");
        return NULL;
    }
    //16. Utf8 : "Ljava/io/PrintStream;"
    if (constant_pool_entry(ptr, new_utf8("Ljava/io/PrintStream;")) < 0) {
        perror("entry utf8 erreur");
        return NULL;
    }

    return ptr;
}

int constant_pool_entry(constant_pool *ptr, void *entry) {
    if (ptr->entry_count == POOL_SIZE) {
        perror("Max constant pool entries.");
        return -1;
    }
    ptr->pool[ptr->entry_count] = entry;
    ptr->entry_count++;
    return ptr->entry_count; // Renvoie l'index dans la constant pool
}

// Renvoie methodref
int constant_pool_method_entry(constant_pool *ptr, char *name, char *type,
        u2 *name_index, u2 *type_index) {
    u2 line_count = constant_pool_count(ptr);
    // x + 1. Methodref_info : (7, 2);
    u2 methodref = constant_pool_entry(ptr, new_methodref(index_this, line_count + 2));
    if (methodref < 0) {
        perror("entry method erreur");
        return -1;
    }
    // x + 2. NameAndType_info : (x + 3, x + 4);
    if (constant_pool_entry(ptr, new_nameandtype(line_count + 3, line_count + 4)) < 0) {
        perror("entry nameandtype erreur");
        return -1;
    }
    // x + 3. Utf8_info : ”fact” ;
    *name_index = constant_pool_entry(ptr, new_utf8(name));
    if (*name_index < 0) {
        perror("entry utf8 erreur");
        return -1;
    }
    // x + 4. Utf8_info : ”(I)I” ;
    *type_index = constant_pool_entry(ptr, new_utf8(type));
    if (*type_index < 0) {
        perror("entry utf8 erreur");
        return -1;
    }
    return methodref;
}

int constant_pool_field_entry(constant_pool *ptr, char *name, char *type, void *data
        , u2 *name_index, u2 *type_index, u2 *data_index) {
    u2 line_count = constant_pool_count(ptr);
    // x + 1. Methodref_info : (7, 2);
    int fieldref = constant_pool_entry(ptr, new_fieldref(index_this, line_count + 2));
    if (fieldref < 0) {
        perror("entry field erreur");
        return -1;
    }
    // x + 2. NameAndType_info : (x + 3, x + 4);
    if (constant_pool_entry(ptr, new_nameandtype(line_count + 3, line_count + 4)) < 0) {
        perror("entry nameandtype erreur");
        return -1;
    }
    // x + 3. Utf8_info : "a" ;
    *name_index = constant_pool_entry(ptr, new_utf8(name));
    if (*name_index < 0) {
        perror("entry utf8 erreur");
        return -1;
    }
    // x + 4. Utf8_info : "I" ;
    *type_index = constant_pool_entry(ptr, new_utf8(type));
    if (*type_index < 0) {
        perror("entry utf8 erreur");
        return -1;
    }
    // x + 5  <Type>_info : <data>;
    if (strcmp(type, "I") == 0) {
        u4 integer = (data == NULL) ? 0 : *((u4 *) data);
        *data_index = constant_pool_entry(ptr, new_integer(integer));
    } else if (strcmp(type, "F") == 0) {
        u4 fl = (data == NULL) ? 0.0 : *((u4 *) data);
        *data_index = constant_pool_entry(ptr, new_float(fl));
    } else if (strcmp(type, "Z") == 0) {
        return fieldref;
    }
    if (*data_index < 0) {
        perror("entry integer erreur");
        return -1;
    }

    return fieldref;
}

// Fonction utilisée pour entrer des constantes pour l'execution de
// l'instruction print et println
u2 constant_pool_print_entry(constant_pool *ptr, char *type, char *name) {
    u2 line_count = constant_pool_count(ptr);
    // x + 1. Methodref_info : (x + 2, x + 4);
    int methodref = constant_pool_entry(ptr, new_methodref(line_count + 2, line_count + 4));
    if (methodref < 0) {
        perror("entry field erreur");
        return -1;
    }
    // x + 2. Class_info : (x + 3);
    if (constant_pool_entry(ptr, new_class(line_count + 3)) < 0) {
        perror("entry class erreur");
        return -1;
    }
    // x + 3. Utf8 : "java/io/PrintStream";
    if (constant_pool_entry(ptr, new_utf8("java/io/PrintStream")) < 0) {
        perror("entry utf8 erreur");
        return -1;
    }
    // x + 4. NameAndType_info (x + 5, x + 6)
    if (constant_pool_entry(ptr, new_nameandtype(line_count + 5, line_count + 6)) < 0) {
        perror("entry utf8 erreur");
        return -1;
    }
    // x + 5. Utf8_info (<name>)
    if (constant_pool_entry(ptr, new_utf8(name)) < 0) {
        perror("entry utf8 erreur");
        return -1;
    }
    // x + 6. Utf8_info (<type)
    if (constant_pool_entry(ptr, new_utf8(type)) < 0) {
        perror("entry utf8 erreur");
        return -1;
    }
    return methodref;
}

// Rentre une valeur dans la constante pool et retourne l'index
u2 constant_pool_value_entry(constant_pool *ptr, char *type, void *data) {
    if (strcmp(type, "I") == 0) {
        u4 integer = (data == NULL) ? 0 : *((u4 *) data);
        return constant_pool_entry(ptr, new_integer(integer));
    } else if (strcmp(type, "F") == 0) {
        u4 fl = (data == NULL) ? 0.0 : *((u4 *) data);
        return constant_pool_entry(ptr, new_float(fl));
    } else if (strcmp(type, "Ljava/lang/String;") == 0) {
        char *str = (data == NULL) ? "" : (char *) data;
        u2 line = constant_pool_entry(ptr, new_utf8(str));
        if (line < 0) {
            perror("entry utf8 erreur");
            return -1;
        }
        return constant_pool_entry(ptr, new_string(line));
    }
    return 0;
}

u2 constant_pool_count(constant_pool *ptr) {
    return ptr->entry_count;
}

u2 constant_pool_this(void) {
    return index_this;
}

u2 constant_pool_super(void) {
    return index_super;
}

u2 constant_pool_code(void) {
    return index_code;
}

NameAndType_info *new_nameandtype(u2 name, u2 desc) {
    NameAndType_info *ptr = malloc(sizeof (NameAndType_info));
    if (ptr == NULL)
        return NULL;
    *ptr = NameAndType_info_default;
    ptr->name_index = name;
    ptr->descriptor_index = desc;
    return ptr;
}

Class_info *new_class(u2 l) {
    Class_info *ptr = malloc(sizeof (Class_info));
    if (ptr == NULL)
        return NULL;
    *ptr = Class_info_default;
    ptr->name_index = l;
    return ptr;
}

Utf8_info *new_utf8(char *string) {
    Utf8_info *ptr = malloc(sizeof (Utf8_info));
    if (ptr == NULL)
        return NULL;
    *ptr = Utf8_info_default;
    ptr->length = strlen(string);
    ptr->bytes = malloc(sizeof (char) * ptr->length);
    if (ptr->bytes == NULL)
        return NULL;
    memcpy(ptr->bytes, string, ptr->length);
    return ptr;
}

Methodref_info *new_methodref(u2 index, u2 name) {
    Methodref_info *ptr = malloc(sizeof (Methodref_info));
    if (ptr == NULL)
        return NULL; 
    *ptr = Methodref_info_default;
    ptr->class_index = index;
    ptr->name_and_type_index = name;
    return ptr;
}

Fieldref_info *new_fieldref(u2 index, u2 name) {
    Fieldref_info *ptr = malloc(sizeof (Fieldref_info));
    if (ptr == NULL) {
        return NULL;
    }
    *ptr = Fieldref_info_default;
    ptr->class_index = index;
    ptr->name_and_type_index = name;
    return ptr;
}

Integer_info *new_integer(u4 bytes) {
    Integer_info *ptr = malloc(sizeof (Integer_info));
    if (ptr == NULL) {
        return NULL;
    }
    *ptr = Integer_info_default;
    ptr->bytes = bytes;
    return ptr;
}

Float_info *new_float(u4 bytes) {
    Float_info *ptr = malloc(sizeof (Float_info));
    if (ptr == NULL) {
        return NULL;
    }
    *ptr = Float_info_default;
    ptr->bytes = bytes;
    return ptr;
}

String_info *new_string(u2 string_index) {
    String_info *ptr = malloc(sizeof (String_info));
    if (ptr == NULL) {
        return NULL;
    }
    *ptr = String_info_default;
    ptr->string_index = string_index;
    return ptr;
}

size_t get_sizeof_entry(void *entry) {
    u1 tag = *((u1 *) entry);
    switch(tag) {
        case CONSTANT_Class: return sizeof(Class_info);
        case CONSTANT_Fieldref: return sizeof(Fieldref_info);
        case CONSTANT_Float: return sizeof(Float_info);
        case CONSTANT_Integer: return sizeof(Integer_info);
        case CONSTANT_NameAndType: return sizeof(NameAndType_info);
        case CONSTANT_Utf8: return sizeof(Utf8_info);
        case CONSTANT_Methodref: return sizeof(Methodref_info);
        case CONSTANT_String: return sizeof(String_info);
        //string
        default: return 0;
    }
}

void _fwrite_class(Class_info *ptr, FILE *f) {
    fwrite(&ptr->tag, sizeof(u1), 1, f);
    ptr->name_index = htons(ptr->name_index);
    fwrite(&ptr->name_index, sizeof(u2), 1, f);
}

void _fwrite_fieldref(Fieldref_info*ptr, FILE *f) {
    fwrite(&ptr->tag, sizeof(u1), 1, f);
    ptr->class_index = htons(ptr->class_index);
    fwrite(&ptr->class_index, sizeof(u2), 1, f);
    ptr->name_and_type_index = htons(ptr->name_and_type_index);
    fwrite(&ptr->name_and_type_index, sizeof(u2), 1, f);
}

void _fwrite_float(Float_info *ptr, FILE *f) {
    fwrite(&ptr->tag, sizeof(u1), 1, f);
    ptr->bytes = htonl(ptr->bytes);
    fwrite(&ptr->bytes, sizeof(u4), 1, f);
}

void _fwrite_integer(Integer_info *ptr, FILE *f) {
    fwrite(&ptr->tag, sizeof(u1), 1, f);
    ptr->bytes = htonl(ptr->bytes);
    fwrite(&ptr->bytes, sizeof(u4), 1, f);
}

void _fwrite_nameandtype(NameAndType_info *ptr, FILE *f) {
    fwrite(&ptr->tag, sizeof(u1), 1, f);
    ptr->name_index = htons(ptr->name_index);
    fwrite(&ptr->name_index, sizeof(u2), 1, f);
    ptr->descriptor_index = htons(ptr->descriptor_index);
    fwrite(&ptr->descriptor_index, sizeof(u2), 1, f);
}

void _fwrite_utf8(Utf8_info *ptr, FILE *f) {
    fwrite(&ptr->tag, sizeof(u1), 1, f);
    u2 len = ptr->length;
    ptr->length = htons(ptr->length);
    fwrite(&ptr->length, sizeof(u2), 1, f);
    fwrite(ptr->bytes, len, 1, f);
}

void _fwrite_methodref(Methodref_info *ptr, FILE *f) {
    fwrite(&ptr->tag, sizeof(u1), 1, f);
    ptr->class_index = htons(ptr->class_index);
    fwrite(&ptr->class_index, sizeof(u2), 1, f);
    ptr->name_and_type_index = htons(ptr->name_and_type_index);
    fwrite(&ptr->name_and_type_index, sizeof(u2), 1, f);
}

void _fwrite_string(String_info *ptr, FILE *f) {
    fwrite(&ptr->tag, sizeof(u1), 1, f);
    ptr->string_index = htons(ptr->string_index);
    fwrite(&ptr->string_index, sizeof(u2), 1, f);
}

void constant_pool_fwrite(constant_pool *ptr, int index, FILE *f) {
    if (index >= ptr->entry_count) {
        perror("Incorrect index");
        return;
    }
    u1 tag = *((u1 *)ptr->pool[index]);
    void *info = ptr->pool[index];
    switch(tag) {
        case CONSTANT_Class: 
            _fwrite_class(info, f);
            break;
        case CONSTANT_Fieldref: 
            _fwrite_fieldref(info, f);
            break;
        case CONSTANT_Float: 
            _fwrite_float(info, f);
            break;
        case CONSTANT_Integer: 
            _fwrite_integer(info, f);
            break;
        case CONSTANT_NameAndType: 
            _fwrite_nameandtype(info, f);
            break;
        case CONSTANT_Utf8: 
            _fwrite_utf8(info, f);
            break;
        case CONSTANT_Methodref: 
            _fwrite_methodref(info, f);
            break;
        case CONSTANT_String:
            _fwrite_string(info, f);
            break;
        default: return;
    }
}
