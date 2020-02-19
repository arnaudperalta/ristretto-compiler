class_compilerdir = class_compiler/
constant_pooldir = constant_pool/
field_pooldir = field_pool/
SHELL=/bin/sh
LEX=flex
YACC=bison
CC=gcc
CFLAGS=-g -std=c11 -pedantic -Wall -I$(class_compilerdir) -I$(constant_pooldir) -I$(field_pooldir)
LDFLAGS=-ly
VPATH = $(class_compilerdir) $(constant_pooldir) $(field_pooldir)
LEXOPTS=-D_POSIX_SOURCE -DYY_NO_INPUT --nounput
YACCOPTS=
PROG=ristretto

all: lex.yy.o $(PROG).tab.o $(class_compilerdir)class_compiler.o $(constant_pooldir)constant_pool.o $(field_pooldir)field_pool.o
	$(CC) $+ -o $(PROG) $(LDFLAGS)

lex.yy.c: $(PROG).l $(PROG).tab.h
	$(LEX) $(LEXOPTS) $<

lex.yy.h: $(PROG).l
	$(LEX) $(LEXOPTS) --header-file=$@ $<

$(PROG).tab.c $(PROG).tab.h: $(PROG).y lex.yy.h
	$(YACC) $(YACCOPTS) $< -d -v --graph

clean:
	-rm $(PROG) *.o lex.yy.* $(PROG).tab.* *.err *.output *.out *.dot *.class *.vcg
	
$(class_compilerdir)class_compiler.o : class_compiler.c class_compiler.h constant_pool.h field_pool.h
$(constant_pooldir)constant_pool.o : constant_pool.c constant_pool.h
$(field_pooldir)field_pool.o : field_pool.c field_pool.h
