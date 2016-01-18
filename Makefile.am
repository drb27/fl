ACLOCAL_AMFLAGS=-I m4
SUBDIRS=.
CLEANFILES=src/parser/parser.cpp src/parser/lexer.cpp src/parser/parser.h
AM_YFLAGS=-d
AM_CXXFLAGS=-std=c++14 -Isrc
bin_PROGRAMS=shell
noinst_LIBRARIES=liblang.a
liblang_a_SOURCES=src/parser/lexer.cpp \
			src/parser/parser.cpp \
			src/parser/ast.cpp \
			src/interpreter/class.cpp \
			src/interpreter/package.cpp \
			src/interpreter/object.cpp \
			src/interpreter/typemgr.cpp \
			src/interpreter/context.cpp \
			src/interpreter/builtins.cpp \
			src/interpreter/dat.cpp \
			src/interpreter/eval_exception.cpp \
			src/interpreter/marshall.cpp \
			src/logger/logger.cpp

liblang_a_DEPENDENCIES=src/parser/lexer.l src/parser/parser.y

shell_SOURCES=src/shell/shell.cpp
shell_LDADD=liblang.a
shell_LDFLAGS=-lfl

TESTS=interpreter_tests
check_PROGRAMS=interpreter_tests
interpreter_tests_SOURCES=src/interpreter/tests/tests.cpp \
				src/interpreter/tests/test-fclass.cpp \
				src/interpreter/tests/test-nametable.cpp \
				src/interpreter/tests/test-package.cpp \
				src/interpreter/tests/test-object.cpp \
				src/interpreter/tests/test-typemgr.cpp \
				src/interpreter/tests/test-context.cpp \
				src/interpreter/tests/test-parser.cpp

interpreter_tests_LDADD=liblang.a

src/parser/parser.cpp : src/parser/parser.y
	$(YACC) $(AM_YFLAGS) src/parser/parser.y -o src/parser/parser.cpp
	mv src/parser/parser.hpp src/parser/parser.h

src/parser/lexer.cpp : src/parser/lexer.l src/parser/parser.cpp
	$(LEX) $(AM_LFLAGS) -o src/parser/lexer.cpp src/parser/lexer.l
