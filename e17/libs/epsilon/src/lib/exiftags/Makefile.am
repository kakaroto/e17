## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

INCLUDES             = -I$(top_builddir)/src \
		       -I$(top_srcdir)/src \
		       -I$(top_srcdir)/src/lib \
		       @IMLIB2_CFLAGS@ @EPEG_CFLAGS@

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

libepsilon_exiftags_la_LIBADD    = @imlib_libs@ @epeg_libs@
libepsilon_exiftags_la_DEPENDENCIES = ../../config.h
libepsilon_exiftags_la_LDFLAGS      = $(LDFLAGS)
