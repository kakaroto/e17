## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LDFLAGS		     = -L$(top_builddir)/src -L$(top_builddir)/src/.libs -L/usr/X11R6/lib 
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src \
                       -I$(top_srcdir)/loaders -I$(top_srcdir)/libltdl \
                       -I/usr/X11R6/include $(X_CFLAGS) \
                       -I$(prefix)/include -I$(includedir)

pkgdir		     = $(libdir)/imlib2_loaders/image

if JPEG_LOADER
JPEG_L              = jpeg.la
endif
if PNG_LOADER
PNG_L               = png.la
endif
if TIFF_LOADER
TIFF_L              = tiff.la
endif
if GIF_LOADER
GIF_L               = gif.la
endif
if ZLIB_LOADER
ZLIB_L              = zlib.la
endif
if BZ2_LOADER
BZ2_L               = bz2.la
endif

pkg_LTLIBRARIES	     = \
$(JPEG_L) \
$(PNG_L) \
$(TIFF_L) \
$(GIF_L) \
$(ZLIB_L) \
$(BZ2_L) \
pnm.la argb.la bmp.la xpm.la tga.la lbm.la

jpeg_la_SOURCES	     = loader_jpeg.c
jpeg_la_LDFLAGS	     = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
jpeg_la_LIBADD	     = @JPEGLIBS@ -lImlib2

png_la_SOURCES	     = loader_png.c
png_la_LDFLAGS	     = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
png_la_LIBADD	     = @PNGLIBS@ -lImlib2

tiff_la_SOURCES	     = loader_tiff.c
tiff_la_LDFLAGS	     = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
tiff_la_LIBADD	     = @TIFFLIBS@ -lImlib2

gif_la_SOURCES	     = loader_gif.c
gif_la_LDFLAGS	     = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
gif_la_LIBADD	     = @GIFLIBS@ -lImlib2

pnm_la_SOURCES	     = loader_pnm.c
pnm_la_LDFLAGS	     = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
pnm_la_LIBADD	     = -lImlib2

argb_la_SOURCES	     = loader_argb.c
argb_la_LDFLAGS	     = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
argb_la_LIBADD	     = -lImlib2

bmp_la_SOURCES	     = loader_bmp.c
bmp_la_LDFLAGS	     = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
bmp_la_LIBADD	     = -lImlib2

xpm_la_SOURCES	     = loader_xpm.c
xpm_la_LDFLAGS	     = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
xpm_la_LIBADD	     = -lImlib2

tga_la_SOURCES       = loader_tga.c
tga_la_LDFLAGS	     = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
tga_la_LIBADD	     = -lImlib2

zlib_la_SOURCES      = loader_zlib.c
zlib_la_LDFLAGS      = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
zlib_la_LIBADD       = @ZLIBLIBS@ -lImlib2

bz2_la_SOURCES       = loader_bz2.c
bz2_la_LDFLAGS       = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
bz2_la_LIBADD        = @BZ2LIBS@ -lImlib2

lbm_la_SOURCES	     = loader_lbm.c
lbm_la_LDFLAGS	     = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
lbm_la_LIBADD	     = -lImlib2
