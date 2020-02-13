#include <stdlib.h>
#include "class_compiler.h"

#define MAX_CONSTANT 512
#define CLASS_LINE   7

u4 magic = 0xCAFEBABE;
u2 mineur = 0x0000;
u2 majeur = 0x0034;
u2 access_flags = 0x0009; // 0x0001 |= 0x0008
u2 interface_count = 0;
u2 field_count = 0;
u2 methods_count = 0;
u2 attr_count = 0;

struct constant_pool {
    u2 entry_count;
    void **pool;
};

constant_pool *constant_pool_init(char *class_name) {
    constant_pool *ptr = malloc(sizeof (constant_pool));
    if (ptr == NULL)
        return NULL;
    ptr->entry_count = 1; // on démarre a 1 comme dans l'énoncé
    ptr->pool = malloc((sizeof (void *)) * MAX_CONSTANT);
    if (ptr->pool == NULL)
        return NULL;

    // 1. Methodref_info : (2, 3);
    if (constant_pool_entry(ptr, new_methodref(2, 3)) != 0) {
        perror("entry cons erreur");
        return NULL;
    }
    // 2. Class_info : 4;
    if (constant_pool_entry(ptr, new_class(4)) != 0) {
        perror("entry ci erreur");
        return NULL;
    }
    // 3. NameAndType_info : (5, 6);
    if (constant_pool_entry(ptr, new_nameandtype(5, 6)) != 0) {
        perror("entry nat erreur");
        return NULL;
    }
    // 4. Utf8_info : "java/lang/Object";
    if (constant_pool_entry(ptr, new_utf8("java/lang/Object")) != 0) {
        perror("entry class erreur");
        return NULL;
    }
    // 5. Utf8_info : "<init>";
    if (constant_pool_entry(ptr, new_utf8("<init>")) != 0) {
        perror("entry class erreur");
        return NULL;
    }
    // 6. Utf8_info : "()V";
    if (constant_pool_entry(ptr, new_utf8("()V")) != 0) {
        perror("entry class erreur");
        return NULL;
    }
    // 7. Class_info : 8;
    if (constant_pool_entry(ptr, new_class(8)) != 0) {
        perror("entry ci erreur");
        return NULL;
    }
    //8. Utf8_info : "class_name";
    if (constant_pool_entry(ptr, new_utf8(class_name)) != 0) {
        perror("entry class erreur");
        return NULL;
    }

    return ptr;
}

int constant_pool_entry(constant_pool *ptr, void *entry) {
    if (ptr->entry_count == MAX_CONSTANT) 
        return -1;
    ptr->pool[ptr->entry_count] = entry;
    ptr->entry_count++;
    return 0;
}

int constant_pool_method_entry(constant_pool *ptr, char *name, char *type) {
    u2 line_count = constant_pool_count(ptr);
    // x + 1. Methodref_info : (7, 2);
    if (constant_pool_entry(ptr, new_methodref(CLASS_LINE, line_count + 2)) != 0) {
        perror("entry method erreur");
        return NULL;
    }
    // x + 2. NameAndType_info : (x + 3, x + 4);
    if (constant_pool_entry(ptr, new_nameandtype(line_count + 3, line_count + 4)) != 0) {
        perror("entry nameandtype erreur");
        return NULL;
    }
    // x + 3. Utf8_info : ”fact” ;
    if (constant_pool_entry(ptr, new_utf8(name)) != 0) {
        perror("entry utf8 erreur");
        return NULL;
    }
    // x + 4. Utf8_info : ”(I)I” ;
    if (constant_pool_entry(ptr, new_utf8(type)) != 0) {
        perror("entry utf8 erreur");
        return NULL;
    }
    return 0;
}

int constant_pool_field_entry(constant_pool *ptr, char *name, char *type) {
    u2 line_count = constant_pool_count(ptr);
    // x + 1. Methodref_info : (7, 2);
    if (constant_pool_entry(ptr, new_fieldref(CLASS_LINE, line_count + 2)) != 0) {
        perror("entry field erreur");
        return NULL;
    }
    // x + 2. NameAndType_info : (x + 3, x + 4);
    if (constant_pool_entry(ptr, new_nameandtype(line_count + 3, line_count + 4)) != 0) {
        perror("entry nameandtype erreur");
        return NULL;
    }
    // x + 3. Utf8_info : ”fact” ;
    if (constant_pool_entry(ptr, new_utf8(name)) != 0) {
        perror("entry utf8 erreur");
        return NULL;
    }
    // x + 4. Utf8_info : ”(I)I” ;
    if (constant_pool_entry(ptr, new_utf8(type)) != 0) {
        perror("entry utf8 erreur");
        return NULL;
    }
    return 0;
}

u2 constant_pool_count(constant_pool *ptr) {
    if (ptr == NULL)
        return 0;
    return ptr->entry_count;
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

void constant_pool_debug(constant_pool *ptr) {
    for (int i = 0; i < ptr->entry_count; ++i) {
        printf("%p\n", ptr->pool[i]);
    }
}
