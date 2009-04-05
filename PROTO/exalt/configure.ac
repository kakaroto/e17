dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache


AC_INIT(configure.ac)


AC_CANONICAL_BUILD

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(exalt, 0.8)

AC_PREREQ(2.52)


AM_CONFIG_HEADER(src/config.h)

AC_CONFIG_MACRO_DIR([m4])

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

# Find edje_cc
PKG_CHECK_MODULES(EDJE, [edje >= 0.5.0])
AC_ARG_WITH(edje-cc,
  AC_HELP_STRING([--with-edje-cc=PATH], [specify a specific path to edje_cc]),
  [
    v=$withval;
    EDJE_CC=$v
  ],[
    EDJE_CC=$(pkg-config --variable=prefix edje)/bin/edje_cc
  ]
)
AC_SUBST(EDJE_CC)
AC_MSG_CHECKING([Which edje_cc to use])
AC_MSG_RESULT(${EDJE_CC})

if test "z$vpnc" = "z"; then
    AC_PATH_PROG(vpnc, vpnc, none)
fi
if test "$vpnc" = "none"; then
    have_vpnc="no"
else
    AC_DEFINE_UNQUOTED(VPNC_COMMAND_PATH, "${vpnc}", [Path of vpnc])
    have_vpnc="yes"
fi


if test "z$wpa_supplicant" = "z"; then
AC_PATH_PROG(wpa_supplicant, wpa_supplicant, none)
fi
if test "$wpa_supplicant" = "none"; then
    have_wpa_supplicant="no"
else
    AC_DEFINE_UNQUOTED(WPA_SUPPLICANT_COMMAND_PATH, "${wpa_supplicant}", [Path of wpa_supplicant])
    have_wpa_supplicant="yes"
    AC_DEFINE_UNQUOTED(HAVE_WPA_SUPPLICANT, "${have_wpa_supplicant}", [have wpa_supplicant support])
fi

if test "z$vpnc_disconnect" = "z"; then
    AC_PATH_PROG(vpnc_disconnect, vpnc_disconnect, none)
fi
if test "$vpnc_disconnect" = "none"; then
    have_vpnc="no"
else
    AC_DEFINE_UNQUOTED(VPNC_DISONNECT_COMMAND_PATH, "${vpnc_disconnect}", [Path of vpnc_disconnect])
    if test  "$vpnc" = "none"; then
        have_vpnc="no"
    else
        have_vpnc="yes"
        AC_DEFINE_UNQUOTED(HAVE_VPNC, "${have_vpnc}", [have vpnc support])
    fi
fi


if test "z$dhcp" = "z"; then
    AC_PATH_PROG(dhcp, dhclient, none)
fi
if test "$dhcp" = "none"; then
    have_dhcp="no"
else
    have_dhcp="yes"
    AC_DEFINE_UNQUOTED(DHCP_COMMAND_PATH, "${dhcp}", [Path of a dhcp command])
    AC_DEFINE_UNQUOTED(HAVE_DHCP, "${have_dhcp}", [have dhcp support])
fi


requirements="elementary ecore eet ecore-file ehal edbus evas dbus-1"

PKG_CHECK_MODULES(EXALT, [
  ecore >= 0.9.9.037,
  eet >= 0.9.9.038,
  ecore-file >= 0.9.9,
  ehal >= 0.1.0.002,
  edbus >= 0.1.0.002,
  dbus-1 >= 0.1,
  evas >= 0.9
  elementary >= 0.1
])



if test X"$enable_shared" = Xyes; then
 DLLDFLAGS=-export-dynamic
 AC_SUBST(DLLDFLAGS)
fi


AC_SUBST(requirements)

AC_SUBST(packagesrcdir)

AC_ARG_ENABLE([shave],
              [  --disable-shave                  Use shave output cleaner when building.],
              [ case "${enableval}" in
               yes)    enable_shave=yes;;
               no)    enable_shave=no;;
               *)    AC_MSG_ERROR([bad value ${enableval} for --disable-shave]);;
               esac],
               [enable_shave=yes])

if test x$enable_shave = xyes; then
    SHAVE_INIT([./])
fi

AC_CONFIG_FILES([
    shave
    shave-libtool
])

AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
src/lib/Makefile
src/lib_dbus/Makefile
src/bin_command/Makefile
src/bin_client/Makefile
data/Makefile
data/icons/Makefile
data/daemon/Makefile
data/daemon/dbus/Makefile
exalt.pc
exalt_dbus.pc
]
)

###############################################################
#### Informations

#$have_vpnc ($vpnc, $vpnc_disconnect)"

echo
echo
echo "--------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "--------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo
echo "  dhcp support...................: $have_dhcp ($dhcp)"
echo "  wpa_supplicant support.........: $have_wpa_supplicant ($wpa_supplicant)"
#echo "  vpnc support...................: not yet implemented"
echo
echo "  Installation Path..............: $prefix"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo


