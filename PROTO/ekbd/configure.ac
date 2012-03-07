
### Version

m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [1])

m4_define([ekbd_version], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([ekbd], [ekbd_version], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_USE_SYSTEM_EXTENSIONS

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_PREREQ([2.2])
LT_INIT([win32-dll])
version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])

### Needed information

AC_CANONICAL_HOST


### Additional options to configure

# X11
EKBD_X11_DEF="#undef"
AC_ARG_ENABLE([x11],
   [AC_HELP_STRING([--disable-x11], [disable x11 support. @<:@default=enabled@:>@])],
   [
   if test "x{enabled}" = "xyes" ; then
      enable_x11="yes"
   else
      enable_x11="no"
   fi
   ],
   [enable_x11="yes"])
AC_MSG_CHECKING([whether to support x11])
AC_MSG_RESULT([${enable_x11}])

### Checks for programs

AC_PROG_CC_STDC

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST([pkgconfig_requires_private])

EFL_WITH_BIN([edje], [edje-cc], [edje_cc])


### Checks for compiler characteristics
AM_PROG_CC_C_O
EFL_ATTRIBUTE_UNUSED

EFL_COMPILER_FLAG([-Wall])
EFL_COMPILER_FLAG([-W])
EFL_COMPILER_FLAG([-Wshadow])
EFL_COMPILER_FLAG([-Wmissing-declarations])

### Checks for libraries

requirement_ekbd="evas > 1.0.0 eina > 1.0.0 ecore > 1.0.0 ecore-file > 1.0.0 ecore-x > 1.0.0 evas > 1.0.0 edje > 1.0.0 x11"
PKG_CHECK_MODULES([EKBD], [${requirement_ekbd}])
PKG_CHECK_MODULES([EKBD_TEST], [ecore > 1.0.0 evas > 1.0.0 elementary])
AC_SUBST([requirement_ekbd])

# X11
if test "a${enable_x11}" = "ayes" ; then
   PKG_CHECK_MODULES([EKBD_X11], [ecore-x > 1.0.0], [enable_x11="yes"], [enable_x11="no"])
fi

AM_CONDITIONAL(HAVE_X11, [test "a${enable_x11}" = "ayes"])
if test "a${enable_x11}" = "ayes" ; then
   AC_DEFINE([HAVE_X11], [1], [Set to 1 if x11 is enabled.])
   EKBD_X11_DEF="#define"
fi
AC_SUBST(EKBD_X11_DEF)

### Checks for header files


### Checks for types


### Checks for structures


### Checks for linker characteristics
lt_enable_auto_import=""
case "${host_os}" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
   ;;
esac
AC_SUBST([lt_enable_auto_import])


### Checks for library functions


AC_OUTPUT([
Makefile
ekbd.pc
ekbd.spec
src/Makefile
src/lib/Makefile
src/lib/Ekbd.h
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
echo "  x11..................: ${enable_x11}"
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

