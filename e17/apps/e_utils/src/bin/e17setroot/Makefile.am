## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@ewl_cflags@ \
@eet_cflags@ \
@engrave_cflags@

bin_PROGRAMS = e17setroot

e_util_eapp_edit_SOURCES = \
e17setroot.c

e17setroot_LDADD = @ewl_libs@ @eet_libs@ @engrave_libs@

