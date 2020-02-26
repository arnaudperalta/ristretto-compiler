%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "class_compiler.h"
#include "method_pool.h"

#define TYPE_LENGTH 256
#define FUNCTION_PARAM_LENGTH 256
#define PARAM_LENGTH 30
#define FILENAME_LENGTH 100

/* --------- Définition bison ---------- */

int yyparse(); // Code qui sera généré suite à la compilation de ce fichier
int yylex(); //Traitement de l'analyse syntaxique par la grammaire définie dans un .l
int yyerror(char *s);
FILE *yyin;
FILE *yyout;

/* ---------- Définition utilisateur ---------- */

/* Fonctions */

int add_field_to_compiler(char *type, char *name, char *data_type, void *data);
int add_funconstant_to_pool(char *type, char *name, char *params);
void close_constructor(void);

void create_function(char *type, char *name, char *params);
void add_variable_to_func(char *type, char *name);
void modify_local_to_func(char *name);
void modify_global_to_func(u2 index);
void init_print_to_func(void);
void add_print_to_func(char *type, char *name);
void stack_value_to_func(char *type, void *value);
void stack_operator_to_func(char *operator, char *type);
char *stack_local_to_func(char *name);
void stack_funcall_to_func(char *name);
void stack_global_to_func(u2 index);
void add_return_to_func(char *type);
void close_function(void);

/* Données */

class_compiler *cc;
int line_analyse = 0; // Ligne en cours d'analyse, permet de traquer la ligne de l'erreur

/* Données pour la construction de fonction */
method *to_build;
method *clinit; // Constructeur
u2 locals_count;
u2 func_name_index;
u2 func_type_index;

%}
%union {
    struct {
        char *type;
        char *name;
        void *data;
        int line;
    } constr;

    char *text;
    int var_int;
    short var_short;
    float var_float;
}

//Symboles terminaux qui seront fournis par yylex()

%token<text> TYPE IDENTIFIER ARRAY PRINT PRINTLN
%token<constr> BOOLCONS INTCONS FLOATCONS STRCONS
%token PV VIRG RETURN
%token ASSIGNMENT
%token OPEN_PAR CLOSE_PAR OPEN_BRA CLOSE_BRA
%type<constr> Constructeur Expression Function Function_call_params
%type<text> Function_param Type Print Variable
%left<text> PLUS MOINS
%left<text> MUL DIV

%start S

%%
// --------- Début de la grammaire ---------
S: 
    Function_declar S
    | Global_variable_declar S
    |   ;

// Enregistrement d'une variable
Global_variable_declar: 
    Type IDENTIFIER ASSIGNMENT Expression PV {
        int index = add_field_to_compiler($1, $2, $4.type, $4.data);
        modify_global_to_func(index);
    }
    | Type IDENTIFIER PV {
        int index = add_field_to_compiler($1, $2, NULL, NULL);
        modify_global_to_func(index);
    }

// Constructeur de données
Constructeur:
    BOOLCONS { $$ = $1 }
    | FLOATCONS { $$ = $1 }
    | INTCONS { $$ = $1 }
    | STRCONS { $$ = $1; }

Function_declar:
    Function_proto OPEN_BRA Function_body CLOSE_BRA {
        close_function();
    }

Function_proto:
    Type IDENTIFIER OPEN_PAR Function_param CLOSE_PAR {
        char *func_type = malloc(strlen($4) + 5);
        sprintf(func_type, "%s", $1);
        create_function(func_type, $2, $4);
    }

Function_param:
    Function_param VIRG Function_param {
        char *param = malloc(PARAM_LENGTH);
        sprintf(param, "%s,%s", $1, $3);
        $$ = param;
    }
    | Type IDENTIFIER {
        char *param = malloc(PARAM_LENGTH);
        sprintf(param, "%s;%s", $1, $2);
        $$ = param;
    }
    | Type {
        if (strcmp($1, "V") != 0) 
            yyerror("Invalid function prototype, missing identifier.");
        $$ = "";
    }

Function_body:
    Variable_declaration Function_body
    | Variable_modif Function_body
    | Print_expression Function_body
    | Return
    | ;

Variable_declaration:
    Type IDENTIFIER PV {
        stack_value_to_func($1, NULL);
        add_variable_to_func($1, $2);
    }
    | TYPE IDENTIFIER ASSIGNMENT Expression PV {
        add_variable_to_func($1, $2);
    }

Variable_modif:
    IDENTIFIER ASSIGNMENT Expression PV {
        modify_local_to_func($1);
    }

Print_expression:
    Print Expression PV {
        add_print_to_func($2.type, $1);
    }

Print:
    PRINT {
        init_print_to_func();
        $$ = $1;
    }
    | PRINTLN {
        init_print_to_func();
        $$ = $1;
    }

Return:
    RETURN PV {
        add_return_to_func("V");
    }
    | RETURN Expression PV {
        add_return_to_func($2.type);
    }

Expression:
    Constructeur {
        stack_value_to_func($1.type, $1.data);
        $$ = $1;
    }
    | Variable {
        $$.name = $1;
        int index = method_search_local(to_build, $1, NULL);
        if (index < 0) {
            // On a rien trouvé dans local, on cherche dans les variables globales
            char *type = malloc(5);
            index = field_pool_search(cc->fp, $1, type);
            if (index < 1) {
                yyerror("Bad variable name");
                return -1;
            }
            stack_global_to_func(index);
            $$.type = type;
        } else {
            $$.type = stack_local_to_func($1);
        }
    }
    | Function { 
        stack_funcall_to_func($1.name);
        strtok($1.type, ")");
        $$.type = strtok(NULL, ")");
    }
    | Expression PLUS Expression {
        if (strcmp($1.type, $3.type) != 0) {
            printf("dbg : %s %s\n", $1.type, $3.type);
            yyerror("Incorrect operandes type");
            return -1;
        }
        stack_operator_to_func($2, $1.type);
    }
    | Expression MOINS Expression {
        if (strcmp($1.type, $3.type) != 0) {
            yyerror("Incorrect operandes type");
            return -1;
        }
        stack_operator_to_func($2, $1.type);
    }
    | Expression MUL Expression {
        if (strcmp($1.type, $3.type) != 0) {
            yyerror("Incorrect operandes type");
            return -1;
        }
        stack_operator_to_func($2, $1.type);
    }
    | Expression DIV Expression {
        if (strcmp($1.type, $3.type) != 0) {
            yyerror("Incorrect operandes type");
            return -1;
        }
        stack_operator_to_func($2, $1.type);
    }
    | OPEN_PAR Expression CLOSE_PAR {
        $$ = $2;
    }

Variable:
    IDENTIFIER {
        $$ = $1
    }

Function:
    IDENTIFIER OPEN_PAR Function_call_params CLOSE_PAR {
        char *fun_type = method_pool_search(cc->mp, $1);
        if (fun_type == NULL) {
            yyerror("Unknown function.");
        } else {
            $$.type = fun_type;
            $$.name = $1;
        }
    }
    | IDENTIFIER OPEN_PAR CLOSE_PAR {
        char *fun_type = method_pool_search(cc->mp, $1);
        if (fun_type == NULL) {
            yyerror("Unknown function.");
        } else {
            $$.type = fun_type;
            $$.name = $1;
        }
    }

// On regarde si la variable est locale sinon on chercher dans les globales
Function_call_params:
    Function_call_params VIRG Function_call_params
    | Constructeur {
        stack_value_to_func($1.type, $1.data);
    }

Type:
    TYPE {
        $$ = $1;
    }
    | TYPE ARRAY {
        char str[PARAM_LENGTH] = {0};
        strcat(str, $2);
        strcat(str, $1);
        $$ = str;
    }
%%

int yyerror(char *s) {
    printf("Ristretto parsing : Syntax error at line %d\nReason : %s\n", line_analyse + 2, s);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        perror("Please give a filename.");
        exit(EXIT_FAILURE);
    }
    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        printf("Le fichier entré n'existe pas.\n");
        return EXIT_FAILURE;
    }
    char *token = strtok(argv[1], ".");
    token[0] = toupper(token[0]);

    cc = class_compiler_init(token);
    if (cc == NULL) {
        perror("class compiler init erreur");
        return EXIT_FAILURE;
    }
    clinit = method_pool_get_clinit(cc->mp);
    to_build = clinit;
    yyparse();
    // On termine l'écriture du constructeur
    close_constructor();
    // On écrit le fichier .class
    class_compiler_print(cc);

    return EXIT_SUCCESS;
}

// Permet d'ajouter a la méthode constructeur l'assignement de la valeur
/*void add_to_constructor(char *type, char *name, void *data, u2 ref_index, u2 data_index) {
    if (strcmp(type, "Z") == 0) {
        int b = (data == NULL) ? 0 : *((int *) data);
        if (b == 1) {
            add_instr_constructor(cc, 0x04);
        } else {
            add_instr_constructor(cc, 0x03);
        } 
    } else if (strcmp(type, "F") == 0 || strcmp(type, "I") == 0) {
        // ldc dans le cas ou la variable est un int ou un float
        add_instr_constructor(cc, 0x13);
        // push de la constante en haut du stack
        add_instr_constructor(cc, data_index >> 8);
        add_instr_constructor(cc, data_index);
    } else {
        return;
    }
    // push du constante sur le field_ref associé
    add_instr_constructor(cc, 0xb3);
    // Le premier octet du numero de ligne de field_ref
    add_instr_constructor(cc, ref_index >> 8);
    add_instr_constructor(cc, ref_index);
}*/

// On renvoie une erreur bison si le type ne correspond pas a la donnée
// On appelle cette fonction quand on reconnait une variable globale dans le .ris
int add_field_to_compiler(char *type, char *name, char *data_type, void *data) {
    if (data_type != NULL && strncmp(type, data_type, 3) != 0) {
        printf("%s %s\n", type, data_type);
        yyerror("Incompatible value for the previous type.");
        return -1;
    }
    u2 ref_index;
    u2 data_index;
    int index = class_compiler_add_field(cc, name, type, data, &ref_index, &data_index);
    if (index < 0) {
        perror("Erreur class compiler add field");
        return -1;
    }
    // On ajoute sa valeur dans le constructeur, 
    // et si data vaut NULL, on lui affectera une valeur par défaut
    //add_to_constructor(type, name, data, ref_index, data_index);

    return index;
}

void close_constructor(void) {
    close_method_pool(cc);
}

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
