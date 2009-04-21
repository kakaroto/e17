AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ELEMENTARY_CFLAGS@

if BUILD_ETHUMB
INCLUDES += @ETHUMB_CFLAGS@
endif

if BUILD_LIBEXIF
INCLUDES += @EXIF_CFLAGS@
endif

bin_PROGRAMS = image-viewer

image_viewer_SOURCES = main.c
image_viewer_LDADD = @ELEMENTARY_LIBS@

if BUILD_ETHUMB
image_viewer_LDADD += @ETHUMB_LIBS@
endif

if BUILD_LIBEXIF
image_viewer_LDADD += @EXIF_LIBS@
endif
