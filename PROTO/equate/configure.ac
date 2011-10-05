dnl Process this file with autoconf to produce a configure script.

### Version

m4_define([v_maj], [0])
m4_define([v_min], [99])
m4_define([v_mic], [0])

m4_define([evil_version], [v_maj.v_min.v_mic])

AC_INIT([equate], [evil_version], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])


### Checks for programs

AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG


### Checks for libraries

PKG_CHECK_MODULES([EQUATE], [elementary edje ecore-evas ecore evas >= 1.0.0])


### Checks for compiler characteristics

AC_HEADER_STDC


EFL_WITH_BIN([edje], [edje-cc], [edje_cc])

AC_CONFIG_FILES([
Makefile
src/Makefile
data/Makefile
data/images/Makefile
])

AC_OUTPUT


#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  OS...................: ${host_os}"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
