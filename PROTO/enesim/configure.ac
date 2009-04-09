# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT([enesim], [0.0.2], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(src/include/config.h)

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


### Additional options to configure


### Checks for programs
AC_PROG_CC


### Checks for libraries

# Eina

PKG_CHECK_MODULES(EINA, [eina-0 >= 0.0.1])

# Libpng

PKG_CHECK_MODULES(PNG, [libpng >= 0.22], [have_png="yes"], [have_png="no"])

### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_C_BIGENDIAN


### Checks for linker characteristics


### Checks for library functions

#######################################
## PTHREADS
pthread_cflags=""
pthread_libs=""
build_pthreads="no"
has_pthreads="no"
need_pthreads="no"
# basic pthread support
AC_CHECK_HEADER(pthread.h,
  [
   has_pthreads="yes"
  ],
  [
   has_pthreads="no"
  ]
)

# sched_getaffinity pthread_attr_setaffinity_np
AC_CHECK_HEADERS(pthread.h sched.h,
      [
        AC_CHECK_LIB(pthread, pthread_attr_setaffinity_np,
          [
            AC_CHECK_LIB(pthread, pthread_barrier_wait,
              [ build_pthreads="yes" ],
              [ build_pthreads="no" ]
            )
          ],
          [ build_pthreads="no" ]
        )
      ],
      [ build_pthreads="no" ]
)

AC_MSG_CHECKING(whether to build pthread code)
AC_ARG_ENABLE(pthreads,
  AC_HELP_STRING([--enable-pthreads], [enable threaded rendering]),
  [
      if test "x$enableval" = "xyes" ; then
        if test "x$build_pthreads" = "xyes"; then
          AC_MSG_RESULT(yes)
          AC_DEFINE(BUILD_PTHREAD, 1, [Build Threaded Rendering])
          build_pthreads="yes"
	  need_pthreads="yes"
        else
          if "x$use_strict" = "xyes"; then
            AC_MSG_ERROR(pthreads headers or functions not found (strict dependencies checking))
          else
            AC_MSG_RESULT(no: pthread headers or functions not found)
          fi
        fi
      else
        AC_MSG_RESULT(no)
        build_pthreads="no"
      fi
  ],
  [
    AC_MSG_RESULT($build_pthreads)
    if test "x$build_pthreads" = "xyes" ; then
      AC_DEFINE(BUILD_PTHREAD, 1, [Build Threaded Rendering])
      need_pthreads="yes"
    fi
  ]
)

#######################################
## Link with pthread if needed
AC_MSG_CHECKING(whether we should link with pthread)
if test "x$need_pthreads" = "xyes"; then
  AC_MSG_RESULT(yes)
  pthread_cflags=""
  pthread_libs="-lpthread"
else
  AC_MSG_RESULT(no)
fi


AC_SUBST(pthread_cflags)
AC_SUBST(pthread_libs)


### Needed information

EFL_CHECK_CPU_MMX([have_mmx="yes"], [have_mmx="no"])
EFL_CHECK_CPU_SSE([have_sse="yes"], [have_sse="no"])
EFL_CHECK_CPU_SSE2([have_sse2="yes"], [have_sse2="no"])
EFL_CHECK_CPU_ALTIVEC([have_altivec="yes"], [have_altivec="no"])


# Add conditionals for every surface format but the argb8888_pre, it is
# the core format and cannot be optional
ENESIM_SURFACE_FORMAT(argb8888_unpre, [yes])
ENESIM_SURFACE_FORMAT(rgb888_a8, [no])
ENESIM_SURFACE_FORMAT(rgb565_xa5, [no])
ENESIM_SURFACE_FORMAT(rgb565_b1a3, [yes])

# Unit tests, coverage and benchmarking

EFL_CHECK_TESTS([enable_tests="yes"], [enable_tests="no"])

EFL_CHECK_COVERAGE([${enable_tests}], [enable_coverage="yes"], [enable_coverage="no"])

EFL_CHECK_BENCHMARK([enable_benchmark="yes"], [enable_benchmark="no"])

### Substitutions

EXAMPLE_CFLAGS="$SDL_CFLAGS $PNG_CFLAGS"
EXAMPLE_LIBS="$SDL_LIBS $PNG_LIBS"
AC_SUBST(EXAMPLE_CFLAGS)
AC_SUBST(EXAMPLE_LIBS)

ENESIM_CFLAGS="$EINA_CFLAGS $EFL_SIMD_FLAGS $EFL_COVERAGE_CFLAGS $pthread_cflags -D_GNU_SOURCE"
ENESIM_LIBS="$EINA_LIBS $EFL_COVERAGE_LIBS $pthread_libs"
AC_SUBST(ENESIM_CFLAGS)
AC_SUBST(ENESIM_LIBS)


## Make the debug preprocessor configurable

AC_OUTPUT([
Makefile
enesim.pc
src/Makefile
src/include/Makefile
src/tool/Makefile
src/lib/Makefile
src/lib/cpu/Makefile
src/lib/primitives/Makefile
src/lib/rasterizer/Makefile
src/lib/format/Makefile
src/lib/drawer/Makefile
src/lib/converter/Makefile
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
echo "  Build example.............................: ${enable_benchmark}"
echo "  Coverage..................................: ${enable_coverage}"
echo "  Build ARGB8888_UNPRE format support.......: ${format_argb8888_unpre}"
echo "  Build RGB8888_A8 format support...........: ${format_rgb888_a8}"
echo "  Build RGB565_XA5 format support...........: ${format_rgb565_xa5}"
echo "  Build RGB565_b1A3 format support..........: ${format_rgb565_b1a3}"
echo
echo "CPU Specific Extensions:"
echo
echo "  Multi Core ...............................: ${build_pthreads}"
echo "  MMX.......................................: ${have_mmx}"
echo "  SSE.......................................: ${have_sse}"
echo "  SSE2......................................: ${have_sse2}"
echo "  ALTIVEC...................................: ${have_altivec}"
echo
echo "Installation Path...........................: ${prefix}"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
