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

type        int|boolean|float|void
array       \[\]|\[\]\[\]
identifier  [_a-zA-Z][_a-zA-Z0-9]*
assignment  =
cmp         [<]|[>]|<=|>=|==
boolcons    true|false
intcons     [-]?[0-9]+
floatcons   [-]?([0-9]*[.])?[0-9]+
strcons     \"(?:[^"\\]|\\.)*\"

%%

"//".*      { ; }
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/] ;
[/][*]      { llerror("Unfinished comment."); }
[+]         { yylval.text = strdup(yytext); return PLUS; }
[-]         { yylval.text = strdup(yytext); return MOINS; }
[*]         { yylval.text = strdup(yytext); return MUL; }
[/]         { yylval.text = strdup(yytext); return DIV; }
&&          { yylval.text = strdup(yytext); return AND; }
\|\|          { yylval.text = strdup(yytext); return OR; }
[!]         { yylval.text = strdup(yytext); return NOT; }
[(]         { return OPEN_PAR; }
[)]         { return CLOSE_PAR; }
[{]         { return OPEN_BRA; }
[}]         { return CLOSE_BRA; }
[;]         { return PV; }
[,]         { return VIRG; }

print       { yylval.text = strdup("print"); return PRINT; }
println     { yylval.text = strdup("println"); return PRINTLN; }
if          { return IF; }
else        { return ELSE; }
while       { return WHILE; }
return      { return RETURN; }
[ \t\n]     ;

{type}       {
    if (strcmp(yytext, "int") == 0) {
        yylval.text = strdup("I");
    } else if (strcmp(yytext, "boolean") == 0) {
        yylval.text = strdup("Z");
    } else if (strcmp(yytext, "float") == 0) {
        yylval.text = strdup("F");
    } else {
        yylval.text = strdup("V");
    }
    return TYPE;
}
{array} { 
    if (strcmp(yytext, "[]") == 0) {
        yylval.text = strdup("[");
    } else {
        char dble_sqbrckts[3];
        dble_sqbrckts[0] = '[';
        dble_sqbrckts[1] = '[';
        dble_sqbrckts[2] = '\0';
        yylval.text = strdup(dble_sqbrckts);
    }
    return ARRAY;
}
{assignment} { return ASSIGNMENT; }
{intcons}    {
    int i = atoi(yytext);
    int *ptr = malloc(sizeof (int));
    *ptr = i;
    yylval.constr.data = ptr;
    yylval.constr.type = "I";
    yylval.constr.line = yylineno;
    return INTCONS; 
}
{boolcons}   { 
    int i = (strcmp(yytext, "true") == 0);
    int *ptr = malloc(sizeof (int));
    *ptr = i;
    yylval.constr.data = ptr;
    yylval.constr.type = "Z";
    return BOOLCONS; 
}
{floatcons}  {
    float f = atof(yytext);
    float *ptr = malloc(sizeof (float));
    *ptr = f;
    yylval.constr.data = ptr;
    yylval.constr.type = "F";
    return FLOATCONS; 
}
{strcons} {
    yytext[strlen(yytext) - 1] = '\0';
    yylval.constr.data = strdup(yytext + 1);
    yylval.constr.type = "Ljava/lang/String;";
    return STRCONS;
}
{identifier} { 
    yylval.text = strdup(yytext); 
    return IDENTIFIER; 
}
{cmp} { 
    yylval.text = strdup(yytext);
    return CMP; 
}

%%
