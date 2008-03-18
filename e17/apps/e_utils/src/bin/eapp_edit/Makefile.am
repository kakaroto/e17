## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I$(top_srcdir)/lib \
@EWL_CFLAGS@ \
@EET_CFLAGS@ \
@ECORE_CFLAGS@ \
@ENGRAVE_CFLAGS@

bin_PROGRAMS = eap_edit

eap_edit_SOURCES = \
eapp_edit_main.c

eap_edit_LDADD = @EWL_LIBS@ @EET_LIBS@ @ECORE_LIBS@ @ENGRAVE_LIBS@

