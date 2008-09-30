dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache


AC_INIT(configure.ac)

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(exalt, 0.6)

AC_PREREQ(2.52)

AM_CONFIG_HEADER(src/config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", [Shared Data Directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", [Shared Data Directory])
fi

dnl Set PACKAGE_BIN_DIR in config.h.
if test "x${bindir}" = 'xNONE'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${ac_default_prefix}/sbin", [Installation Directory for User Executables])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${prefix}/sbin", [Installation Directory for User Executables])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${bindir}", [Installation Directory for User Executables])
fi

dnl Set PACKAGE_LIB_DIR in config.h.
if test "x${libdir}" = 'xNONE'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${ac_default_prefix}/lib", [Installation Directory for Libraries])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${prefix}/lib", [Installation Directory for Libraries])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${libdir}", [Installation Directory for Libraries])
fi


dnl Set PACKAGE_LOCALE_DIR in config.h.
if test "x$LIBINTL" = "x"; then
  LIBINTL="$INTLLIBS"
 fi
 if test "x${prefix}" = "xNONE"; then
   AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/share/locale", [Locale-specific data directory])
 else
   AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/share/locale", [Locale-specific data directory])
 fi


dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [Source Code Directory])



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

if test "z$vpnc" = "z"; then
    AC_PATH_PROG(vpnc, vpnc, none)
fi
if test "$vpnc" = "none"; then
    have_vpnc="no"
else
    AC_DEFINE_UNQUOTED(VPNC_COMMAND_PATH, "${vpnc}", [Path of vpnc])
    have_vpnc="yes"
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


requirements="ecore eet ecore-file ehal"

PKG_CHECK_MODULES(EXALT, [
  ecore >= 0.9.9.037,
  eet >= 0.9.9.038,
  ecore-file >= 0.9.9,
  ehal >= 0.1.0.002,
  edbus >= 0.1.0.002,
  dbus-1 >= 0.1,
  evas >= 0.9
])



if test X"$enable_shared" = Xyes; then
 DLLDFLAGS=-export-dynamic
 AC_SUBST(DLLDFLAGS)
fi


AC_SUBST(requirements)

AC_SUBST(packagesrcdir)


AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
src/lib/Makefile
src/lib_dbus/Makefile
data/Makefile
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
echo "  wpa_supplicant support.........: $have_wpa_supplicant ($wpa_supplicant)"
echo "  dhcp support...................: $have_dhcp ($dhcp)"
echo "  vpnc support...................: not yet implemented"
echo
echo "  Installation Path..............: $prefix"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo


