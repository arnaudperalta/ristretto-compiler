%{
  #include <ctype.h>
  #include <stdlib.h>
  #include <stdio.h>
  int yylex(void);
  void yyerror(char const *);
  #define STACK_CAPACITY 50
  static int stack[STACK_CAPACITY];
  static size_t stack_size = 0;
%}
%define api.value.type { A FAIRE }
%token NUMBER
DEFINIR PRIORITES ET ASSOCIATIVITE DES OPERATEURS
%start lignes
%%
lignes:
  lignes error '\n'    { stack_size = 0; yyerrok; }
| expr error '\n'      { stack_size = 0; yyerrok; }
| error '\n'           { stack_size = 0; yyerrok; }
| lignes expr '\n'     { printf("%d\n", stack[0]); stack_size = 0; }
| lignes '\n'
| expr '\n'            { printf("%d\n", stack[0]); stack_size = 0; }
| '\n'
;

expr :
;
%%
 
void yyerror(char const *s) {
  fprintf(stderr, "%s\n", s);
}
