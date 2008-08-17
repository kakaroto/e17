## Process this file with automake to produce Makefile.in

INCLUDES = \
	@evas_cflags@ @edb_cflags@ @ebits_cflags@ @ecore_cflags@

bin_PROGRAMS = med

med_SOURCES = \
	config.c entry.c \
	evaswin.c file.c object.c \
	extras.c med.c \
	menu.c menubuild.c \
	menuedit.c util.c \
	mi_pmap.c

med_LDADD = @evas_libs@ @edb_libs@ @ebits_libs@ @ecore_libs@
