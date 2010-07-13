# FIXME:
#  * what about USE_PAM and pam not found ?
#  * DISPLAY should not be set here but in source instead
#  * what about LOCALEDIR ?

AC_INIT([elsa], [0.0.1], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
AM_CONFIG_HEADER([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

m4_ifdef([AM_GNU_GETTEXT_VERSION], [
AM_GNU_GETTEXT_VERSION([0.12.1])
])

m4_ifdef([AM_GNU_GETTEXT], [
AM_GNU_GETTEXT([external])
po_makefile_in=po/Makefile.in
AM_CONDITIONAL([HAVE_PO], [true])
],[
AM_CONDITIONAL([HAVE_PO], [false])
])
AC_SUBST(LTLIBINTL)


### Needed information

# Set the localedir var for gettext
AC_DEFINE([LOCALEDIR], ["/usr/share/locale/"], ["locales"])

# Set DISPLAY in config.h.
display=":0.0"
AC_DEFINE_UNQUOTED(DISPLAY, "${display}", [Display to use])

# Set USE_PAM in config.h.
usepam=""
AC_DEFINE_UNQUOTED(USE_PAM, "${use_pam}", [Use pam authentification])


### Additional options to configure

# Set edje_cc path
AC_ARG_WITH([edje-cc],
   [AC_HELP_STRING([ --with-edje-cc=PATH], [specify a specific path to edje_cc])],
   [
    v=$withval;
    edje_cc=$v
    echo "  Enlightenment edje_cc explicitly set to "$edje_cc;
   ],
   [edje_cc=$(pkg-config --variable=prefix edje)/bin/edje_cc])
AC_SUBST(edje_cc)


### Checks for programs
AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG


### Checks for libraries

PKG_CHECK_MODULES([ELSA], [elementary >= 0.5.0.0])

AC_CHECK_LIB([pam], [pam_start],
   [dummy="yes"],
   [AC_MSG_ERROR([libpam not found.])])


### Checks for header files

# Pam
AC_CHECK_HEADER([security/pam_appl.h],
   [dummy="yes"],
   [AC_MSG_ERROR("Cannot find security/pam_appl.h. Make sure your CPPFLAGS environment variable contains include lines for the location of this file")])


### Checks for types


### Checks for structures


### Checks for compiler characteristics
AC_ISC_POSIX
AC_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_GNU_SOURCE
AC_C___ATTRIBUTE__

EFL_COMPILER_FLAG([-Wall])
EFL_COMPILER_FLAG([-W])
EFL_COMPILER_FLAG([-Wshadow])


### Checks for linker characteristics


### Checks for library functions
AC_FUNC_MALLOC
AC_CHECK_FUNCS(strndup)


AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
data/themes/Makefile
])


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
echo "  edje_cc..............: ${edje_cc}"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo


