dnl Process this file with autoconf to produce a configure script.

rm -f config.cache

AC_INIT(configure.ac)

AC_DEFINE_UNQUOTED([CONFIGURED_WITH], "$*", [what options were given to configure])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(enhance, 0.0.1)
AM_CONFIG_HEADER(config.h)

AC_PREREQ(2.52)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_INSTALL
AC_PROG_CXX
AC_LANG(C)


AM_ENABLE_SHARED
dnl AM_DISABLE_STATIC
AM_PROG_LIBTOOL

if test X"$enable_shared" = Xyes; then
 DLLDFLAGS=-export-dynamic
 AC_SUBST(DLLDFLAGS)
fi

dnl ###########################################################################
dnl # functions
dnl ###################

AC_C_CONST
AC_C_INLINE
AC_C_VOLATILE
AC_C_BIGENDIAN

AC_STRUCT_TM
AC_STRUCT_ST_BLKSIZE
AC_STRUCT_ST_BLOCKS

AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_SIGNAL
AC_TYPE_OFF_T
AC_TYPE_MODE_T

AC_CHECK_TYPE(ulong, ,[AC_DEFINE(ulong, u_long,[FreeBSD compat])])

AC_CHECK_HEADERS([limits.h])

AC_HEADER_TIME

AC_CHECK_FUNCS([strcasecmp])
AC_CHECK_FUNCS([strncasecmp])
AC_CHECK_FUNCS([strtol])
AC_CHECK_FUNCS([strstr])
AC_CHECK_FUNCS([strrchr])
AC_CHECK_FUNCS([strchr])
AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([strtoul])
AC_CHECK_FUNCS([strtoull])


AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_FUNC_MALLOC
AC_FUNC_FORK
AC_FUNC_MKTIME
AC_FUNC_LSTAT
AC_FUNC_CLOSEDIR_VOID

PKG_CHECK_MODULES(ENHANCE, [
  ecore >= 0.9.9
  exml >= 0.1.1
  etk >= 0.1.0
])

dnl ###########################################################################
dnl ###########################################################################
dnl ###########################################################################

dnl prefix
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_PREFIX,
  "${ac_default_prefix}", Prefix)
else
  AC_DEFINE_UNQUOTED(PACKAGE_PREFIX, "${prefix}", Prefix)
fi

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", [Shared Data Directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", [Shared Data Directory])
fi

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", src)

dnl Use -Wall if we have gcc.
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl

dnl
dnl Adjust the Makefile.am conditionals accordingly
dnl

AC_OUTPUT([
Makefile
enhance.pc
src/Makefile
src/lib/Makefile
enhance-config
], [
chmod +x enhance-config
])
