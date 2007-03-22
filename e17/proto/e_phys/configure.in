AC_PREREQ(2.59)
AC_INIT([e_phys], [0.1], [rephorm@rephorm.com])
AM_INIT_AUTOMAKE([1.6 foreign])

AC_PROG_CC
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

AC_HEADER_STDC
AC_CHECK_HEADERS(math.h)

PKG_CHECK_MODULES(EPHYS, [
  evas
  ecore
  ecore-evas
])

AC_OUTPUT([
Makefile
ephys.pc
src/Makefile
src/lib/Makefile
src/bin/Makefile
])
