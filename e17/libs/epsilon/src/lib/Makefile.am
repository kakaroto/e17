## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = exiftags

LDFLAGS		     = -L/usr/local/lib @imlib_libs@ @epeg_libs@
INCLUDES	     = -I/usr/local/include \
		       -I$(includedir) @imlib_cflags@ @epeg_cflags@

lib_LTLIBRARIES      = libepsilon.la
include_HEADERS      = Epsilon.h
libepsilon_la_SOURCES  = \
Epsilon.c \
Epsilon_Exif.c \
md5.c md5.h \
exiftags/canon.c \
exiftags/casio.c \
exiftags/exif.c \
exiftags/exifutil.c \
exiftags/fuji.c \
exiftags/getopt.c \
exiftags/jpeg.c \
exiftags/makers.c \
exiftags/minolta.c \
exiftags/nikon.c \
exiftags/olympus.c \
exiftags/tagdefs.c

libepsilon_la_LIBADD	= $(LDFLAGS)
libepsilon_la_DEPENDENCIES = ../config.h 
libepsilon_la_LDFLAGS      = -version-info 0:1:0

EXTRA_DIST= exiftags
