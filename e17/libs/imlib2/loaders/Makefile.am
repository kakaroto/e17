## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LDFLAGS		     = -L/usr/X11R6/lib -L$(top_srcdir)/src
INCLUDES	     = -I/usr/X11R6/include -I$(top_srcdir)/libltdl \
                       $(X_CFLAGS) -I$(prefix)/include -I$(includedir) \
		       -I. -I$(top_srcdir) -I$(top_srcdir)/src \
		       -I$(top_srcdir)/loaders

pkgdir		     = $(libdir)/loaders/image

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

pkg_LTLIBRARIES	     = \
$(JPEG_L) \
$(PNG_L) \
$(TIFF_L) \
$(GIF_L) \
pnm.la argb.la bmp.la xpm.la tga.la #xcf.la

jpeg_la_SOURCES	     = loader_jpeg.c
jpeg_la_LDFLAGS	     = -no-undefined -module -avoid-version
jpeg_la_LIBADD	     = @JPEGLIBS@ -L$(top_builddir)/src/.libs -lImlib2

png_la_SOURCES	     = loader_png.c
png_la_LDFLAGS	     = -no-undefined -module -avoid-version
png_la_LIBADD	     = @PNGLIBS@ -L$(top_builddir)/src/.libs -lImlib2

tiff_la_SOURCES	     = loader_tiff.c
tiff_la_LDFLAGS	     = -no-undefined -module -avoid-version
tiff_la_LIBADD	     = @TIFFLIBS@ -L$(top_builddir)/src/.libs -lImlib2

gif_la_SOURCES	     = loader_gif.c
gif_la_LDFLAGS	     = -no-undefined -module -avoid-version
gif_la_LIBADD	     = @GIFLIBS@ -L$(top_builddir)/src/.libs -lImlib2

pnm_la_SOURCES	     = loader_pnm.c
pnm_la_LDFLAGS	     = -no-undefined -module -avoid-version
pnm_la_LIBADD	     = -L$(top_builddir)/src/.libs -lImlib2

argb_la_SOURCES	     = loader_argb.c
argb_la_LDFLAGS	     = -no-undefined -module -avoid-version
argb_la_LIBADD	     = -L$(top_builddir)/src/.libs -lImlib2

bmp_la_SOURCES	     = loader_bmp.c
bmp_la_LDFLAGS	     = -no-undefined -module -avoid-version
bmp_la_LIBADD	     = -L$(top_builddir)/src/.libs -lImlib2

xpm_la_SOURCES	     = loader_xpm.c
xpm_la_LDFLAGS	     = -no-undefined -module -avoid-version
xpm_la_LIBADD	     = -L$(top_builddir)/src/.libs -lImlib2

tga_la_SOURCES       = loader_tga.c
tga_la_LDFLAGS	     = -no-undefined -module -avoid-version
tga_la_LIBADD	     = -L$(top_builddir)/src/.libs -lImlib2

#xcf_la_SOURCES       = loader_xcf.c loader_xcf_pixelfuncs.c
#xcf_la_LDFLAGS	     = -no-undefined -module -avoid-version
#xcf_la_LIBADD	     =
