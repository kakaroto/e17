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
pkg_LTLIBRARIES	     = jpeg.la png.la db.la gif.la tiff.la pnm.la argb.la bmp.la xpm.la tga.la #xcf.la

jpeg_la_SOURCES	     = @JPEG_LA@
jpeg_la_LDFLAGS	     = -no-undefined -module -avoid-version
jpeg_la_LIBADD	     = @JPEGLIBS@

png_la_SOURCES	     = @PNG_LA@
png_la_LDFLAGS	     = -no-undefined -module -avoid-version
png_la_LIBADD	     = @PNGLIBS@

db_la_SOURCES        = @EDB_LA@
db_la_LDFLAGS	     = -no-undefined -module -avoid-version
db_la_LIBADD	     = @EDBLIBS@ -lz

tiff_la_SOURCES	     = @TIFF_LA@
tiff_la_LDFLAGS	     = -no-undefined -module -avoid-version
tiff_la_LIBADD	     = @TIFFLIBS@

gif_la_SOURCES	     = @GIF_LA@
gif_la_LDFLAGS	     = -no-undefined -module -avoid-version
gif_la_LIBADD	     = @GIFLIBS@

pnm_la_SOURCES	     = loader_pnm.c
pnm_la_LDFLAGS	     = -no-undefined -module -avoid-version
pnm_la_LIBADD	     =

argb_la_SOURCES	     = loader_argb.c
argb_la_LDFLAGS	     = -no-undefined -module -avoid-version
argb_la_LIBADD	     =

bmp_la_SOURCES	     = loader_bmp.c
bmp_la_LDFLAGS	     = -no-undefined -module -avoid-version
bmp_la_LIBADD	     = 

xpm_la_SOURCES	     = loader_xpm.c
xpm_la_LDFLAGS	     = -no-undefined -module -avoid-version
xpm_la_LIBADD	     = 

tga_la_SOURCES       = loader_tga.c
tga_la_LDFLAGS	     = -no-undefined -module -avoid-version
tga_la_LIBADD	     = 

#xcf_la_SOURCES       = loader_xcf.c loader_xcf_pixelfuncs.c
#xcf_la_LDFLAGS	     = -no-undefined -module -avoid-version
#xcf_la_LIBADD	     =
