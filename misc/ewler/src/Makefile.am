## Process this file with automake to produce Makefile.in

SUBDIRS = ewl_uic

INCLUDES = @ewl_cflags@ @xml2_cflags@

bin_PROGRAMS = ewler

ewler_SOURCES = \
	ewler.c ewler.h \
	form.c form.h \
	form_file.c form_file.h \
	inspector.c inspector.h \
	main.c \
	project.c project.h \
	selected.c selected.h \
	widgets.c widgets.h \
	layout.c

ewler_LDADD = @ewl_libs@ @xml2_libs@
