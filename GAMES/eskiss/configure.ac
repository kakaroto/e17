m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [2])
m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])

AC_INIT([eskiss], [v_ver], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
AM_CONFIG_HEADER([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

### Default options with respect to host

case "$host_os" in
   mingw*)
      want_gnutls="no"
      want_openssl="no"
      want_cipher="no"
      want_signature="no"
      ;;
   *)
      want_gnutls="auto"
      want_openssl="auto"
      want_cipher="yes"
      want_signature="yes"
      ;;
esac

### Additional options to configure

AC_ARG_WITH([edje-cc],
   [AC_HELP_STRING([--with-edje-cc=PATH], [specify a specific path to edje_cc])],
   [edje_cc=$withval],
   [edje_cc=$(pkg-config --variable=prefix edje)/bin/edje_cc])
AC_SUBST(edje_cc)

### Checks for programs
AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

# doxygen program for documentation building

EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

### Checks for libraries

# Eina library

PKG_CHECK_MODULES(ESKISS, [edje ecore-evas ecore-file ecore evas eet eina])
PKG_CHECK_MODULES(ECORE_X, [ecore-x], [have_ecore_x="yes"], [have_ecore_x="no"])

AM_CONDITIONAL(HAVE_ECORE_X, test "x$have_ecore_x" = "xyes")
if test "x$have_ecore_x" = "xyes"; then
   AC_DEFINE(HAVE_ECORE_X, 1, [Ecore-x available])
fi

### Checks for header files

AC_CHECK_HEADER([chipmunk/chipmunk.h],
   [dummy="yes"],
   [AC_MSG_ERROR("Cannot find chipmunk.h. Make sure your CFLAGS environment variable contains include lines for the location of this file")])


### Checks for types


### Checks for structures


### Checks for compiler characteristics
AM_PROG_CC_C_O
AC_C_CONST
AC_C_INLINE
AC_PROG_CC_STDC
EFL_CHECK_PATH_MAX

### Checks for linker characteristics

# use --enable-auto-import on Windows

lt_enable_auto_import=""
case "$host_os" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
      ;;
esac
AC_SUBST(lt_enable_auto_import)

# use --as-needed if supported

LDFLAGS_SAVE=${LDFLAGS}
LDFLAGS="${LDFLAGS} -Wl,--as-needed"

AC_LINK_IFELSE(
   [AC_LANG_PROGRAM([[]], [])],
   [ld_as_needed="-Wl,--as-needed"],
   [ld_as_needed=""]
)

LDFLAGS=${LDFLAGS_SAVE}
AC_SUBST(ld_as_needed)


### Checks for library functions

AC_CHECK_LIB([chipmunk], [cpInitChipmunk],
   [ESKISS_LIBS="${ESKISS_LIBS} -lchipmunk"],
   [AC_MSG_ERROR("Cannot find cpInitChipmunk() in libchipmunk. Make sure your LDFLAGS environment variable contains include lines for the location of this file")],
   [-lm])

AC_OUTPUT([
Makefile
data/Makefile
data/levels/Makefile
data/edje/Makefile
src/Makefile
src/bin/Makefile
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
echo "  Documentation........: ${build_doc}"
if test "x${build_doc}" = "xyes" ; then
   echo "    Building...........: make doc"
fi
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
