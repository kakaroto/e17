# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT([e_dbus], [0.5.0.063], [enlightenment-devel@lists.sourceforge.net])
release="ver-pre-svn-05"
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
AM_CONFIG_HEADER([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC
AC_HEADER_STDC
AM_PROG_CC_C_O
AC_C___ATTRIBUTE__

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

VMAJ=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $1);}'`
VMIN=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $2);}'`
VMIC=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $3);}'`
SNAP=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $4);}'`
version_info=`expr $VMAJ + $VMIN`":$VMIC:$VMIN"
econnman_release_info="-release $release"
ebluez_release_info="-release $release"
edbus_release_info="-release $release"
ehal_release_info="-release $release"
enotify_release_info="-release $release"
AC_SUBST(version_info)
AC_SUBST(econnman_release_info)
AC_SUBST(ebluez_release_info)
AC_SUBST(edbus_release_info)
AC_SUBST(ehal_release_info)
AC_SUBST(enotify_release_info)

dnl Check enabled modules to build
AC_ARG_ENABLE([ehal],
   [AC_HELP_STRING([--disable-ehal], [Disable ehal build])],
   [enable_ehal=$enableval],
   [enable_ehal="yes"])

AC_ARG_ENABLE([econnman],
   [AC_HELP_STRING([--enable-econnman], [Enable econnman build])],
   [enable_econnman=$enableval],
   [enable_econnman="yes"])

AC_ARG_ENABLE([ebluez],
   [AC_HELP_STRING([--enable-ebluez], [Enable ebluez build])],
   [enable_ebluez=$enableval],
   [enable_ebluez="yes"])

AC_ARG_ENABLE([enotify],
   [AC_HELP_STRING([--disable-enotify], [Disable enotify build])],
   [enable_enotify=$enableval],
   [enable_enotify="yes"])

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST(pkgconfig_requires_private)

PKG_CHECK_MODULES([EINA], [eina-0])

PKG_CHECK_MODULES([EDBUS], [
  dbus-1 >= 0.62
  ecore >= 0.9.9
])

##### Find out the version of DBUS we're using
dbus_version=`pkg-config --modversion dbus-1`
DBUS_VERSION_MAJOR=`echo $dbus_version | awk -F. '{print $1}'`
DBUS_VERSION_MINOR=`echo $dbus_version | awk -F. '{print $2}'`
DBUS_VERSION_MICRO=`echo $dbus_version | awk -F. '{print $3}'`
if test "z$DBUS_VERSION_MAJOR" = "z"; then
  DBUS_VERSION_MAJOR="0"
fi
if test "z$DBUS_VERSION_MINOR" = "z"; then
  DBUS_VERSION_MINOR="0"
fi
if test "z$DBUS_VERSION_MICRO" = "z"; then
  DBUS_VERSION_MICRO="0"
fi
DBUS_VERSION_CFLAGS="$DBUS_VERSION_CFLAGS -DDBUS_VERSION_MAJOR=$DBUS_VERSION_MAJOR"
DBUS_VERSION_CFLAGS="$DBUS_VERSION_CFLAGS -DDBUS_VERSION_MINOR=$DBUS_VERSION_MINOR"
DBUS_VERSION_CFLAGS="$DBUS_VERSION_CFLAGS -DDBUS_VERSION_MICRO=$DBUS_VERSION_MICRO"
AC_SUBST(DBUS_VERSION_CFLAGS)

AC_FUNC_ALLOCA

dnl evas
if test "x${enable_enotify}" = "xyes" ; then
   PKG_CHECK_MODULES(EVAS, [evas >= 0.9.9], [], [enable_enotify="no"])
fi

AC_ARG_ENABLE([build-test-gui],
   [AS_HELP_STRING([--enable-build-test-gui], [Build with gui for testing])],
   [PKG_CHECK_MODULES([HAL_TEST_GUI],
       [ewl efreet],
    # temporarily disable until update to newer tree2 api
    [AC_DEFINE([EWL_GUI], 0, [An ewl gui for e_dbus_hal])],
    [AC_DEFINE([EWL_GUI], 0, [An ewl gui for e_dbus_hal])])
   ])

AM_CONDITIONAL([BUILD_EHAL],     [test "x${enable_ehal}"     = "xyes"])
AM_CONDITIONAL([BUILD_ENOTIFY],  [test "x${enable_enotify}"  = "xyes"])
AM_CONDITIONAL([BUILD_ECONNMAN], [test "x${enable_econnman}" = "xyes"])
AM_CONDITIONAL([BUILD_EBLUEZ], [test "x${enable_ebluez}" = "xyes"])

EFL_EDBUS_BUILD=""
lt_enable_auto_import=""
case "$host_os" in
   mingw* | cegcc*)
      EFL_EDBUS_BUILD="-DEFL_EDBUS_BUILD"
      lt_enable_auto_import="-Wl,--enable-auto-import"
   ;;
esac
AC_SUBST(EFL_EDBUS_BUILD)
AC_SUBST(lt_enable_auto_import)

AC_OUTPUT([
e_dbus.spec
Makefile
src/Makefile
src/lib/Makefile
src/lib/dbus/Makefile
src/lib/hal/Makefile
src/lib/notification/Makefile
src/lib/connman/Makefile
src/lib/bluez/Makefile
src/bin/Makefile
edbus.pc
ehal.pc
enotify.pc
econnman.pc
ebluez.pc
])


#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  Modules:"
echo
echo "    EHal...............: $enable_ehal"
echo "    EConnman...........: $enable_econnman"
echo "    EBluez.............: $enable_ebluez"
echo "    ENotify............: $enable_enotify"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
