ACLOCAL_AMFLAGS=-I m4
AM_TESTS_ENVIRONMENT = \
		     PATH='$(top_builddir)':$$PATH; \
		     export PATH;
CLEANFILES=src/parser/parser.cpp \
	src/parser/lexer.cpp \
	src/parser/flex.h \
	src/parser/bison.h

EXTRA_DIST=src/parser/lexer.l \
	   src/parser/parser.y

AM_YFLAGS=-d
AM_CXXFLAGS=-std=c++14 -I$(top_builddir)/src -I$(top_srcdir)/src
bin_PROGRAMS=fl
noinst_LIBRARIES=liblang.a

liblang_a_SOURCES=src/parser/lexer.cpp \
			src/parser/parser.cpp \
			src/interpreter/class.cpp \
			src/interpreter/class.h \
			src/interpreter/obj/object.cpp \
			src/interpreter/obj/int_object.cpp \
			src/interpreter/obj/class_object.cpp \
			src/interpreter/obj/string_object.cpp \
			src/interpreter/obj/bool_object.cpp \
			src/interpreter/obj/list_object.cpp \
			src/interpreter/obj/void_object.cpp \
			src/interpreter/obj/fn_object.cpp \
			src/interpreter/obj/enum_object.cpp \
			src/interpreter/obj/float_object.cpp \
			src/interpreter/obj/lazy_object.cpp \
			src/interpreter/obj/object.h \
			src/interpreter/obj/int_object.h \
			src/interpreter/obj/class_object.h \
			src/interpreter/obj/string_object.h \
			src/interpreter/obj/bool_object.h \
			src/interpreter/obj/list_object.h \
			src/interpreter/obj/void_object.h \
			src/interpreter/obj/fn_object.h \
			src/interpreter/obj/enum_object.h \
			src/interpreter/obj/float_object.h \
			src/interpreter/obj/lazy_object.h \
			src/interpreter/obj/foundation.h \
			src/interpreter/obj/all.h \
			src/interpreter/typemgr.cpp \
			src/interpreter/typemgr.h \
			src/interpreter/context.cpp \
			src/interpreter/context.h \
			src/interpreter/package.cpp \
			src/interpreter/package.h \
			src/interpreter/builtins.cpp \
			src/interpreter/builtins.h \
			src/interpreter/dat.cpp \
			src/interpreter/dat.h \
			src/interpreter/eval_exception.cpp \
			src/interpreter/eval_exception.h \
			src/interpreter/marshall.cpp \
			src/logger/logger.cpp \
			src/logger/logger.h \
			src/interpreter/factory.cpp \
			src/interpreter/factory.h \
			src/interpreter/smartlist.cpp \
			src/interpreter/smartlist.h \
			src/interpreter/optimization.cpp \
			src/interpreter/optimization.h \
			src/interpreter/marshall.h \
			src/inc/references.h \
			src/parser/rawfn.cpp \
			src/parser/rawfn.h \
			src/parser/ast/ast.cpp \
			src/parser/ast/ast.h \
			src/parser/ast/nodes.h \
			src/parser/ast/literal.cpp \
			src/parser/ast/list.cpp \
			src/parser/ast/methodcall.cpp \
			src/parser/ast/attr.cpp \
			src/parser/ast/symbol.cpp \
			src/parser/ast/assign.cpp \
			src/parser/ast/fundef.cpp \
			src/parser/ast/funcall.cpp \
			src/parser/ast/if.cpp \
			src/parser/ast/lvalue.h \
			src/parser/ast/sequence.cpp \
			src/parser/ast/pair.cpp \
			src/parser/ast/selector.cpp \
			src/parser/ast/while.cpp \
			src/parser/ast/enum.cpp \
			src/parser/ast/classdef.cpp \
			src/parser/ast/literal.h \
			src/parser/ast/list.h \
			src/parser/ast/methodcall.h \
			src/parser/ast/attr.h \
			src/parser/ast/symbol.h \
			src/parser/ast/assign.h \
			src/parser/ast/fundef.h \
			src/parser/ast/funcall.h \
			src/parser/ast/if.h \
			src/parser/ast/sequence.h \
			src/parser/ast/pair.h \
			src/parser/ast/selector.h \
			src/parser/ast/while.h \
			src/parser/ast/enum.h \
			src/parser/ast/classdef.h \
			src/parser/action_target.h \
			src/parser/callable.h \
			src/parser/symspec.h \
			src/parser/symspec.cpp \
			src/interpreter/collection.h \
			src/interpreter/collection.cpp \
			src/inc/common.h

liblang_a_DEPENDENCIES=src/parser/lexer.l src/parser/parser.y

fl_SOURCES=src/shell/shell.cpp
fl_LDADD=liblang.a
fl_LDFLAGS=-lfl

TESTS=interpreter_tests fl_tests.sh
check_PROGRAMS=interpreter_tests
interpreter_tests_SOURCES=src/interpreter/tests/tests.cpp \
				src/interpreter/tests/test-fclass.cpp \
				src/interpreter/tests/test-object.cpp \
				src/interpreter/tests/test-typemgr.cpp \
				src/interpreter/tests/test-context.cpp \
				src/interpreter/tests/test-parser.cpp \
				src/interpreter/tests/test-smartlist.cpp \
				src/interpreter/tests/test-fclass.h \
				src/interpreter/tests/test-object.h \
				src/interpreter/tests/test-typemgr.h \
				src/interpreter/tests/test-context.h \
				src/interpreter/tests/test-parser.h \
				src/interpreter/tests/test-smartlist.h \
				src/interpreter/tests/test-package.h \
				src/interpreter/tests/test-package.cpp

interpreter_tests_LDADD=liblang.a

$(top_builddir)/src/parser/parser.cpp $(top_builddir)/src/parser/bison.h: $(top_srcdir)/src/parser/parser.y
	$(YACC) $(AM_YFLAGS) $(top_srcdir)/src/parser/parser.y -o $(top_builddir)/src/parser/parser.cpp --defines=$(top_builddir)/src/parser/bison.h

$(top_builddir)/src/parser/lexer.cpp $(top_builddir)/src/parser/flex.h: $(top_srcdir)/src/parser/lexer.l $(top_builddir)/src/parser/parser.cpp $(top_builddir)/src/parser/bison.h
	$(LEX) $(AM_LFLAGS) --header-file=$(top_builddir)/src/parser/flex.h -o $(top_builddir)/src/parser/lexer.cpp $(top_srcdir)/src/parser/lexer.l
