AC_INIT([ecore_unit_tests], [0.1])
AC_PREREQ([2.54])

AM_INIT_AUTOMAKE

AC_PROG_CC
AM_PROG_CC_C_O

PKG_CHECK_MODULES(ECORE_UNIT_TESTS, ecore-file >= 0.9.9 check >= 0.9.4)

AC_CONFIG_FILES([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
])

AC_OUTPUT
