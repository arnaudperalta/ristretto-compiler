%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "class_compiler.h"
#include "method_pool.h"
#include "stack_manager.h"

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

int add_field_to_compiler(char *type, char *name, char *data_type, void *data);
void close_constructor(void);

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
%token PV VIRG RETURN IF ELSE WHILE
%token ASSIGNMENT
%token OPEN_PAR CLOSE_PAR OPEN_BRA CLOSE_BRA
%type<constr> Constructeur Expression Function Function_call_params
%type<text> Function_param Type Print Variable Parametre
%left<text> OR
%right<text> AND
%left<text> PLUS MOINS CMP
%left<text> MUL DIV NOT

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
    BOOLCONS { $$ = $1; }
    | FLOATCONS { $$ = $1; }
    | INTCONS { $$ = $1; }
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
    Function_param VIRG Parametre {
        char *param = malloc(PARAM_LENGTH);
        sprintf(param, "%s,%s", $1, $3);
        $$ = param;
    }
    | Parametre {
        $$ = $1;
    }

Parametre:
    Type IDENTIFIER {
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
    | Condition Function_body
    | Loop Function_body
    | Print_expression Function_body
    | Function_call Function_body
    | Return
    | ;

Variable_declaration:
    Type IDENTIFIER PV {
        stack_value_to_func($1, NULL);
        add_variable_to_func($1, $2);
    }
    | Type IDENTIFIER ASSIGNMENT Expression PV {
        add_variable_to_func($1, $2);
    }

Variable_modif:
    IDENTIFIER ASSIGNMENT Expression PV {
        modify_local_to_func($1);
    }

Condition:
    If_init OPEN_PAR Expression CLOSE_PAR If_begin Function_body If_end Else

If_init: 
    IF {
        // On push un 0 sur le stack, car si on ne rentre pas dans ce if
        // on pourra rentrer dans le else
        method_instruction(to_build, 0x04);
    }

If_begin:
    OPEN_BRA {
        init_condition(is);
        // En rentrant dans le if on push un 1 pour ne pas rentrer dans le else
        // on pop le 0 d'avant
        method_instruction(to_build, 0x57);
        method_instruction(to_build, 0x03);
    }

If_end:
    CLOSE_BRA {
        finish_condition(is);
    }

Else:
    ELSE Else_begin Function_body Else_end
    | ;

Else_begin:
    OPEN_BRA {
        init_condition(is);
    }

Else_end:
    CLOSE_BRA {
        finish_condition(is);
    }

Loop:
    While_init OPEN_PAR Expression CLOSE_PAR While_begin Function_body While_end

While_init:
    WHILE {
        init_while(ws);
    }

While_begin:
    OPEN_BRA {
        begin_while(ws);
    }

While_end:
    CLOSE_BRA {
        finish_while(ws);
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
        char *type = malloc(5);
        int index = method_search_local(to_build, $1, type);
        if (index < 0) {
            // On a rien trouvé dans local, on cherche dans les variables globales
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
        char *type_dup = malloc(strlen($1.type) + 1);
        if(type_dup)
        {
            strcpy(type_dup, $1.type);
        }
        strtok(type_dup, ")");
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
    | Expression CMP Expression {
        if (strcmp($1.type, $3.type) != 0) {
            yyerror("Incorrect operandes type");
            return -1;
        }
        stack_operator_to_func($2, $1.type);
    }
    | Expression AND Expression {
        if (strcmp($1.type, $3.type) != 0) {
            yyerror("Incorrect operandes type");
            return -1;
        }
        stack_operator_to_func($2, $1.type);
    }
    | Expression OR Expression {
        if (strcmp($1.type, $3.type) != 0) {
            yyerror("Incorrect operandes type");
            return -1;
        }
        stack_operator_to_func($2, $1.type);
    }
    | OPEN_PAR Expression CLOSE_PAR {
        $$ = $2;
    }
    | NOT Expression {
        stack_operator_to_func($1, $2.type);
    }

Variable:
    IDENTIFIER {
        $$ = $1;
    }

Function_call:
    Function PV {
        stack_funcall_to_func($1.name);
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
    Function_call_params VIRG Expression
    | Expression

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
    printf("Ristretto parsing : Syntax error. Reason : %s\n", s);
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
    method_pool_end(cc->mp);
}
