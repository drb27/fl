ACLOCAL_AMFLAGS=-I m4
SUBDIRS=.
bin_PROGRAMS=shell
noinst_LIBRARIES = libparse.a
BUILT_SOURCES = src/parser/parser.tab.h
libparse_a_SOURCES=src/parser/lexer.l src/parser/parser.y
shell_SOURCES=src/shell/shell.cpp
shell_LDADD=libparse.a
shell_LDFLAGS=-lfl
shell_CXXFLAGS = -Isrc

src/parser/parser.tab.h : src/parser/parser.y
