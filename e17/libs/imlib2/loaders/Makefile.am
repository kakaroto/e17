## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LDFLAGS		     = -L/usr/X11R6/lib
INCLUDES	     = -I/usr/X11R6/include -I$(top_srcdir)/libltdl \
                       $(X_CFLAGS) -I$(prefix)/include -I$(includedir) \
		       -I. -I$(top_srcdir) -I$(top_srcdir)/src \
		       -I$(top_srcdir)/loaders

pkgdir		     = $(libdir)/loaders/image
pkg_LTLIBRARIES	     = png.la jpeg.la gif.la pnm.la argb.la tiff.la bmp.la

png_la_SOURCES	     = loader_png.c
png_la_LDFLAGS	     = -no-undefined -module -avoid-version
png_la_LIBADD	     = -lpng -lz

jpeg_la_SOURCES	     = loader_jpeg.c
jpeg_la_LDFLAGS	     = -no-undefined -module -avoid-version
jpeg_la_LIBADD	     = -ljpeg

gif_la_SOURCES	     = loader_gif.c
gif_la_LDFLAGS	     = -no-undefined -module -avoid-version
gif_la_LIBADD	     = -lgif

pnm_la_SOURCES	     = loader_pnm.c
pnm_la_LDFLAGS	     = -no-undefined -module -avoid-version
pnm_la_LIBADD	     =

argb_la_SOURCES	     = loader_argb.c
argb_la_LDFLAGS	     = -no-undefined -module -avoid-version
argb_la_LIBADD	     =

tiff_la_SOURCES	     = loader_tiff.c
tiff_la_LDFLAGS	     = -no-undefined -module -avoid-version
tiff_la_LIBADD	     = -ltiff

bmp_la_SOURCES	     = loader_bmp.c
bmp_la_LDFLAGS	     = -no-undefined -module -avoid-version
bmp_la_LIBADD	     = 
