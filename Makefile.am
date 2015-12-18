CXX=g++
ACLOCAL_AMFLAGS=-I m4
SUBDIRS=.
CLEANFILES=src/parser/parser.cpp src/parser/lexer.cpp src/parser/parser.h
AM_YFLAGS=-d
bin_PROGRAMS=shell
noinst_LIBRARIES = libparse.a
libparse_a_SOURCES=src/parser/lexer.cpp src/parser/parser.cpp
libparse_a_DEPENDENCIES=src/parser/lexer.l src/parser/parser.y
shell_SOURCES=src/shell/shell.cpp
shell_LDADD=libparse.a
shell_LDFLAGS=-lfl
shell_CXXFLAGS = -Isrc


src/parser/parser.cpp : src/parser/parser.y
	$(YACC) $(AM_YFLAGS) src/parser/parser.y -o src/parser/parser.cpp
	mv src/parser/parser.hpp src/parser/parser.h

src/parser/lexer.cpp : src/parser/lexer.l src/parser/parser.cpp
	$(LEX) $(AM_LFLAGS) -o src/parser/lexer.cpp src/parser/lexer.l

