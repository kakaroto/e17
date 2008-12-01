# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(enesim, 0.0.1, enlightenment-devel@lists.sourceforge.net)
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

PKG_CHECK_MODULES(EINA, [eina-0 >= 0.0.1])
PKG_CHECK_MODULES(PNG, [libpng >= 0.22], [have_png="yes"], [have_png="no"])
#PKG_CHECK_MODULES(SDL, [sdl >= 1.2.10], [have_sdl="yes"], [have_sdl="no"])

build_example="no"
#if test "x$have_sdl" = "xyes" && test "x$have_png" = "xyes"; then 
if test "x$have_png" = "xyes"; then 
	build_example="yes"
fi

## TODO Bench

## Coverage
AC_ARG_ENABLE([coverage],
   [AC_HELP_STRING([--enable-coverage], [compile with coverage profiling instrumentation @<:@default=no@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       enable_coverage="yes"
    else
       enable_coverage="no"
    fi
   ],
   [enable_coverage="no"]
)
## lcov program for coverage report
if test "x$build_example" = "xno" -a "x$enable_coverage" = "xyes" ; then
   AC_MSG_WARN([Coverage report requested but tests not being built. Run configure with --enable-tests])
   enable_coverage="no"
fi

if test "x$enable_coverage" = "xyes"; then
   AC_CHECK_PROG(have_lcov, [lcov], [yes], [no])
   if test "x$have_lcov" = "xyes" ; then
      COVERAGE_CFLAGS="-fprofile-arcs -ftest-coverage"
      COVERAGE_LIBS="-lgcov"
# remove any optimisation flag and force debug symbols
      DEBUG_CFLAGS="-g -O0 -DDEBUG"
   else
      AC_MSG_WARN([lcov is not found, disable profiling instrumentation])
      enable_coverage="no"
   fi
fi
AC_SUBST(COVERAGE_CFLAGS)
AC_SUBST(COVERAGE_LIBS)

AM_CONDITIONAL(ENESIM_ENABLE_COVERAGE, test "x${enable_coverage}" = "xyes")


AC_MSG_CHECKING([whether to use profiling instrumentation])
AC_MSG_RESULT([$enable_coverage])


## Add conditionals for every surface format but the argb8888_pre, it is
## the core format and cannot be optional
ENESIM_SURFACE_FORMAT(argb8888_unpre, [yes])
ENESIM_SURFACE_FORMAT(rgb888_a8, [no])
ENESIM_SURFACE_FORMAT(rgb565_xa5, [no])
ENESIM_SURFACE_FORMAT(rgb565_b1a3, [yes])

EXAMPLE_CFLAGS="$SDL_CFLAGS $PNG_CFLAGS"
EXAMPLE_LIBS="$SDL_LIBS $PNG_LIBS"
AC_SUBST(EXAMPLE_CFLAGS)
AC_SUBST(EXAMPLE_LIBS)

AM_CONDITIONAL(BUILD_EXAMPLE, test "x${build_example}" = "xyes")

ENESIM_CFLAGS="$EINA_CFLAGS $COVERAGE_CFLAGS"
ENESIM_LIBS="$EINA_LIBS $COVERAGE_LIBS"
AC_SUBST(ENESIM_CFLAGS)
AC_SUBST(ENESIM_LIBS)

## The tool directory
## Code taken from texinfo
if test "$cross_compiling" = no; then
  native_tools=
else
  native_tools=tools
  test -d "$native_tools" || mkdir "$native_tools"
  confdir=`(cd "$srcdir";pwd)`
  # Make sure the secondary configure won't fail with
  # "error: source directory already configured".
  rm -f config.status
  AC_MSG_NOTICE([[Doing configure of native tools (${build}).]])
  cd "$native_tools" || exit 1
  # Run secondary configure in alternate environment or
  # it gets the wrong CC etc.
  # env -i gives this build host configure a clean environment;
  # consequently, we have to re-initialize $PATH.
  env -i CC="$BUILD_CC" AR="$BUILD_AR" RANLIB="$BUILD_RANLIB" \
      PATH="$PATH" \
      tools_only=1 \
    ${confdir}/configure --build=${build} --host=${build} 
  cd .. || exit 1
  AC_MSG_NOTICE([[Continuing with main configure (${host}).]])
fi
AC_SUBST(native_tools)
AM_CONDITIONAL(TOOLS_ONLY, [[test "x$tools_only" = x1]])

## Make the debug preprocessor configurable

AC_OUTPUT([
Makefile
enesim.pc
src/Makefile
src/include/Makefile
src/tool/Makefile
src/lib/Makefile
src/lib/rasterizer/Makefile
src/lib/renderer/Makefile
src/lib/drawer/Makefile
src/lib/util/Makefile
src/lib/vector/Makefile
src/lib/vector/component/Makefile
examples/Makefile
data/Makefile
],[
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
echo "  Build example.............................: ${build_example}"
echo "  Coverage..................................: ${enable_coverage}"
echo "  Build ARGB8888_UNPRE format support.......: ${format_argb8888_unpre}"
echo "  Build RGB8888_A8 format support...........: ${format_rgb888_a8}"
echo "  Build RGB565_XA5 format support...........: ${format_rgb565_xa5}"
echo "  Build RGB565_b1A3 format support..........: ${format_rgb565_b1a3}"
echo
echo "Installation Path...........................: ${prefix}"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
