ACLOCAL_AMFLAGS=-I m4
SUBDIRS=.
CLEANFILES=src/parser/parser.cpp src/parser/lexer.cpp src/parser/parser.h
AM_YFLAGS=-d
AM_CXXFLAGS=-std=c++14 -Isrc
bin_PROGRAMS=shell
noinst_LIBRARIES=liblang.a
liblang_a_SOURCES=src/parser/lexer.cpp \
			src/parser/parser.cpp \
			src/interpreter/class.cpp \
			src/interpreter/object.cpp \
			src/interpreter/typemgr.cpp \
			src/interpreter/context.cpp \
			src/interpreter/builtins.cpp \
			src/interpreter/dat.cpp \
			src/interpreter/eval_exception.cpp \
			src/interpreter/marshall.cpp \
			src/logger/logger.cpp \
			src/interpreter/smartlist.cpp \
			src/interpreter/optimization.cpp \
			src/parser/rawfn.cpp \
			src/parser/ast/ast.cpp \
			src/parser/ast/literal.cpp \
			src/parser/ast/list.cpp \
			src/parser/ast/methodcall.cpp \
			src/parser/ast/attr.cpp \
			src/parser/ast/symbol.cpp \
			src/parser/ast/assign.cpp \
			src/parser/ast/fundef.cpp \
			src/parser/ast/funcall.cpp \
			src/parser/ast/if.cpp \
			src/parser/ast/sequence.cpp \
			src/parser/ast/pair.cpp \
			src/parser/ast/selector.cpp \
			src/parser/ast/while.cpp \
			src/parser/ast/enum.cpp \
			src/parser/ast/classdef.cpp

liblang_a_DEPENDENCIES=src/parser/lexer.l src/parser/parser.y

shell_SOURCES=src/shell/shell.cpp
shell_LDADD=liblang.a
shell_LDFLAGS=-lfl

TESTS=interpreter_tests
check_PROGRAMS=interpreter_tests
interpreter_tests_SOURCES=src/interpreter/tests/tests.cpp \
				src/interpreter/tests/test-fclass.cpp \
				src/interpreter/tests/test-object.cpp \
				src/interpreter/tests/test-typemgr.cpp \
				src/interpreter/tests/test-context.cpp \
				src/interpreter/tests/test-parser.cpp \
				src/interpreter/tests/test-smartlist.cpp

interpreter_tests_LDADD=liblang.a

src/parser/parser.cpp : src/parser/parser.y
	$(YACC) $(AM_YFLAGS) src/parser/parser.y -o src/parser/parser.cpp --defines=src/parser/bison.h

src/parser/lexer.cpp : src/parser/lexer.l src/parser/parser.cpp
	$(LEX) $(AM_LFLAGS) --header-file=src/parser/flex.h -o src/parser/lexer.cpp src/parser/lexer.l
