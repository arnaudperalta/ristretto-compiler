%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "class_compiler.h"

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

int add_variable_to_pool(char *type, char *name, char *data_type, void *data);
int add_funconstant_to_pool(char *type, char *name, char *params);

/* Données */

constant_pool *pool;
int line_analyse = 0; // Ligne en cours d'analyse, permet de traquer la ligne de l'erreur

%}
%union {
    struct {
        char *type;
        void *data;
        int line;
    } constr;

    char *text;
    int var_int;
    short var_short;
    float var_float;
}

//Symboles terminaux qui seront fournis par yylex()

%token<text> TYPE IDENTIFIER ARRAY
%token<constr> BOOLCONS INTCONS FLOATCONS
%token PV VIRG END_OF_FILE
%token ASSIGNMENT
%token OPERATOR
%token OPEN_PAR CLOSE_PAR OPEN_BRA CLOSE_BRA
%type<constr> Constructeur
%type<text> Function_param Type

%%
// --------- Début de la grammaire ---------
S: 
    Variable_declar S
|   Function_declar S
|   ;

// Enregistrement d'une variable
Variable_declar: 
    Type IDENTIFIER ASSIGNMENT Constructeur PV { 
        line_analyse = $4.line;
        add_variable_to_pool($1, $2, $4.type, $4.data);
    }
    | Type IDENTIFIER PV {
        add_variable_to_pool($1, $2, NULL, NULL);
    }

Constructeur:
    BOOLCONS { $$ = $1 }
    | FLOATCONS { $$ = $1 }
    | INTCONS { $$ = $1 }

Function_declar:
    Type IDENTIFIER Function_param PV {
        add_funconstant_to_pool($1, $2, $3);
    }
|   Type IDENTIFIER Function_param OPEN_BRA Function_body CLOSE_BRA {
        add_funconstant_to_pool($1, $2, $3);
    }

Function_param:
    OPEN_PAR Type CLOSE_PAR {
        if (strcmp($2, "void") != 0) 
            yyerror("Invalid function prototype, missing identifier.");
        $$ = NULL;
    }
    | OPEN_PAR Type IDENTIFIER CLOSE_PAR {
        char param[PARAM_LENGTH];
        sprintf(param, "%s;%s,", $2, $3);
        $$ = param;
    }
    | OPEN_PAR Type IDENTIFIER VIRG Function_param CLOSE_PAR {
        char *str;
        if ($5 == NULL) {
            char tab[FUNCTION_PARAM_LENGTH] = "";
            str = tab;
        } else {
            str = $5;
        }
        char param[PARAM_LENGTH];
        sprintf(param, "%s;%s,", $2, $3);
        strcat(str, param);
        $$ = str;
    }
    | Type IDENTIFIER {
        char param[PARAM_LENGTH];
        sprintf(param, "%s;%s,", $1, $2);
        $$ = param;
    }

Function_body: ;

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
    // Si un filename est donné, on créé un fichier .class
    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("erreur fopen");
        return EXIT_FAILURE;
    }
    char *token = strtok(argv[1], ".");
    token[0] = toupper(token[0]);
    //sprintf(filename, "%s.class", token);
    //yyout = fopen(filename, "w");
    /*if (yyout == NULL) {
        perror("erreur fopen");
        return EXIT_FAILURE;
    }*/
    constant_pool *pool = constant_pool_init(token);
    if (pool == NULL) {
        perror("constant pool init erreur");
        return EXIT_FAILURE;
    }
    yyparse();

    fclose(yyout);

    return EXIT_SUCCESS;
}

// On renvoie une erreur bison si le type ne correspond pas a la donnée
int add_variable_to_pool(char *type, char *name, char *data_type, void *data) {
    if (data_type != NULL && strncmp(type, data_type, 3) != 0) {
        printf("%s %s\n", type, data_type);
        yyerror("Incompatible value for the previous type.");
        return -1;
    }
    constant_pool_field_entry(pool, name, type);
    free(data);
    return 0;
}

int add_funconstant_to_pool(char *type, char *name, char *params) {
    if (params == NULL)
        printf("function void\n");
    else
        printf("func name : %s params : %s\n", name, params);
    constant_pool_method_entry(pool, name, type);
    return 0;
}
