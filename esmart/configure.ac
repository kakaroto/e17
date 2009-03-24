dnl Process this file with autoconf to produce a configure script.

AC_INIT(esmart, 0.9.0.050, enlightenment-devel@lists.sourceforge.net)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(src/config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_BIGENDIAN
AC_C_CONST

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

case "$host_os" in
    mingw*)
        dnl needed for correct definition of EAPI
        AC_DEFINE(EFL_ESMART_CONTAINER_BUILD, 1, [Define to mention that esmart container is built])
        AC_DEFINE(EFL_ESMART_TEXT_ENTRY_BUILD, 1, [Define to mention that esmart text entry is built])
        ;;
esac

# evas is required by all of the modules anyway :)
requirements="evas"
PKG_CHECK_MODULES(CONTAINER, [evas >= 0.9.9 ecore >= 0.9.9 ecore-evas >= 0.9.9 eina-0])

AC_CHECK_HEADER(ltdl.h, have_esmart_container=yes, have_esmart_container=no)

PKG_CHECK_MODULES(DRAGGIES, [
    evas >= 0.9.9
    ecore-x >= 0.9.9
    ecore-evas >= 0.9.9
    eina-0
  ], [
    have_esmart_draggies=yes
    requirements="$requirements ecore-evas"
  ], [
    have_esmart_draggies=no
  ]
)

PKG_CHECK_MODULES(RESIZE, [
    evas >= 0.9.9
    ecore-x >= 0.9.9
    ecore-evas >= 0.9.9
    eina-0
  ], [
    have_esmart_resize=yes
    requirements="$requirements ecore-evas"
  ], [
    have_esmart_resize=no
  ]
)

PKG_CHECK_MODULES(TRANSX11, [
    evas >= 0.9.9
    ecore-x >= 0.9.9
    imlib2 >= 1.0.0
    eina-0
  ], [
    have_esmart_trans_x11=yes
    requirements="$requirements ecore-x imlib2"
  ], [
    have_esmart_trans_x11=no
  ]
)

PKG_CHECK_MODULES(THUMB, [
    evas >= 0.9.9
    imlib2 >= 1.0.0
    epsilon >= 0.3.0
    eina-0
  ], [
    have_esmart_thumb=yes
    requirements="$requirements imlib2 epsilon"
  ], [
    have_esmart_thumb=no
  ]
)

PKG_CHECK_MODULES(TEXT_ENTRY, [
    evas >= 0.9.9
    ecore >= 0.9.9
    ecore-evas >= 0.9.9
    edje >= 0.9.9
    eina-0
  ], [
    have_esmart_text_entry=yes
    requirements="$requirements edje"
  ], [
    have_esmart_text_entry=no
  ]
)

PKG_CHECK_MODULES(FILE_DIALOG, [
    evas >= 0.9.9
    edje >= 0.5.0
    eina-0
  ], [
    have_esmart_file_dialog=yes
    requirements="$requirements edje"
  ], [
    have_esmart_file_dialog=no
  ]
)

PKG_CHECK_MODULES(CAIRO, [
    cairo >= 1.6.4
    evas >= 0.9.9
    eina-0
  ], [
    PKG_CHECK_MODULES(CAIRO_TEST, [
        cairo >= 1.6.4
        evas >= 0.9.9
        ecore-evas >= 0.9.9
        ecore >= 0.9.9
      ])
    have_esmart_cairo=yes
    requirements="$requirements cairo"
  ], [
    have_esmart_cairo=no
  ]
)

PKG_CHECK_MODULES(XPIXMAP, [
    evas >= 0.9.9
    ecore-x >= 0.9.9
    eina-0
  ], [
    have_esmart_xpixmap=yes
    requirements="$requirements ecore-x"
  ], [
    have_esmart_xpixmap=no
  ]
)

dnl Check for edce_cc
EDJE_DEF=""
AC_SUBST(EDJE_DEF)
AC_ARG_WITH(edje-cc,
        AC_HELP_STRING([--with-edje-cc=PATH],[Specify a specific path to edje_cc]),
[
  v=$withval;
  edje_cc=$v
  echo "  Enlightenment edje_cc explicitly set to "$edje_cc;
],[
  edje_cc=$(pkg-config --variable=prefix edje)/bin/edje_cc
])
AC_SUBST(edje_cc)


have_esmart_textarea=no
have_esmart_file_dialog=no

AM_CONDITIONAL(BUILD_ESMART_CONTAINER, test $have_esmart_container = yes)
AM_CONDITIONAL(BUILD_ESMART_DRAGGIES, test $have_esmart_draggies = yes)
AM_CONDITIONAL(BUILD_ESMART_RESIZE, test $have_esmart_resize = yes)
AM_CONDITIONAL(BUILD_ESMART_FILE_DIALOG, test $have_esmart_file_dialog = yes)
AM_CONDITIONAL(BUILD_ESMART_TEXT_ENTRY, test $have_esmart_text_entry = yes)
AM_CONDITIONAL(BUILD_ESMART_TEXTAREA, test $have_esmart_textarea = yes)
AM_CONDITIONAL(BUILD_ESMART_THUMB, test $have_esmart_thumb = yes)
AM_CONDITIONAL(BUILD_ESMART_TRANS_X11, test $have_esmart_trans_x11 = yes)
AM_CONDITIONAL(BUILD_ESMART_CAIRO, test $have_esmart_cairo = yes)
AM_CONDITIONAL(BUILD_ESMART_XPIXMAP, test $have_esmart_xpixmap = yes)

AM_CONDITIONAL(BUILD_TEST_APP, test $have_esmart_container = yes -a $have_esmart_thumb = yes)

AC_PATH_PROG(PERL, perl, 0)
AC_SUBST(PERL)

AC_SUBST(requirements)

AC_OUTPUT([
Makefile
esmart_container.pc
esmart_draggies.pc
esmart_file_dialog.pc
esmart_resize.pc
esmart_textarea.pc
esmart_text_entry.pc
esmart_thumb.pc
esmart_trans_x11.pc
esmart_xpixmap.pc
esmart.spec
src/Makefile
src/lib/Makefile
src/lib/esmart_container/Makefile
src/lib/esmart_container/layout/Makefile
src/lib/esmart_container/layout/default/Makefile
src/lib/esmart_container/layout/entice/Makefile
src/lib/esmart_container/layout/engage/Makefile
src/lib/esmart_text_entry/Makefile
src/lib/esmart_file_dialog/Makefile
src/lib/esmart_thumb/Makefile
src/lib/esmart_trans_x11/Makefile
src/lib/esmart_draggies/Makefile
src/lib/esmart_resize/Makefile
src/lib/esmart_cairo/Makefile
src/lib/esmart_xpixmap/Makefile
src/bin/Makefile
data/Makefile
data/images/Makefile
])

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "Components:"
echo "  Container...............: $have_esmart_container"
echo "  Draggies................: $have_esmart_draggies"
echo "  Resize..................: $have_esmart_resize"
echo "  File Dialog.............: $have_esmart_file_dialog"
echo "  Text Entry..............: $have_esmart_text_entry"
echo "  Text Area...............: $have_esmart_textarea"
echo "  Thumb...................: $have_esmart_thumb"
echo "  Trans X11...............: $have_esmart_trans_x11"
echo "  Cairo...................: $have_esmart_cairo"
echo "  X Pixmap................: $have_esmart_xpixmap"
echo
echo "------------------------------------------------------------------------"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
