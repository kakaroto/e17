## Process this file with automake to produce Makefile.in

bin_PROGRAMS = efsd

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile

INCLUDES = -DDEBUG #-DDEBUG_NEST

include_HEADERS = \
	efsd.h \
	libefsd.h

efsd_SOURCES = \
	efsd_main.h \
	efsd_main.c \
	efsd_macros.h \
	efsd_debug.h \
	efsd_debug.c \
	efsd_fam.h \
	efsd_fam.c \
	efsd_fileops.h \
	efsd_fileops.c \
	efsd_globals.h \
	efsd_io.h \
	efsd_io.c \
	efsd_magic.c \
	efsd_magic.h \
	efsd_queue.h \
	efsd_queue.c \
	efsd_list.h \
	efsd_list.c \
	efsd_misc.h \
	efsd_misc.c \
	efsd_options.h \
	efsd_options.c \
	efsd_types.h \
	efsd_types.c

efsd_LDADD = -ledb

lib_LTLIBRARIES = libefsd.la

libefsd_la_SOURCES   = \
	efsd.h \
	efsd_macros.h \
	efsd_debug.h \
	efsd_debug.c \
	efsd_io.h \
	efsd_io.c \
	efsd_misc.h \
	efsd_misc.c \
	efsd_options.h \
	efsd_options.c \
	efsd_types.h \
	efsd_types.c \
	libefsd.c

