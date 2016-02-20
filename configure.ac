AC_PREREQ([2.69])
AC_INIT([fl],[0.0.1],[david.bradshaw.usa@gmail.com])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/shell/shell.cpp])
AC_CONFIG_HEADERS([src/inc/config.h])

AC_PROG_CXX
AM_PROG_AR
AC_PROG_LIBTOOL
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_MKDIR_P

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([chdir getcwd memset strtol])

AC_CHECK_LIB([cppunit],[main])
AC_CONFIG_FILES([Makefile Doxyfile])
AC_CONFIG_FILES([fl_tests.sh],[chmod +x fl_tests.sh])
AC_OUTPUT
