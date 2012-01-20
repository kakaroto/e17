AC_INIT([ekbd], [0.0.1], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
AM_CONFIG_HEADER([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

### Needed information

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

### Checks for compiler characteristics
AC_USE_SYSTEM_EXTENSIONS
AC_ISC_POSIX
AC_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_C___ATTRIBUTE__
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

### Checks for libraries

PKG_CHECK_MODULES([EKBD], [evas > 1.0.0 eina > 1.0.0 ecore > 1.0.0 ecore-file > 1.0.0 ecore-x > 1.0.0 evas > 1.0.0 edje > 1.0.0 x11])
PKG_CHECK_MODULES([EKBD_TEST], [ecore > 1.0.0 evas > 1.0.0 elementary])

### Checks for header files

### Checks for types


### Checks for structures
EFL_COMPILER_FLAG([-Wall])
EFL_COMPILER_FLAG([-W])
EFL_COMPILER_FLAG([-Wshadow])
EFL_COMPILER_FLAG([-Wmissing-declarations])


### Checks for linker characteristics


### Checks for library functions
AC_FUNC_MALLOC
AC_CHECK_FUNCS(strndup)


AC_OUTPUT([
Makefile
ekbd.pc
ekbd.spec
src/Makefile
src/lib/Makefile
src/bin/Makefile
keyboards/Makefile
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

