## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@ewl_cflags@ \
@eet_cflags@ \
@engrave_cflags@ \
@imlib2_cflags@ \
@e_cflags@

bin_PROGRAMS = e17setroot

e17setroot_SOURCES = \
e17setroot.c

e17setroot_LDADD = @ewl_libs@ @eet_libs@ @engrave_libs@ @imlib2_libs@ @e_libs@

filesdir = $(datadir)/e_utils/data/e17setroot
files_DATA = 

	

