## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@EWL_CFLAGS@ \
@EET_CFLAGS@ \
@ECORE_CFLAGS@ \
@ENGRAVE_CFLAGS@

bin_PROGRAMS = e_util_eapp_edit

e_util_eapp_edit_SOURCES = \
eapp_edit_main.c

e_util_eapp_edit_LDADD = @EWL_LIBS@ @EET_LIBS@ @ECORE_LIBS@ @ENGRAVE_LIBS@

