%{
#include "ristretto.tab.h"

char *strdup(const char *str)
{
    int n = strlen(str) + 1;
    char *dup = malloc(n);
    if(dup)
    {
        strcpy(dup, str);
    }
    return dup;
}

int llerror(char *s) {
    printf("Ristretto parsing : Syntax error at line %d\nReason : %s\n", yylineno + 2, s);
    return 0;
}
%}

%option noyywrap
%option yylineno

type        int|boolean|float|void|int
array       \[\]|\[\]\[\]
identifier  [_a-zA-Z][_a-zA-Z0-9]*
operator    [=][=]|[&][&]|[|][|]|[+]|[-]|[/]|[*]|[%]
assignment  =
modify      [=]|[+][=]|[-][=]|[%][=]|[/][=]
boolcons    true|false
intcons     [0-9]+
floatcons   [+-]?([0-9]*[.])?[0-9]+

%%

"//".*      { ; }
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/] ;
[/][*]      { llerror("Unfinished comment."); }
[(]         { return(OPEN_PAR); }
[)]         { return(CLOSE_PAR); }
[{]         { return(OPEN_BRA); }
[}]         { return(CLOSE_BRA); }
[;]         { return(PV); }
[,]         { return(VIRG); }
[ \t\n]       ;

{type}       { yylval.text = strdup(yytext); return TYPE; }
{array}      { 
    yylval.text = yytext;
    return ARRAY;
}
{operator}   { return OPERATOR; }
{assignment} { return ASSIGNMENT; }
{intcons}    {
    int i = atoi(yytext);
    int *ptr = malloc(sizeof (int));
    *ptr = i;
    yylval.constr.data = ptr;
    yylval.constr.type = "int";
    yylval.constr.line = yylineno;
    return INTCONS; 
}
{boolcons}   { 
    int i = (strcmp(yytext, "true") == 0);
    int *ptr = malloc(sizeof (int));
    *ptr = i;
    yylval.constr.data = ptr;
    yylval.constr.type = "boolean";
    return BOOLCONS; 
}
{floatcons}  {
    float f = atof(yytext);
    int *ptr = malloc(sizeof (float));
    *ptr = f;
    yylval.constr.data = ptr;
    yylval.constr.type = "float";
    return FLOATCONS; 
}
{identifier} { 
    yylval.text = strdup(yytext); 
    return IDENTIFIER; 
}

%%
