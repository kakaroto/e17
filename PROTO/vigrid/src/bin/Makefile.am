
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/include \
-I$(top_builddir)/src/include \
@VIGRID_CFLAGS@

bin_PROGRAMS = vigrid

vigrid_SOURCES = vigrid.c

vigrid_LDADD = $(top_builddir)/src/lib/libvigrid.la @VIGRID_LIBS@
