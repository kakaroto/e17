dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache


AC_INIT(configure.ac)


AC_CANONICAL_BUILD

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(exalt, 0.9)

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
AC_C___ATTRIBUTE__

# doxygen program for documentation building
EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

vpnc=""
AC_ARG_WITH([vpnc],
            AC_HELP_STRING([--with-vpnc=VPNC_PATH], [explicitly specify a vpnc path]),
            [vpnc=$withval])


if test "z$vpnc" = "z"; then
    AC_PATH_PROG(vpnc, vpnc, none)
fi
if test "$vpnc" = "none"; then
    have_vpnc="no"
else
    AC_DEFINE_UNQUOTED(VPNC_COMMAND_PATH, "${vpnc}", [Path of vpnc])
    have_vpnc="yes"
fi

wpa_supplicant=""
AC_ARG_WITH([wpa_supplicant],
            AC_HELP_STRING([--with-wpa_supplicant=WPA_SUPPLICANT_PATH], [explicitly specify a wpa_supplicant path]),
            [wpa_supplicant=$withval])

#removed since wpa_supplicant is required and not optional
if test "z$wpa_supplicant" = "z"; then
AC_PATH_PROG(wpa_supplicant, wpa_supplicant, none)
fi


if test "$wpa_supplicant" = "none"; then
    have_wpa_supplicant="no"
    AC_MSG_ERROR([No wpa_supplicant program found])
else
    AC_DEFINE_UNQUOTED(WPA_SUPPLICANT_COMMAND_PATH, "${wpa_supplicant}", [Path of wpa_supplicant])
    have_wpa_supplicant="yes"
fi

vpnc_disconnect=""
AC_ARG_WITH([vpnc_disconnect],
            AC_HELP_STRING([--with-vpnc-disconnect=VPNC_DISCONNECT_PATH], [explicitly specify a vpnc disconnect path]),
            [vpnc_disconnect=$withval])

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


dhcp=""
AC_ARG_WITH([dhcp],
            AC_HELP_STRING([--with-dhcp=DHCP_PATH], [explicitly specify a dhcp path, currently only dhclient is supported]),
            [dhcp=$withval])


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

#can't retrieve the system service dir :/
#dbusservicedir=`$PKG_CONFIG --variable=session_bus_system-services_dir dbus-1`
#AC_SUBST(dbusservicedir)

requirements="ecore >= 1.0.0 eet >= 1.4.0 ecore-file >= 1.0.0 ehal >= 1.0.0 edbus >= 1.0.0 dbus-1"

PKG_CHECK_MODULES(EXALT, [
  eina >= 1.0.0,
  ecore >= 1.0.0,
  eet >= 1.4.0,
  ecore-file >= 1.0.0,
  ehal >= 1.0.0,
  edbus >= 1.0.0,
  dbus-1 >= 0.1
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
doc/Makefile
src/Makefile
src/bin/Makefile
src/lib/Makefile
src/lib_dbus/Makefile
src/bin_command/Makefile
data/Makefile
exalt.pc
exalt_dbus.pc
org.e.Exalt.service
]
)

txt_strip() {
        echo "[$]@" | sed -e 's/^[[ \t]]*\([[^ \t]]*\)[[ \t]]*$/\1/g'
}

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
echo "  dhcp support (only dhclient is supported)...: $have_dhcp ($dhcp)"
echo "  wpa_supplicant support......................: $have_wpa_supplicant ($wpa_supplicant)"
#echo "  DBus services dir...........................: $(txt_strip $dbusservicedir))"
#echo "  vpnc support...................: not yet implemented"
echo
echo "  Documentation...............................: ${build_doc}"
echo
echo "  Installation Path...........................: $prefix"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
echo
echo
echo "PLEASE READ THE README !"
echo


