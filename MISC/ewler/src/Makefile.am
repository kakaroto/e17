## Process this file with automake to produce Makefile.in

SUBDIRS = 

INCLUDES = @ewl_cflags@ @exml_cflags@

MAINTAINERCLEANFILES = \
	Makefile.in config.h.in

bin_PROGRAMS = ewler

ewler_SOURCES = \
	callback.c \
	file.c \
	form.c \
	inspector.c \
	main.c \
	project.c \
	spec.c \
	tool.c \
	widget.c

ewler_LDADD = @ewl_libs@ @exml_libs@
