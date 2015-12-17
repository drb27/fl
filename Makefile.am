ACLOCAL_AMFLAGS=-I m4
SUBDIRS=.
bin_PROGRAMS=shell
noinst_LIBRARIES = libparse.a
BUILT_SOURCES = src/parser/parser.tab.h src/parser/parser.c src/parser/lexer.c
libparse_a_SOURCES=src/parser/lexer.l src/parser/parser.y
shell_SOURCES=src/shell/shell.cpp
shell_LDADD=libparse.a
shell_LDFLAGS=-lfl
shell_CXXFLAGS = -Isrc

src/parser/parser.tab.h : src/parser/parser.y

src/parser/parser.c : src/parser/parser.y
	$(YACC) src/parser/parser.y -o src/parser/parser.c

src/parser/lexer.c : src/parser/parser.tab.h
