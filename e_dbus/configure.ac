##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [1])
m4_define([v_min], [0])
m4_define([v_mic], [999])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v export || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--   When released, remove the dnl on the below line
dnl m4_undefine([v_rev])
##--   When doing snapshots - change soname. remove dnl on below line
dnl m4_define([relname], [ver-pre-svn-07])
dnl m4_define([v_rel], [-release relname])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], [m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])],
[m4_define([v_ver], [v_maj.v_min.v_mic])])
m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_INIT([e_dbus], [v_ver], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])
AH_TOP([
#ifndef EFL_CONFIG_H__
#define EFL_CONFIG_H__
])
AH_BOTTOM([
#endif /* EFL_CONFIG_H__ */
])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], , [m4_define([v_rev], [0])])
m4_ifdef([v_rel], , [m4_define([v_rel], [])])
AC_DEFINE_UNQUOTED(VMAJ, [v_maj], [Major version])
AC_DEFINE_UNQUOTED(VMIN, [v_min], [Minor version])
AC_DEFINE_UNQUOTED(VMIC, [v_mic], [Micro version])
AC_DEFINE_UNQUOTED(VREV, [v_rev], [Revison])
version_info="lt_cur:lt_rev:lt_age"
release_info="v_rel"
AC_SUBST(version_info)
AC_SUBST(release_info)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
VMAJ=v_maj
AC_SUBST(VMAJ)

### Needed information

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

want_ebluez="yes"
want_econnman="yes"
want_ehal="yes"
want_enotify="yes"
want_eofono="yes"
want_eukit="yes"

case "$host_os" in
   mingw*)
      want_ebluez="no"
      want_econnman="no"
      want_ehal="no"
      want_enotify="no"
      want_eofono="no"
      want_eukit="no"
      ;;
esac

requirement_ebluez="edbus >= 1.0.0"
requirement_econnman="edbus >= 1.0.0"
requirement_edbus="ecore >= 1.0.0 eina >= 1.0.0 dbus-1 >= 0.62"
requirement_ehal="edbus >= 1.0.0"
requirement_enotify="edbus >= 1.0.0"
requirement_eofono="edbus >= 1.0.0"
requirement_eukit="edbus >= 1.0.0"
requirement_dbus="dbus-1 >= 0.62"

### Additional options to configure

dnl Check enabled modules to build

AC_ARG_ENABLE([ebluez],
   [AC_HELP_STRING([--enable-ebluez], [Enable ebluez build])],
   [enable_ebluez=$enableval],
   [enable_ebluez="${want_ebluez}"])

AC_ARG_ENABLE([econnman],
   [AC_HELP_STRING([--enable-econnman], [Enable econnman build])],
   [enable_econnman=$enableval],
   [enable_econnman="${want_econnman}"])

AC_ARG_ENABLE([ehal],
   [AC_HELP_STRING([--disable-ehal], [Disable ehal build])],
   [enable_ehal=$enableval],
   [enable_ehal="${want_ehal}"])

AC_ARG_ENABLE([enotify],
   [AC_HELP_STRING([--disable-enotify], [Disable enotify build])],
   [enable_enotify=$enableval],
   [enable_enotify="${want_enotify}"])

AC_ARG_ENABLE([eofono],
   [AC_HELP_STRING([--enable-eofono], [Enable eofono build])],
   [enable_eofono=$enableval],
   [enable_eofono="${want_eofono}"])

AC_ARG_ENABLE([eukit],
   [AC_HELP_STRING([--disable-eukit], [Disable eukit build])],
   [enable_eukit=$enableval],
   [enable_eukit="${want_eukit}"])


### Checks for programs

AC_PROG_CC
AM_PROG_CC_C_O

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22 ; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST(pkgconfig_requires_private)

# doxygen program for documentation building
EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])


### Checks for libraries

PKG_CHECK_MODULES([EDBUS], [ecore >= 1.0.0 eina >= 1.0.0 dbus-1 >= 0.62])

# Find out the version of DBUS we're using
dbus_version=`pkg-config --modversion dbus-1`
DBUS_VERSION_MAJOR=`echo $dbus_version | awk -F. '{print $1}'`
DBUS_VERSION_MINOR=`echo $dbus_version | awk -F. '{print $2}'`
DBUS_VERSION_MICRO=`echo $dbus_version | awk -F. '{print $3}'`

if test "z$DBUS_VERSION_MAJOR" = "z" ; then
   DBUS_VERSION_MAJOR="0"
fi

if test "z$DBUS_VERSION_MINOR" = "z" ; then
   DBUS_VERSION_MINOR="0"
fi

if test "z$DBUS_VERSION_MICRO" = "z" ; then
   DBUS_VERSION_MICRO="0"
fi

DBUS_VERSION_CFLAGS="$DBUS_VERSION_CFLAGS -DDBUS_VERSION_MAJOR=$DBUS_VERSION_MAJOR"
DBUS_VERSION_CFLAGS="$DBUS_VERSION_CFLAGS -DDBUS_VERSION_MINOR=$DBUS_VERSION_MINOR"
DBUS_VERSION_CFLAGS="$DBUS_VERSION_CFLAGS -DDBUS_VERSION_MICRO=$DBUS_VERSION_MICRO"
AC_SUBST(DBUS_VERSION_CFLAGS)

# Dependencies for the libraries
if test "x${enable_enotify}" = "xyes" ; then
   PKG_CHECK_MODULES([EVAS],
      [evas >= 1.0.0],
      [requirement_enotify="evas >= 1.0.0 ${requirement_enotify}"],
      [enable_enotify="no"])
fi

AM_CONDITIONAL([BUILD_EBLUEZ],   [test "x${enable_ebluez}"   = "xyes"])
AM_CONDITIONAL([BUILD_ECONNMAN], [test "x${enable_econnman}" = "xyes"])
AM_CONDITIONAL([BUILD_EHAL],     [test "x${enable_ehal}"     = "xyes"])
AM_CONDITIONAL([BUILD_ENOTIFY],  [test "x${enable_enotify}"  = "xyes"])
AM_CONDITIONAL([BUILD_EOFONO],   [test "x${enable_eofono}"   = "xyes"])
AM_CONDITIONAL([BUILD_EUKIT],    [test "x${enable_eukit}"    = "xyes"])

# Dependencies for the binaries

EFL_ENABLE_BIN([edbus-test], ["yes"])
EFL_ENABLE_BIN([edbus-test-client], ["yes"])
EFL_ENABLE_BIN([edbus-bluez-test], [${enable_ebluez}])
EFL_ENABLE_BIN([edbus-connman-test], [${enable_econnman}])
EFL_ENABLE_BIN([edbus-notification-daemon-test], [${enable_enotify}])
EFL_ENABLE_BIN([edbus-notify-send], [${enable_enotify}])
EFL_ENABLE_BIN([edbus-notify-test], [${enable_enotify}])
EFL_ENABLE_BIN([edbus-ofono-test], [${enable_eofono}])
EFL_ENABLE_BIN([edbus-ukit-test], [${enable_eukit}])

if test "x${have_edbus_test}" = "xyes" ; then
   PKG_CHECK_MODULES([EDBUS_TEST],
      [ecore >= 1.0.0 dbus-1 >= 0.62],
      [have_edbus_test="yes"],
      [have_edbus_test="no"])
fi

if test "x${have_edbus_test_client}" = "xyes" ; then
   PKG_CHECK_MODULES([EDBUS_TEST_CLIENT],
      [ecore >= 1.0.0 dbus-1 >= 0.62],
      [have_edbus_test_client="yes"],
      [have_edbus_test_client="no"])
fi

if test "x${have_edbus_bluez_test}" = "xyes" ; then
   PKG_CHECK_MODULES([EDBUS_BLUEZ_TEST],
      [ecore >= 1.0.0 eina >= 1.0.0 dbus-1 >= 0.62],
      [have_edbus_bluez_test="yes"],
      [have_edbus_bluez_test="no"])
fi

if test "x${have_edbus_connman_test}" = "xyes" ; then
   PKG_CHECK_MODULES([EDBUS_CONNMAN_TEST],
      [ecore >= 1.0.0 eina >= 1.0.0 dbus-1 >= 0.62],
      [have_edbus_connman_test="yes"],
      [have_edbus_connman_test="no"])
fi

if test "x${have_edbus_notification_daemon_test}" = "xyes" ; then
   PKG_CHECK_MODULES([EDBUS_NOTIFICATION_DAEMON_TEST],
      [ecore >= 1.0.0 evas >= 1.0.0 eina >= 1.0.0 dbus-1 >= 0.62],
      [have_edbus_notification_daemon_test="yes"],
      [have_edbus_notification_daemon_test="no"])
fi

if test "x${have_edbus_notify_send}" = "xyes" ; then
   PKG_CHECK_MODULES([EDBUS_NOTIFY_SEND],
      [ecore >= 1.0.0 evas >= 1.0.0 eina >= 1.0.0 dbus-1 >= 0.62],
      [have_edbus_notify_send="yes"],
      [have_edbus_notify_send="no"])
fi

if test "x${have_edbus_notify_test}" = "xyes" ; then
   PKG_CHECK_MODULES([EDBUS_NOTIFY_TEST],
      [ecore >= 1.0.0 evas >= 1.0.0 dbus-1 >= 0.62],
      [have_edbus_notify_test="yes"],
      [have_edbus_notify_test="no"])
fi

if test "x${have_edbus_ofono_test}" = "xyes" ; then
   PKG_CHECK_MODULES([EDBUS_OFONO_TEST],
      [ecore >= 1.0.0 eina >= 1.0.0 dbus-1 >= 0.62],
      [have_edbus_ofono_test="yes"],
      [have_edbus_ofono_test="no"])
fi

if test "x${have_edbus_ukit_test}" = "xyes" ; then
   PKG_CHECK_MODULES([EDBUS_UKIT_TEST],
      [ecore >= 1.0.0 eina >= 1.0.0 dbus-1 >= 0.62],
      [have_edbus_ukit_test="yes"],
      [have_edbus_ukit_test="no"])
fi


### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

EFL_EDBUS_BUILD=""
case "$host_os" in
   mingw*)
      EFL_EDBUS_BUILD="-DEFL_EDBUS_BUILD"
   ;;
esac
AC_SUBST(EFL_EDBUS_BUILD)

AC_HEADER_STDC
AC_C___ATTRIBUTE__

if ! test "x${VMIC}" = "x" ; then
   EFL_COMPILER_FLAG([-Wall])
   EFL_COMPILER_FLAG([-W])
fi

EFL_COMPILER_FLAG([-Wshadow])


### Checks for linker characteristics

lt_enable_auto_import=""
case "$host_os" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
   ;;
esac
AC_SUBST(lt_enable_auto_import)


### Checks for library functions

AC_FUNC_ALLOCA


AC_SUBST(requirement_ebluez)
AC_SUBST(requirement_econnman)
AC_SUBST(requirement_edbus)
AC_SUBST(requirement_ehal)
AC_SUBST(requirement_enotify)
AC_SUBST(requirement_eofono)
AC_SUBST(requirement_eukit)
AC_SUBST(requirement_dbus)

AC_OUTPUT([
e_dbus.spec
Makefile
doc/Makefile
doc/Doxyfile
src/Makefile
src/lib/Makefile
src/lib/bluez/Makefile
src/lib/connman/Makefile
src/lib/dbus/Makefile
src/lib/hal/Makefile
src/lib/notification/Makefile
src/lib/ofono/Makefile
src/lib/ukit/Makefile
src/bin/Makefile
ebluez.pc
econnman.pc
edbus.pc
ehal.pc
enotify.pc
eofono.pc
eukit.pc
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
echo "    EBluez.............: $enable_ebluez"
echo "    EConnman...........: $enable_econnman"
echo "    EHal...............: $enable_ehal"
echo "    ENotify............: $enable_enotify"
echo "    EOfono.............: $enable_eofono"
echo "    EUkit..............: $enable_eukit"
echo
echo "  Binaries:"
echo
echo "    EDbus test.........: $have_edbus_test"
echo "    EDbus client test..: $have_edbus_test_client"
echo "    EBluez test........: $have_edbus_bluez_test"
echo "    EConnman test......: $have_edbus_connman_test"
echo "    ENotify Daemon test: $have_edbus_notification_daemon_test"
echo "    ENotify send.......: $have_edbus_notify_send"
echo "    ENotify test.......: $have_edbus_notify_test"
echo "    EOfono test........: $have_edbus_ofono_test"
echo "    EUkit test.........: $have_edbus_ukit_test"
echo
echo "Documentation..........: ${build_doc}"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
