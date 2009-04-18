MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@EMAGE_CFLAGS@ \
@PNG_CFLAGS@

# PNG
if EMAGE_ENABLE_PNG
emage_png_LTLIBRARIES = emage_png.la
emage_pngdir = $(libdir)/emage/

emage_png_la_SOURCES = \
emage_png.c

emage_png_la_LIBADD       = $(top_builddir)/src/lib/libemage.la @EMAGE_LIBS@ @PNG_LIBS@
emage_png_la_LDFLAGS      = -no-undefined -module -avoid-version
emage_png_la_DEPENDENCIES = $(top_builddir)/src/lib/libemage.la
endif

# JPG
if !EMAGE_ENABLE_JPG
emage_jpg_LTLIBRARIES = emage_jpg.la
emage_jpgdir = $(libdir)/emage/

emage_jpg_la_SOURCES = \
emage_jpg.c

emage_jpg_la_LIBADD       = $(top_builddir)/src/lib/libemage.la @EMAGE_LIBS@ -ljpeg
emage_jpg_la_LDFLAGS      = -no-undefined -module -avoid-version
emage_jpg_la_DEPENDENCIES = $(top_builddir)/src/lib/libemage.la
endif
