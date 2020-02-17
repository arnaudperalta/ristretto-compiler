/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TYPE = 258,
     IDENTIFIER = 259,
     ARRAY = 260,
     BOOLCONS = 261,
     INTCONS = 262,
     FLOATCONS = 263,
     PV = 264,
     VIRG = 265,
     END_OF_FILE = 266,
     ASSIGNMENT = 267,
     OPERATOR = 268,
     OPEN_PAR = 269,
     CLOSE_PAR = 270,
     OPEN_BRA = 271,
     CLOSE_BRA = 272
   };
#endif
/* Tokens.  */
#define TYPE 258
#define IDENTIFIER 259
#define ARRAY 260
#define BOOLCONS 261
#define INTCONS 262
#define FLOATCONS 263
#define PV 264
#define VIRG 265
#define END_OF_FILE 266
#define ASSIGNMENT 267
#define OPERATOR 268
#define OPEN_PAR 269
#define CLOSE_PAR 270
#define OPEN_BRA 271
#define CLOSE_BRA 272




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 35 "ristretto.y"
{
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
/* Line 1529 of yacc.c.  */
#line 96 "ristretto.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

