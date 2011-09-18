dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT([expedite], [0.7.2], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

PKG_PROG_PKG_CONFIG


### Additional options to configure

## Simple X11 build/link

## Evas engines

AC_ARG_ENABLE(simple-x11,
   [AC_HELP_STRING([--enable-simple-x11], [enable simple x11 linking])],
   [want_evas_simple_x11=$enableval]
)

# Software X11
have_software_xlib="no"
have_software_xcb="no"
EXPEDITE_CHECK_ENGINE([software-x11], [Software X11], "yes")

if test "x${have_software_x11}" = "xyes" ; then
   have_software_xlib=`${PKG_CONFIG} --variable=Xlib evas-software-x11`
   if test "x${have_software_xlib}" = "xyes" ; then
      AC_DEFINE(HAVE_EVAS_SOFTWARE_XLIB, 1, [Evas Software Xlib Engine Support])
   fi
   have_software_xcb=`${PKG_CONFIG} --variable=XCB evas-software-x11`
   if test "x${have_software_xcb}" = "xyes" ; then
      AC_DEFINE(HAVE_EVAS_SOFTWARE_XCB, 1, [Evas Software XCB Engine Support])
   fi
fi

AM_CONDITIONAL([BUILD_SOFTWARE_XLIB], [test "x${have_software_xlib}" = "xyes"])
AM_CONDITIONAL([BUILD_SOFTWARE_XCB], [test "x${have_software_xcb}" = "xyes"])

# XRender X11
EXPEDITE_CHECK_ENGINE([xrender-x11], [XRender X11], "yes")

# OpenGL X11
EXPEDITE_CHECK_ENGINE([opengl-x11], [OpenGL X11], "yes")

# XRender XCB
EXPEDITE_CHECK_ENGINE([xrender-xcb], [XRender XCB], "yes")

# GDI
EXPEDITE_CHECK_ENGINE([software-gdi], [Software GDI], "yes", [gdi_libs="-lgdi32"])
AC_SUBST(gdi_libs)

# DirectDraw
EXPEDITE_CHECK_ENGINE([software-ddraw], [Software DirectDraw], "yes", [ddraw_libs="-lddraw -ldxguid -lgdi32"])
AC_SUBST(ddraw_libs)

# Direct3D
EXPEDITE_CHECK_ENGINE([direct3d], [Direct3D], "yes", [d3d_libs="-ld3d9 -ld3dx9d -lgdi32"])
AC_SUBST(d3d_libs)

# Software SDL
EXPEDITE_CHECK_ENGINE([software-sdl], [Software SDL], "yes")

# GL SDL
EXPEDITE_CHECK_ENGINE([opengl-sdl], [OpenGL SDL], "yes")

# FB
EXPEDITE_CHECK_ENGINE([fb], [Framebuffer], "yes")

# DIRECTFB
EXPEDITE_CHECK_ENGINE([directfb], [DirectFB], "yes")

# Software 16bit X11
EXPEDITE_CHECK_ENGINE([software-16-x11], [Software 16 bits X11], "yes")

# 16bit DirectDraw
EXPEDITE_CHECK_ENGINE([software-16-ddraw], [Software 16 bits DirectDraw], "yes", [ddraw_16_libs="-lddraw -ldxguid"])
AC_SUBST(ddraw_16_libs)

# 16bit WinCE
EXPEDITE_CHECK_ENGINE([software-16-wince], [Software 16 bits WinCE], "yes", [wince_16_libs="-laygshell"])
AC_SUBST(wince_16_libs)

### Checks for programs

AC_ISC_POSIX
m4_ifdef([AC_PROG_OBJC],
   [
    AC_PROG_OBJC
    _AM_DEPENDENCIES(OBJC)
   ],
   [
    AC_CHECK_TOOL([OBJC], [gcc])
    AC_SUBST([OBJC])
    AC_SUBST([OBJCFLAGS])
   ]
)
AC_PROG_CXX
AC_PROG_CC

LT_PROG_RC



### Checks for libraries

# Evil library for compilation on Windows

case "$host_os" in
   mingw32ce*)
      PKG_CHECK_MODULES([EVIL], [evil])
      AC_DEFINE(HAVE_EVIL, 1, [Set to 1 if Evil library is installed])
      ;;
esac

# Eina

PKG_CHECK_MODULES([EINA], eina >= 1.0.0)

# Evas

PKG_CHECK_MODULES([EVAS], evas >= 1.0.0 eet >= 1.4.0)

# Xlib

if test "x$have_software_xlib" = "xyes" -o "x$have_software_16_x11" = "xyes" -o "x$have_xrender_x11" = "xyes" -o "x$have_opengl_x11" = "xyes"; then
   AC_PATH_XTRA
   AC_CHECK_HEADERS([X11/Xlib.h X11/Xutil.h],
      [have_xlib="yes"],
      [have_xlib="no"])
   if test "x$want_evas_simple_x11" = "xyes"; then
      x_libs="${x_libs} -lX11 -lXext -lXrender"
   else
      x_dir=${x_dir:-/usr/X11R6}
      x_cflags=${x_cflags:--I${x_includes:-$x_dir/include}}
      x_libs="${x_libs:--L${x_libraries:-$x_dir/lib}} -lX11 -lXext -lXrender"
   fi
fi
AM_CONDITIONAL(BUILD_X11, [test "x$have_xlib" = "xyes"])

AC_SUBST(x_cflags)
AC_SUBST(x_libs)

# XCB

if test "x$have_software_xcb" = "xyes"; then
   PKG_CHECK_MODULES([XCB], [xcb xcb-keysyms], [have_xcb="yes"], [have_xcb="no"])
fi
AM_CONDITIONAL(BUILD_XCB, test "x$have_xcb" = "xyes")
if test "x$have_xcb" = "xyes"; then
   AC_DEFINE(BUILD_XCB, 1, [build xcb support])
fi

# SDL

if test "x$have_software_sdl" = "xyes" -o "x$have_gl_sdl" = "xyes"; then
   PKG_CHECK_MODULES([SDL], [sdl], [have_sdl="yes"], [have_sdl="no"])
fi
AM_CONDITIONAL(BUILD_SDL, test "x$have_sdl" = "xyes")

# DirectFB

if test "x$have_directfb" = "xyes"; then
   PKG_CHECK_MODULES([DIRECTFB], [directfb], [have_directfb="yes"], [have_directfb="no"])
fi
AM_CONDITIONAL(BUILD_DIRECTFB, test "x$have_directfb" = "xyes")


### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_PROG_CC_C_O
AC_C___ATTRIBUTE__

WIN32_CFLAGS=""
case "$host_os" in
   cegcc*)
      WIN32_CFLAGS="-mwin32"
      ;;
esac
AC_SUBST(WIN32_CFLAGS)

EXPEDITE_CXXFLAGS=""
AC_LANG_PUSH([C++])
CXXFLAGS_save="${CXXFLAGS}"
CXXFLAGS="${CXXFLAGS} -fno-rtti -fno-exceptions"
AC_LINK_IFELSE(
   [AC_LANG_PROGRAM([[]])],
   [have_cxx_flags="yes"],
   [have_cxx_flags="no"])
AC_MSG_CHECKING([whether the compiler supports -fno-rtti -fno-exceptions])
AC_MSG_RESULT([${have_cxx_flags}])
CXXFLAGS="${CXXFLAGS_save}"
if test "x${have_cxx_flags}" = "xyes" ; then
   EXPEDITE_CXXFLAGS="-fno-rtti -fno-exceptions"
fi
AC_LANG_POP([C++])
AC_SUBST(EXPEDITE_CXXFLAGS)


### Checks for linker characteristics

lt_enable_auto_import=""
case "$host_os" in
   mingw* | cegcc*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
      ;;
esac
AC_SUBST(lt_enable_auto_import)


### Checks for library functions

AC_CHECK_FUNCS([sync])


AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
])

echo
echo "  $PACKAGE $VERSION"
echo
echo "  Engines:"
echo "    Software X11.................: ${have_software_x11} (Xlib: ${have_software_xlib}) (XCB: ${have_software_xcb})"
echo "    X Render X11.................: ${have_xrender_x11}"
echo "    Open GL X11..................: ${have_opengl_x11}"
echo "    X Render XCB.................: ${have_xrender_xcb}"
echo "    Software GDI.................: ${have_software_gdi}"
echo "    Software DirectDraw..........: ${have_software_ddraw}"
echo "    Direct3D.....................: ${have_direct3d}"
echo "    Software SDL.................: ${have_software_sdl}"
echo "    Open GL SDL..................: ${have_opengl_sdl}"
echo "    FB...........................: ${have_fb}"
echo "    DirectFB.....................: ${have_directfb}"
echo "    Software X11 16 bits.........: ${have_software_16_x11}"
echo "    Software DirectDraw 16 bits..: ${have_software_16_ddraw}"
echo "    Software WinCE 16 bits.......: ${have_software_16_wince}"
echo
echo "  Installation:"
echo "    prefix.......................: ${prefix}"
echo
