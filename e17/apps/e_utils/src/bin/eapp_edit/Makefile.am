## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@ewl_cflags@ \
@eet_cflags@ \
@ecore_cflags@ \
@engrave_cflags@

bin_PROGRAMS = e_util_eapp_edit

e_util_eapp_edit_SOURCES = \
eapp_edit_main.c

e_util_eapp_edit_LDADD = @ewl_libs@ @eet_libs@ @ecore_libs@ @engrave_libs@

