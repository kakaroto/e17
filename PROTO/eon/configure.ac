# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT([eon], [0.0.3], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_C_BIGENDIAN
AC_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL
AM_PROG_LEX
if test "x$ac_cv_prog_LEX" = "x"; then
  AC_MSG_ERROR(Cannot find flex)
fi
AC_PROG_YACC
if test "x$ac_cv_prog_YACC" = "x"; then
  AC_MSG_ERROR(Cannot find bison)
fi

dnl Add the languages which your application supports here.

if test "x${bindir}" = 'x${exec_prefix}/bin'; then
  if test "x${exec_prefix}" = "xNONE"; then
    if test "x${prefix}" = "xNONE"; then
      bindir="${ac_default_prefix}/bin";
    else
      bindir="${prefix}/bin";
    fi
  else
    if test "x${prefix}" = "xNONE"; then
      bindir="${ac_default_prefix}/bin";
    else
      bindir="${prefix}/bin";
    fi
  fi
fi

if test "x${libdir}" = 'x${exec_prefix}/lib'; then
  if test "x${exec_prefix}" = "xNONE"; then
    if test "x${prefix}" = "xNONE"; then
      libdir="${ac_default_prefix}/lib";
    else
      libdir="${prefix}/lib";
    fi
  else
    if test "x${prefix}" = "xNONE"; then
      libdir="${ac_default_prefix}/lib";
    else
      libdir="${prefix}/lib";
    fi
  fi
fi

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", [Shared Data Directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", [Shared Data Directory])
fi

dnl Set PACKAGE_BIN_DIR in config.h.
if test "x${bindir}" = 'xNONE'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${ac_default_prefix}/bin", [Installation Directory for User Executables])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${prefix}/bin", [Installation Directory for User Executables])
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

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [Source Code Directory])

case "$host_os" in
    *bsd*)
       AC_DEFINE(HAVE_BSD, [], "Have a BSD distribution")
    ;;
    linux*)
       AC_DEFINE(HAVE_LINUX, [], "Have a Linux distribution")
    ;;
esac

requirements="eina ekeko ecore enesim emage etch freetype2 fontconfig"

PKG_CHECK_MODULES(EINA, [eina >= 0.0.1])
PKG_CHECK_MODULES(EKEKO, [ekeko])
PKG_CHECK_MODULES(ECORE, [ecore >= 0.9.9])
PKG_CHECK_MODULES(ENESIM, [enesim >= 0.0.1])
PKG_CHECK_MODULES(EMAGE, [emage >= 0.0.1])
PKG_CHECK_MODULES(ETCH, [etch >= 0.0.1])
PKG_CHECK_MODULES([FREETYPE], [freetype2 >= 9.3.0])
PKG_CHECK_MODULES([FONTCONFIG], [fontconfig])
################################################################################
##                               Scripting                                    ##
################################################################################
## Neko
NEKO_LIBS=
AC_CHECK_LIB(neko, val_callEx,
             [ AC_CHECK_HEADERS(neko.h neko_vm.h, have_neko="yes", have_neko="no") ],
             have_neko="yes")
if test "x$have_neko" = "xyes"; then
	NEKO_LIBS=-lneko
	AC_DEFINE(BUILD_SCRIPT_NEKO, [1], [Build support for NekoVM scripts])
fi
AM_CONDITIONAL(BUILD_SCRIPT_NEKO, test "x$have_neko" = "xyes")
AC_SUBST(NEKO_LIBS)
################################################################################
##                                 Cache                                      ##
################################################################################
PKG_CHECK_MODULES(ESHM, [eshm], [have_eshm=yes], [have_eshm=no])
if test "x$have_eshm" = "xyes"; then
	AC_DEFINE(BUILD_CACHE_ESHM, [1], [Build support for shared memory based cache])
fi
AM_CONDITIONAL(BUILD_CACHE_EHSM, test "x$have_eshm" = "xyes")
################################################################################
##                                Engines                                     ##
################################################################################
## SDL engine

PKG_CHECK_MODULES(ECORE_SDL, [ecore-sdl >= 0.9.9], [have_ecore_sdl=yes], [have_ecore_sdl=no])
PKG_CHECK_MODULES(SDL, [sdl], [have_sdl=yes], [have_sdl=no])
if test "x$have_ecore_sdl" = "xyes" && test "x$have_sdl" = "xyes"; then
	build_engine_sdl=yes
	AC_DEFINE(BUILD_ENGINE_SDL, [1], [Build support for SDL engine])
else
	build_engine_sdl=no
fi
AM_CONDITIONAL(BUILD_ENGINE_SDL, test "x$build_engine_sdl" = "xyes")

PKG_CHECK_MODULES(ECORE_IPC, [ecore-ipc], [have_ecore_ipc=yes], [have_ecore_ipc=no])
if test "x$have_ecore_ipc" = "xyes"; then
	build_engine_ipc=yes
	AC_DEFINE(BUILD_ENGINE_IPC, [1], [Build support for IPC engine])
else
	build_engine_ipc=no
fi
AM_CONDITIONAL(BUILD_ENGINE_IPC, test "x$build_engine_ipc" = "xyes")

## Check for the possible engines
if test "x$build_engine_sdl" = "xno" && test "x$build_engine_ipc" = "xyes"; then
  AC_MSG_ERROR(No engine to compile)
fi

#####################################################################
## Fill in flags
AC_SUBST(requirements)
AC_SUBST(packagesrcdir)

EON_CFLAGS="$EINA_CFLAGS $EKEKO_CFLAGS $ECORE_CFLAGS $ENESIM_CFLAGS $ETCH_CFLAGS $EMAGE_CFLAGS $ESHM_CFLAGS $FREETYPE_CFLAGS $FONTCONFIG_CFLAGS"
EON_LIBS="$EINA_LIBS $EKEKO_LIBS $ECORE_LIBS $ENESIM_LIBS $ETCH_LIBS $EMAGE_LIBS $ESHM_LIBS $FREETYPE_LIBS $FONTCONFIG_LIBS"
AC_SUBST(EON_CFLAGS)
AC_SUBST(EON_LIBS)

#####################################################################
## Output

AC_OUTPUT([
Makefile
eon.pc
src/Makefile
src/lib/Makefile
src/lib/core/Makefile
src/lib/filter/Makefile
src/lib/paint/Makefile
src/lib/layout/Makefile
src/lib/shape/Makefile
src/lib/widget/Makefile
src/lib/parser/Makefile
src/lib/parser/common/Makefile
src/lib/script/Makefile
src/lib/script/neko/Makefile
src/include/Makefile
src/engines/Makefile
src/engines/sdl/Makefile
src/engines/enesim/Makefile
src/engines/remote/Makefile
src/bin/Makefile
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
echo "Installation Path.........: $prefix"
echo
echo "Engines:"
echo "  SDL Engine..................................: $build_engine_sdl"
echo "  Remote Engine...............................: $build_engine_ipc"
echo
echo "Scripts:"
echo "  NekoVM......................................: $have_neko"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
