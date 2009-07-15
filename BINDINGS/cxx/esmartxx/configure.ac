dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(esmartxx, 0.1)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC






AM_PROG_LIBTOOL






PKG_CHECK_MODULES(EFL, eflxx evasxx edjexx)
PKG_CHECK_MODULES(ESMART_TEXTENTRY, esmart_text_entry)
PKG_CHECK_MODULES(ESMART_CONTAINER, esmart_container)
AC_OUTPUT([
esmartxx_container.pc
esmartxx_container-uninstalled.pc
esmartxx_group.pc
esmartxx_group-uninstalled.pc
esmartxx_textentry.pc
esmartxx_textentry-uninstalled.pc
Makefile
src/Makefile
src/container/Makefile
src/group/Makefile
src/textentry/Makefile
])
