dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(evasxx, 0.1)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC






AM_PROG_LIBTOOL






PKG_CHECK_MODULES(EFL, evas eflxx)
PKG_CHECK_MODULES(SIGC, sigc++-2.0)
AC_OUTPUT([
Makefile
include/Makefile
include/evasxx/Makefile
src/Makefile
evasxx.pc
evasxx-uninstalled.pc
])
