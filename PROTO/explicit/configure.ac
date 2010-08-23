AC_INIT([explicit], [0.0.1], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

VMAJ=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $1);}'`
VMIN=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $2);}'`
VMIC=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $3);}'`
SNAP=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $4);}'`
version_info=`expr $VMAJ + $VMIN`":$VMIC:$VMIN"
#release="ver-pre-svn-00"
#release_info="-release $release"
release_info=""
AC_SUBST(version_info)
AC_SUBST(release_info)

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

dnl we just have set the version info, then:
AC_SUBST(version_info)

AC_PROG_CC

PKG_CHECK_MODULES([EXPLICIT], [eina ecore ecore-con ecore-file])
PKG_CHECK_MODULES([EVAS], [evas])

### Checks for header files
AC_HEADER_ASSERT
AC_HEADER_DIRENT
AC_HEADER_TIME

### Checks for types


### Checks for structures


### Checks for compiler characteristics
AC_C_CONST
AC_C_BIGENDIAN
AC_C_INLINE
AC_C___ATTRIBUTE__
AC_PROG_CC_STDC
AM_PROG_CC_C_O

### Checks for library functions
AC_FUNC_ALLOCA
AC_CHECK_FUNCS(strlcpy)

AC_CONFIG_FILES([
Makefile
src/Makefile
src/bin/Makefile
src/lib/Makefile
])

AC_OUTPUT

echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
