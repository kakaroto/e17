# get rid of that stupid cache mechanism
rm -f config.cache

m4_define([v_maj], [0])
m4_define([v_min], [1])
m4_define([v_mic], [0])
m4_define([v_ver], [v_maj.v_min.v_mic])

AC_INIT([espionnage], [v_ver])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_GNU_SOURCE
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

#m4_ifdef([AM_GNU_GETTEXT_VERSION], [
#AM_GNU_GETTEXT_VERSION([0.12.1])
#])

#m4_ifdef([AM_GNU_GETTEXT], [
#AM_GNU_GETTEXT([external])
#po_makefile_in=po/Makefile.in
#AM_CONDITIONAL([HAVE_PO], [true])
#],[
#AM_CONDITIONAL([HAVE_PO], [false])
#])

AC_SUBST(LTLIBINTL)


### Needed information

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

### Default options with respect to host


### Additional options to configure


### Checks for programs
AC_PROG_CXX

# pkg-config
PKG_PROG_PKG_CONFIG


### Checks for libraries

EFL_WITH_BIN([edje], [edje-cc], [edje_cc])

PKG_CHECK_MODULES([ESPIONNAGE],
   [
    eina >= 1.0.0
    evas >= 1.0.0
    ecore >= 1.0.0
    eet >= 1.2.2
    edje >= 1.0.0
    elementary >= 0.5.1
    emotion >= 0.1.0
   ])

## http://libface.sourceforge.net
have_libface="no"
PKG_CHECK_MODULES([FACE], [libface], [have_libface="yes"], [have_libface="no"])
PKG_CHECK_MODULES([FACE2], [libface >= 0.2], [have_libface2="yes"], [have_libface2="no"])

if test "x${have_libface}" = "xyes"; then
   AC_DEFINE(HAVE_FACE, 1, [Define to 1 if libface is available])
fi

if test "x${have_libface2}" = "xyes"; then
   AC_DEFINE(HAVE_FACE2, 1, [Define to 1 if libface >= 0.2 is available])
fi

### Checks for compiler characteristics

AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_PROG_CC_C_O
AC_C___ATTRIBUTE__

AC_CONFIG_FILES([
Makefile
espionnage.desktop
src/Makefile
src/bin/Makefile
$po_makefile_in
])

AC_OUTPUT

echo
echo
echo "--------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "--------------------------------------------------------"
echo
echo "  Face detection...........: ${have_libface}"
echo
echo "  Installation.............: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...................: $prefix"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
