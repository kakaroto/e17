## Process this file with automake to produce Makefile.in

bin_PROGRAMS = efsd

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile

INCLUDES = $(GLIB_CFLAGS)

include_HEADERS = \
	efsd.h \
	libefsd.h

efsd_SOURCES = \
	efsd.c \
	efsd_debug.h \
	efsd_fam.h \
	efsd_fam.c \
	efsd_io.h \
	efsd_io.c \
	efsd_fileops.h \
	efsd_fileops.c \
	efsd_misc.h \
	efsd_misc.c

efsd_LDADD = $(GLIB_LIBS)

lib_LTLIBRARIES = libefsd.la

libefsd_la_SOURCES   = \
	efsd_io.h \
	efsd_io.c \
	efsd.h \
	libefsd.c

