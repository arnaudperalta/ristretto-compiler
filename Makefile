class_compilerdir = class_compiler/
constant_pooldir = constant_pool/
method_pooldir = method_pool/
field_pooldir = field_pool/
stack_manadir = stack_manager/

SHELL=/bin/sh
LEX=flex
YACC=bison
CC=gcc
CFLAGS=-g -std=c11 -pedantic -Wall -I$(class_compilerdir) -I$(constant_pooldir) -I$(field_pooldir) -I$(method_pooldir) -I$(stack_manadir)
LDFLAGS=
VPATH = $(class_compilerdir) $(constant_pooldir) $(field_pooldir) $(method_pooldir) $(stack_manadir)
LEXOPTS = -D_POSIX_SOURCE -DYY_NO_INPUT --nounput
OBJ = *.o $(class_compilerdir)*.o $(constant_pooldir)*.o $(method_pooldir)*.o $(field_pooldir)*.o $(stack_manadir)*.o
PROG=ristretto

all: lex.yy.o $(PROG).tab.o $(class_compilerdir)class_compiler.o $(constant_pooldir)constant_pool.o $(field_pooldir)field_pool.o $(method_pooldir)method_pool.o $(stack_manadir)stack_manager.o
	$(CC) $+ -o $(PROG) $(LDFLAGS)

lex.yy.c: $(PROG).l $(PROG).tab.h
	$(LEX) $(LEXOPTS) $<

lex.yy.h: $(PROG).l
	$(LEX) $(LEXOPTS) --header-file=$@ $<

$(PROG).tab.c $(PROG).tab.h: $(PROG).y lex.yy.h
	$(YACC) $(YACCOPTS) $< -d -v --graph

clean:
	$(RM) $(OBJ) $(PROG) $(PROG).tab.* *.vcg *.output lex.yy.* *.class

tar:
	$(MAKE) clean
	tar -zcf "$(CURDIR).tar.gz" ./* class_compiler/* constant_pool/* method_pool/* field_pool/* stack_manager/*
	
$(class_compilerdir)class_compiler.o : class_compiler.c class_compiler.h constant_pool.h field_pool.h method_pool.h stack_manager.h
$(constant_pooldir)constant_pool.o : constant_pool.c constant_pool.h
$(field_pooldir)field_pool.o : field_pool.c field_pool.h
$(method_pooldir)method_pool.o : method_pool.c method_pool.h
$(stack_manadir)stack_manager.o : stack_manager.c stack_manager.h class_compiler.h
