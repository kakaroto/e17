## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@EWL_CFLAGS@ \
@EET_CFLAGS@ \
@ENGRAVE_CFLAGS@ \
@IMLIB2_CFLAGS@ \
@ENLIGHTENMENT_CFLAGS@

bin_PROGRAMS = e17setroot

e17setroot_SOURCES = \
e17setroot.c

e17setroot_LDADD = @EWL_LIBS@ @EET_LIBS@ @ENGRAVE_LIBS@ @IMLIB2_LIBS@ @ENLIGHTENMENT_LIBS@


