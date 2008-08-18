## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I$(top_srcdir)/lib \
@EET_CFLAGS@ \
@ENGRAVE_CFLAGS@ \
@IMLIB2_CFLAGS@ \
@ENLIGHTENMENT_CFLAGS@

bin_PROGRAMS = e17setroot

e17setroot_SOURCES = \
e17setroot.c

e17setroot_LDADD = @ENGRAVE_LIBS@ @IMLIB2_LIBS@ @ENLIGHTENMENT_LIBS@ @ECORE_LIBS@


