
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_builddir)/src \
-I$(top_srcdir)/src \
-I$(top_srcdir)/src/lib

noinst_LTLIBRARIES = libepsilon_exiftags.la

libepsilon_exiftags_la_SOURCES = \
exif.h \
exifint.h \
jpeg.h \
makers.h \
canon.c \
casio.c \
exif.c \
exifutil.c \
fuji.c \
jpeg.c \
makers.c \
minolta.c \
nikon.c \
olympus.c \
tagdefs.c

libepsilon_exiftags_la_LIBADD    =
libepsilon_exiftags_la_DEPENDENCIES = ../../config.h
libepsilon_exiftags_la_LDFLAGS      = $(LDFLAGS)
