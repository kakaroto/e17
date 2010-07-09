dnl Process this file with autoconf to produce a configure script.

rm -f config.cache

AC_INIT([exchange], [0.0.0.002], [mcalamelli@gmail.com])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
AM_CONFIG_HEADER([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

VMAJ=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $1);}'`
VMIN=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $2);}'`
VMIC=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $3);}'`
SNAP=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $4);}'`
version_info=`expr $VMAJ + $VMIN`":$VMIC:$VMIN"
AC_SUBST(version_info)
AC_SUBST(VMAJ)

### Needed information

requirements_exchange=""


### Additional options to configure

# Etk
AC_ARG_ENABLE([etk],
   [AC_HELP_STRING([--disable-etk], [Disable ETK support])],
   [enable_etk=$enableval],
   [enable_etk="yes"])

if test "x${enable_etk}" = "xyes" ; then
   PKG_CHECK_MODULES([ETK], [etk], [], [enable_etk="no"])
   AC_DEFINE([HAVE_ETK], [1], [Set to 1 if ETK is installed])
fi

AM_CONDITIONAL([BUILD_ETK], [test "x${enable_etk}" = "xyes"])

# Ewl
AC_ARG_ENABLE([ewl],
   [AC_HELP_STRING([--disable-ewl], [Disable EWL support])],
   [enable_ewl=$enableval],
   [enable_ewl="yes"])

if test "x${enable_ewl}" = "xyes" ; then
   PKG_CHECK_MODULES([EWL], [ewl], [], [enable_ewl="no"])
   AC_DEFINE([HAVE_EWL], [1], [Set to 1 if EWL is installed])
fi

AM_CONDITIONAL([BUILD_EWL], [test "x${enable_ewl}" = "xyes"])


### Checks for programs
AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST(pkgconfig_requires_private)


### Checks for libraries

PKG_CHECK_MODULES([EXCHANGE],
   [eina-0 ecore ecore-evas ecore-file edje libxml-2.0])

requirements_exchange="eina-0 ecore ecore-evas ecore-file edje libxml-2.0"
AC_SUBST(requirements_exchange)


### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics
AC_ISC_POSIX
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST

EFL_EXCHANGE_BUILD=""
EXCHANGE_CPPFLAGS=""
case "$host_os" in
   mingw32ce*)
      EFL_EXCHANGE_BUILD="-DEFL_EXCHANGE_BUILD"
      EXCHANGE_CPPFLAGS="-D_WIN32_WCE=0x0420"
      ;;
   mingw*)
      EFL_EXCHANGE_BUILD="-DEFL_EXCHANGE_BUILD"
      EXCHANGE_CPPFLAGS="-D_WIN32_WINNT=0x0501"
      ;;
esac
AC_SUBST(EXCHANGE_CPPFLAGS)
AC_SUBST(EFL_EXCHANGE_BUILD)


### Checks for linker characteristics

# use --enable-auto-import on Windows

lt_enable_auto_import=""
case "$host_os" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
      ;;
esac
AC_SUBST(lt_enable_auto_import)

AC_OUTPUT([
exchange.pc
Makefile
src/Makefile
src/lib/Makefile
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
echo "  Etk..................: ${enable_etk}"
echo "  Ewl..................: ${enable_ewl}"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
