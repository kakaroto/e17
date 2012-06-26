AC_INIT(clouseau, 0.1.0, tom@stosb.com)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_CONFIG_HEADER(config.h)

AC_GNU_SOURCE
AC_PROG_LIBTOOL

PKG_CHECK_MODULES([EFL],
  [
   elementary >= 0.7.0
   evas >= 1.0.0
   ecore >= 1.0.0
   ecore-ipc >= 1.0.0
   edje >= 1.0.0
  ])

EFL_ATTRIBUTE_UNUSED

AC_CHECK_LIB(dl,dlopen)
AC_CHECK_FUNCS(dlopen dlerror)

AC_CONFIG_FILES([
Makefile
src/Makefile
src/lib/Makefile
src/bin/Makefile
src/scripts/Makefile
data/Makefile
data/images/Makefile
])

AC_OUTPUT
