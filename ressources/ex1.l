%{
  #include <limits.h>
  #include "ex1.tab.h"
  void string_to_int(int *v, const char *s);
%}
%option noyywrap
%%
[[:digit:]]+               { A FAIRE }
[[:blank:]]
.|\n                       { A FAIRE }
%%

void string_to_int(int *r, const char *s) {
  char *p;
  long v;
  errno = 0;
  v = strtol(yytext, &p, 10);
  if ( ( *p != '\0' || ( errno == ERANGE 
                     && ( v == LONG_MIN || v == LONG_MAX ) ) ) 
       || ( v < INT_MIN || v > INT_MAX ) ) {
    fprintf(stderr, "Error converting string to int\n");
    exit(EXIT_FAILURE);
  } 
  *r = v;
}
