dnl Process this file with autoconf to produce a configure script.

m4_define([v_maj], [0])
m4_define([v_min], [1])
m4_define([v_mic], [0])
m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])

AC_INIT([ecrire], [v_ver], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# GETTEXT stuff
m4_ifdef([AM_GNU_GETTEXT_VERSION], [
AM_GNU_GETTEXT_VERSION([0.18])
])

m4_ifdef([AM_GNU_GETTEXT], [
AM_GNU_GETTEXT([external])
po_makefile_in=po/Makefile.in
AM_CONDITIONAL([HAVE_PO], [true])
],[
AM_CONDITIONAL([HAVE_PO], [false])
])
AC_SUBST([LTLIBINTL])


### Needed information

AC_CANONICAL_BUILD
AC_CANONICAL_HOST


### Checks for programs
AC_PROG_CC_STDC

# pkg-config
PKG_PROG_PKG_CONFIG

### Checks for libraries

PKG_CHECK_MODULES([EFL],
   [
    elementary >= 0.7.0
    edje >= 1.1.0
    ecore >= 1.0.0
    efreet >= 1.0.0
   ])

have_ecore_x="no"
PKG_CHECK_MODULES([ECORE_X],
   [ecore-x >= 1.0.0],
   [
    AC_DEFINE(HAVE_ECORE_X, 1, [X11 support])
    have_ecore_x="yes"
   ])

### Checks for header files

AC_CHECK_HEADERS([unistd.h])

### Checks for types


### Checks for structures


### Checks for compiler characteristics

AC_HEADER_STDC
EFL_ATTRIBUTE_UNUSED


### Checks for linker characteristics

AC_CONFIG_FILES([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
$po_makefile_in
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
echo "  X Window support.....: ${have_ecore_x}"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
