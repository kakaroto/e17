
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
   -Wall -g @ENLIL_CFLAGS@ @FLICKR_CFLAGS@

lib_LTLIBRARIES      = libenlil.la
ENLILHEADERS      = Enlil.h
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
flickr.c \
download.c

installed_headersdir = $(prefix)/include/enlil
installed_headers_DATA = $(ENLILHEADERS)

libenlil_la_LIBADD  = @ENLIL_LIBS@ @FLICKR_LIBS@

libenlil_la_DEPENDENCIES =

libenlil_la_LDFLAGS = -version-info 1:0:0

