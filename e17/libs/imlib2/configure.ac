dnl Process this file with autoconf to create configure.

AC_INIT(src/Imlib2.h)
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE(imlib2, 1.0.4)

AM_CONFIG_HEADER(config.h)

AC_ARG_WITH(ttf,
        [  --with-ttf=DIR     use freetype rooted at <DIR>],
        [CPPFLAGS="$CPPFLAGS -I$withval/include"
        LIBS="-L$withval/lib $LIBS"])

mmx=no
AC_ARG_ENABLE(mmx,[  --enable-mmx            attempt compiling using mmx assembly [default=yes]],
[
  if test x$enableval = xyes; then
    mmx=yes
  fi
],
[
  if test x$target_os = xlinux-gnu; then
  mmx=`cat /proc/cpuinfo | grep mmx`
  if test -n "$mmx"; then
     mmx=yes
  fi
else
  echo ""
  echo "You are not running Linux - This script cannot auto-detect mmx assembly."
  echo "You will have to enable the mmx assembly (which gives anywhere from 10%"
  echo "to 300% speedups) by adding --enable-mmx on the configure command-line."
  echo ""
fi
]
)
if test x$mmx = xyes; then
  AC_DEFINE(DO_MMX_ASM, 1, [enabling MMX Assembly])
fi
AM_CONDITIONAL(HAVE_MMX, test x$mmx = xyes)

pkglibdir='${libdir}'/loaders
AC_SUBST(pkglibdir)

AC_C_BIGENDIAN	      

AC_LIBLTDL_CONVENIENCE
AC_CONFIG_SUBDIRS(libltdl)

AC_PROG_CC
AM_PROG_CC_STDC
AC_C_CONST

AM_ENABLE_SHARED
dnl AM_DISABLE_STATIC
AC_LIBTOOL_DLOPEN
dnl AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL
AM_WITH_DMALLOC

if test X"$enable_shared" = Xyes; then
  DLLDFLAGS=-export-dynamic
  AC_SUBST(DLLDFLAGS)
fi

AC_PROG_INSTALL
AC_PROG_MAKE_SET

ASFLAGS="$ASFLAGS -I../"
AS=$CC
AM_PROG_AS

AC_MSG_CHECKING(whether fopen accepts "b" mode)
AC_CACHE_VAL([ag_cv_func_fopen_binary],
[AC_TRY_RUN([#include <stdio.h>
int main (int argc, char *argv[])
{
   FILE *fp = fopen("/bin/sh", "rb");
   return (fclose(fp) >= 0);
}],[ag_cv_func_fopen_binary=yes],
[ag_cv_func_fopen_binary=yes],[ag_cv_func_fopen_binary=yes])
rm -f core *.exe.core])
AC_MSG_RESULT([$ag_cv_func_fopen_binary])
if test x$ag_cv_func_fopen_binary = xyes; then
  AC_DEFINE(USE_FOPEN_BINARY, 1,
	    [Define this if we can use the "b" mode for fopen safely.])
fi

AC_PATH_X
AC_PATH_XTRA

if test "x$have_x" = "xdisabled"; then
   AC_MSG_WARN([Xlib support is disabled.])
else

if test "x$x_includes" = "x"; then
  x_includes="/usr/include"
fi
  
x_cflags="$X_CFLAGS"
x_ldflags="$X_LDFLAGS"
x_libs="$X_LIBS $X_EXTRA_LIBS"
X_OBJS="color.lo context.lo draw.lo grab.lo rend.lo rgba.lo ximage.lo"
  
AC_CHECK_LIB(X11, XOpenDisplay, x_libs="-lX11 $x_libs",
  AC_MSG_ERROR([ERROR: libX11 not found.]); exit,
  $X_LDFLAGS $X_EXTRA_LIBS $X_LIBS)
AC_CHECK_LIB(Xext, XShmAttach, x_libs="-lXext $x_libs",
             AC_CHECK_LIB(XextSam, XShmAttach,
	                  x_libs="-lXextSam -lXext $x_libs",
			  AC_MSG_ERROR([ERROR: XShm not found.]); exit,
			  $x_libs),
             $x_libs)
AC_CHECK_LIB(Xext, XShapeCombineMask, x_libs="-lXext $x_libs",
	     AC_MSG_ERROR([ERROR: XShape not found.]); exit,
	     $x_libs)
fi

AC_SUBST(X_OBJS)
																	
LTLIBOBJS=`echo "$LIBOBJS" | sed 's,.o ,.lo ,g;s,.o$,.lo,'`
AC_SUBST(LTLIBOBJS)

AC_CHECK_LIB(ttf, TT_Init_FreeType, TTF_LIBS=-lttf, [
echo ""
echo "ERROR: Imlib 2 needs a system with libttf.so (TrueType Font Library)"
echo "You can obtain it from:"
echo "ftp://ftp.enlightenment.org/pub/enlightenment/"
echo "ftp://www.rasterman.com/pub/enlightenment/"
echo "You can also obtain it from:"
echo "http://www.freetype.org/"
echo "You can obtain some truetype fonts from:"
echo "http://voyager.cns.ohiou.edu/~sadkins/web_library/fonts/font_archive/"
echo ""
AC_MSG_ERROR([Fatal Error: no FreeType detected.])], -lm)

AC_CHECK_HEADERS(freetype.h freetype/freetype.h freetype1/freetype/freetype.h,[ break] , )

AC_CHECK_HEADER(freetype.h, [ ftype=yes ], [ ftype=no ])
AC_CHECK_HEADER(freetype/freetype.h, [ ftype_ftype=yes ], [ ftype_ftype=no ])
AC_CHECK_HEADER(freetype1/freetype/freetype.h, [ ftype1_ftype=yes ], [ ftype1_ftype=no ])

if test x$ftype = xno; then
if test x$ftype_ftype = xno; then
if test x$ftype1_ftype = xno; then
echo ""
echo "ERROR: freetype.h not found in freetype.h or freetype/freetype.h include"
echo "paths. (ie /usr/include, /usr/local/include etc.). You need the freetype"
echo "header files to build Imlib 2. This may mean you have to install the"
echo "freetype-devel packages for your distribution or if all else fails"
echo "find the source for freetype and install that somewhere where Imlib 2"
echo "can find the header files."
echo ""
echo "You can obtain it from:"
echo "ftp://ftp.enlightenment.org/pub/enlightenment/"
echo "ftp://www.rasterman.com/pub/enlightenment/"
echo "You can also obtain it from:"
echo "http://www.freetype.org/"
echo "You can obtain some truetype fonts from:"
echo "http://voyager.cns.ohiou.edu/~sadkins/web_library/fonts/font_archive/"
echo ""
AC_MSG_ERROR([Fatal Error: no FreeType header files detected.])
fi
fi
fi
AC_CHECK_LIB(dl, dlopen, dlopen_libs=-ldl)

EXTRA_LIBS="-L/usr/local/lib -L/usr/X11R6/lib"

# Test for libjpeg
AC_CHECK_LIB(jpeg, jpeg_destroy_decompress,
  jpeg_ok=yes,
  jpeg_ok=no
  AC_MSG_WARN(*** Native JPEG support will not be built (JPEG library not found) ***), $EXTRA_LIBS)
if test "$jpeg_ok" = yes; then
  AC_MSG_CHECKING([for jpeglib.h])
  AC_TRY_CPP(
  [#include <stdio.h>
   #undef PACKAGE
   #undef VERSION
   #include <jpeglib.h>],
  jpeg_ok=yes,
  jpeg_ok=no)
  AC_MSG_RESULT($jpeg_ok)
  if test "$jpeg_ok" = yes; then
    JPEGLIBS="-ljpeg"
  else
    AC_MSG_WARN(*** Native JPEG support will not be built (JPEG header file not found) ***)
  fi
fi

# Test for libpng
AC_CHECK_LIB(png, png_read_info,
  png_ok=yes,
  png_ok=no
  AC_MSG_WARN(*** Native PNG support will not be built (PNG library not found) ***), $EXTRA_LIBS -lz -lm)
if test "$png_ok" = yes; then
  AC_MSG_CHECKING([for png.h])
  AC_TRY_CPP(
  [#include <stdio.h>
   #undef PACKAGE
   #undef VERSION
   #include <png.h>],
  png_ok=yes,
  png_ok=no)
  AC_MSG_RESULT($png_ok)
  if test "$png_ok" = yes; then
    PNGLIBS="-lpng -lz -lm"
  else
    AC_MSG_WARN(*** Native PNG support will not be built (PNG header file not found) ***)
  fi
fi

# Test for libtiff
AC_CHECK_LIB(tiff, TIFFReadScanline,
  tiff_libs="-ltiff"
  tiff_ok=yes,
AC_CHECK_LIB(tiff, TIFFWriteScanline,
  tiff_libs="-ltiff -ljpeg -lz -lm"
  tiff_ok=yes,
AC_CHECK_LIB(tiff34, TIFFFlushData,
  tiff_libs="-ltiff -ljpeg -lz -lm"
  tiff_ok=yes,
  tiff_ok=no
  AC_MSG_WARN(*** Native TIFF support will not be built (TIFF library not found) ***), 
  $EXTRA_LIBS -ljpeg -lz -lm),
  $EXTRA_LIBS -ljpeg -lz -lm),
  $EXTRA_LIBS)
if test "$tiff_ok" = yes; then
  AC_MSG_CHECKING([for tiffio.h])
  AC_TRY_CPP(
  [#include <stdio.h>
   #undef PACKAGE
   #undef VERSION
   #include <tiffio.h>],
  tiff_ok=yes,
  tiff_ok=no)
  AC_MSG_RESULT($tiff_ok)
  if test "$tiff_ok" = yes; then
    TIFFLIBS=$tiff_libs
  else
    AC_MSG_WARN(*** Native TIFF support will not be built (TIFF header file not found) ***)
  fi
fi

# Test for libungif
AC_CHECK_LIB(ungif, DGifOpenFileName,
  gif_libs="-lungif"
  gif_ok=yes,
  gif_ok=no,
  $EXTRA_LIBS -lX11)
if test "$gif_ok" = yes; then
  AC_MSG_CHECKING([for gif_lib.h])
  AC_TRY_CPP(
  [#include <stdio.h>
   #undef PACKAGE
   #undef VERSION
   #include <gif_lib.h>],
  gif_ok=yes,
  gif_ok=no)
  AC_MSG_RESULT($gif_ok)
  if test "$gif_ok" = yes; then
    GIFLIBS=$gif_libs
  fi
fi

# Test for libgif
if test "$gif_ok" = no; then
  AC_CHECK_LIB(gif, DGifOpenFileName,
    gif_libs="-lgif"
    gif_ok=yes,
    gif_ok=no
    AC_MSG_WARN(*** Native GIF support will not be built (GIF library not found) ***),
    $EXTRA_LIBS)
  if test "$gif_ok" = yes; then
    AC_MSG_CHECKING([for gif_lib.h])
    AC_TRY_CPP(
    [#include <stdio.h>
     #undef PACKAGE
     #undef VERSION
     #include <gif_lib.h>],
    gif_ok=yes,
    gif_ok=no)
    AC_MSG_RESULT($gif_ok)
    if test "$gif_ok" = yes; then
      GIFLIBS=$gif_libs
    else
      AC_MSG_WARN(*** Native GIF support will not be built (GIF header file not found) ***)
    fi
  fi
fi

AM_CONDITIONAL(JPEG_LOADER, test "$jpeg_ok" = yes)
AM_CONDITIONAL(PNG_LOADER, test "$png_ok" = yes)
AM_CONDITIONAL(TIFF_LOADER, test "$tiff_ok" = yes)
AM_CONDITIONAL(GIF_LOADER, test "$gif_ok" = yes)

AC_SUBST(JPEGLIBS)
AC_SUBST(PNGLIBS)
AC_SUBST(TIFFLIBS)
AC_SUBST(GIFLIBS)

AC_SUBST(x_cflags)
AC_SUBST(x_includes)
AC_SUBST(x_ldflags)
AC_SUBST(x_libs)
AC_SUBST(dlopen_libs)

AC_OUTPUT(imlib2-config Makefile loaders/Makefile src/Makefile test/Makefile \
          filters/Makefile demo/Makefile doc/Makefile imlib2.spec \
	  ,
[
test -z "$CONFIG_HEADERS" || echo timestamp > stamp-h;
chmod +x imlib2-config
])
