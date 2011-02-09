# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(configure.ac)
AC_CANONICAL_TARGET
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE(Enlil, 0.6)
AM_CONFIG_HEADER(config.h)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_PROG_CC_C_O

AM_PROG_LIBTOOL

AC_SUBST(version_info)
AC_SUBST(requirements)
AC_SUBST(packagesrcdir)
AC_SUBST(packagedatadir)

PKG_PROG_PKG_CONFIG

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
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

# doxygen program for documentation building
EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

EFL_CHECK_PTHREAD(["yes"],
                  [have_pthread="yes"],
                  [have_pthread="no"],
                  [have_pthread_spinlock="yes"],
                  [have_pthread_spinlock="no"])


requirement_flickr=""
have_flickr="no"

PKG_CHECK_MODULES(EABZU, [eabzu_client > 0.0.1 ],
                  requirement_eabzu="eabzu_client"
                  have_eabzu="yes"
                  AC_DEFINE_UNQUOTED(HAVE_EABZU, "${have_eabzu}", [have network synchronization (eabzu) support]),
                  have_eabzu="no"
                  )
# required libs / cflags
flickr_libs=$FLICKR_LIBS
flickr_cflags=$FLICKR_CFLAGS
AC_SUBST(flickr_libs)
AC_SUBST(flickr_cflags)

requirements="ecore eet ecore-file evas ethumb_client"
requirements="${requirements} ${requirement_flickr}"
AC_SUBST(requirements)



PKG_CHECK_MODULES(ENLIL, [
                             ecore >= 0.9.9.062
                             ecore-file >= 0.9.9.062
                             eet >= 1.2.2
                             ethumb_client >= 0.1
                             libexif >= 0.6.16
                             libiptcdata >= 1.0.2
                             libxml-2.0 >= 0.1.1
                        ])
# required libs / cflags
enlil_libs=$ENLIL_LIBS
enlil_cflags=$ENLIL_CFLAGS
AC_SUBST(enlil_libs)
AC_SUBST(enlil_cflags)

AC_SUBST(EDJE_CC)
AC_MSG_CHECKING([Which edje_cc to use])
AC_MSG_RESULT(${EDJE_CC})

AC_OUTPUT([
Makefile
src/Makefile
src/enlil/Makefile
src/bin_load_sync/Makefile
src/bin_sync/Makefile
src/bin_print/Makefile
src/bin_geocaching_print/Makefile
src/bin_transformations/Makefile
enlil.pc
doc/Makefile
])


echo
echo
echo "--------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "--------------------------------------------------------"
echo
echo
echo "  Installation Path...................: $prefix"
echo ""
echo "  Thread Support...........................: ${have_pthread} (spinlock: ${have_pthread_spinlock})"
echo "  Network synchronisation support (requires eabzu_client) .....: ${have_eabzu}"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
