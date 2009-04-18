# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(emage, 0.0.2, enlightenment-devel@lists.sourceforge.net)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(src/include/config.h)

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_C_BIGENDIAN

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

create_shared_lib=""
case "$host_os" in
    mingw|mingw32)
	create_shared_lib="-no-undefined"
    ;;
esac
AC_SUBST(create_shared_lib)

PKG_CHECK_MODULES(ENESIM, [enesim >= 0.0.1])
PKG_CHECK_MODULES(EINA, [eina-0 >= 0.0.1])

EMAGE_CFLAGS="$EINA_CFLAGS $ENESIM_CFLAGS"
EMAGE_LIBS="$EINA_LIBS $ENESIM_LIBS"
AC_SUBST(EMAGE_CFLAGS)
AC_SUBST(EMAGE_LIBS)

## Different Modules
## PNG
PKG_CHECK_MODULES(PNG, [libpng >= 0.22], [enable_png="yes"], [enable_png="no"])
AM_CONDITIONAL(EMAGE_ENABLE_PNG, test "x${enable_png}" = "xyes")
## JPG
AC_CHECK_HEADER(jpeglib.h, [enable_jpg="yes"], [enable_jpg="no"])
AM_CONDITIONAL(EMAGE_ENABLE_JPG, test "x${enable_jpg}" = "xyes")

## Make the debug preprocessor configurable

AC_OUTPUT([
Makefile
emage.pc
src/Makefile
src/include/Makefile
src/lib/Makefile
src/modules/Makefile
examples/Makefile
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
echo
echo "Configuration Options Summary:"
echo
echo "Providers"
echo "PNG.......................: ${enable_png}"
echo "JPG.......................: ${enable_jpg}"
echo "Installation Path.........: $prefix"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
