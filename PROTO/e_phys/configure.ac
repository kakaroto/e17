AC_PREREQ(2.59)
AC_INIT([e_phys], [0.1], [rephorm@rephorm.com])
AM_INIT_AUTOMAKE([1.6 foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_PROG_LIBTOOL

AC_CANONICAL_HOST

EFL_EPHYS_BUILD=""
case "$host_os" in
   mingw*)
      EFL_EPHYS_BUILD="-DEFL_EPHYS_BUILD"
   ;;
esac
AC_SUBST([EFL_EPHYS_BUILD])

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
