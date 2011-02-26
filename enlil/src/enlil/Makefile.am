
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/enlil \
-I$(top_builddir)/src/enlil \
@ENLIL_CFLAGS@ \
@EFL_ENLIL_BUILD@

lib_LTLIBRARIES = libenlil.la

libenlil_la_SOURCES  = \
enlil.c \
Enlil.h \
enlil_private.h \
library.c \
collection.c \
tag.c \
album.c \
photo.c \
sync.c \
load.c \
thumb.c \
file_manager.c \
transformations.c \
iptc.c \
exif.c \
exif/jpeg-data.c \
exif/jpeg-marker.c \
exif/exif-i18n.c \
geocaching.c \
net_sync.c \
flickr.c \
download.c

installed_headersdir = $(prefix)/include/enlil
installed_headers_DATA = Enlil.h

libenlil_la_LIBADD  = @ENLIL_LIBS@

libenlil_la_LDFLAGS = -no-undefined -version-info @version_info@

