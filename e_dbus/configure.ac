# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(e_dbus, 0.5.0.050, enlightenment-devel@lists.sourceforge.net)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_HEADER_STDC
AM_PROG_CC_C_O

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

VMAJ=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $1);}'`
VMIN=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $2);}'`
VMIC=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $3);}'`
SNAP=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $4);}'`
version_info=`expr $VMAJ + $VMIN`":$VMIC:$VMIN"
AC_SUBST(version_info)

dnl Check enabled modules to build
AC_ARG_ENABLE(ehal,
				  AC_HELP_STRING([--disable-ehal],
									  [Disable ehal build]),
				  [enable_ehal=$enableval],
				  [enable_ehal="yes"])
AC_ARG_ENABLE(enm,
				  AC_HELP_STRING([--disable-enm],
									  [Disable enm build]),
				  [enable_enm=$enableval],
				  [enable_enm="yes"])
AC_ARG_ENABLE(enotify,
				  AC_HELP_STRING([--disable-enotify],
									  [Disable enotify build]),
				  [enable_enotify=$enableval],
				  [enable_enotify="yes"])
AC_ARG_ENABLE(econnman,
				  AC_HELP_STRING([--disable-econnman],
									  [Disable econnman build]),
				  [enable_econnman=$enableval],
				  [enable_econnman="yes"])


PKG_CHECK_MODULES([EINA], [eina-0])

PKG_CHECK_MODULES([EDBUS], [
  dbus-1 >= 0.62
  ecore >= 0.9.9
])


dnl evas
if test "x${enable_enotify}" = "xyes"
then
	PKG_CHECK_MODULES(EVAS, [evas >= 0.9.9], [], [enable_enotify="no"])
fi

AC_ARG_ENABLE(build-test-gui,
  AS_HELP_STRING([--enable-build-test-gui], [Build with gui for testing]),
  [ PKG_CHECK_MODULES([HAL_TEST_GUI], [
    ewl
    efreet
    ],
    # temporarily disable until update to newer tree2 api
    [AC_DEFINE([EWL_GUI], 0, [An ewl gui for e_dbus_hal])],
    [AC_DEFINE([EWL_GUI], 0, [An ewl gui for e_dbus_hal])]
    )
 ]
)

AM_CONDITIONAL([BUILD_EHAL],    [test "x${enable_ehal}"    = "xyes"])
AM_CONDITIONAL([BUILD_ENM],     [test "x${enable_enm}"     = "xyes"])
AM_CONDITIONAL([BUILD_ENOTIFY], [test "x${enable_enotify}" = "xyes"])
AM_CONDITIONAL([BUILD_ECONNMAN], [test "x${enable_econnman}" = "xyes"])

AC_OUTPUT([
e_dbus.spec
Makefile
src/Makefile
src/lib/Makefile
src/lib/dbus/Makefile
src/lib/hal/Makefile
src/lib/nm/Makefile
src/lib/notification/Makefile
src/lib/connman/Makefile
src/bin/Makefile
edbus.pc
ehal.pc
enotify.pc
enm.pc
econnman.pc
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
echo "Modules:"
echo
echo "  EHal.................: $enable_ehal"
echo "  ENM..................: $enable_enm"
echo "  ENotify..............: $enable_enotify"
echo "  EConnman.............: $enable_econnman"
echo
echo "Configuration Options Summary:"
echo
echo "  Compilation..........: make"
echo
echo "  Installation.........: make install"
echo
echo "    prefix.............: $prefix"
echo
