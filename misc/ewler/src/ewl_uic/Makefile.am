## Process this file with automake to produce Makefile.in

INCLUDES = @ewl_cflags@ @xml2_cflags@
CFLAGS = -g

bin_PROGRAMS = ewl_uic

ewl_uic_SOURCES = \
	ewl_uic.c \
	widgets.c \
	form_file.c \
	generate.c

ewl_uic_LDADD = @ewl_libs@ @xml2_libs@
