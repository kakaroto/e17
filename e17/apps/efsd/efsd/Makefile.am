## Process this file with automake to produce Makefile.in

bin_PROGRAMS = efsd

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile

INCLUDES = -DDEBUG #-DDEBUG_NEST

include_HEADERS = \
	efsd.h \
	libefsd.h

efsd_SOURCES = \
	efsd.c \
	efsd_common.h \
	efsd_common.c \
	efsd_debug.h \
	efsd_debug.c \
	efsd_fam.h \
	efsd_fam.c \
	efsd_fileops.h \
	efsd_fileops.c \
	efsd_io.h \
	efsd_io.c \
	efsd_queue.h \
	efsd_queue.c \
	efsd_list.h \
	efsd_list.c \
	efsd_misc.h \
	efsd_misc.c \
	efsd_types.h \
	efsd_types.c

lib_LTLIBRARIES = libefsd.la

libefsd_la_SOURCES   = \
	efsd.h \
	efsd_debug.h \
	efsd_debug.c \
	efsd_io.h \
	efsd_io.c \
	efsd_common.h \
	efsd_common.c \
	efsd_misc.h \
	efsd_misc.c \
	efsd_types.h \
	efsd_types.c \
	libefsd.c

