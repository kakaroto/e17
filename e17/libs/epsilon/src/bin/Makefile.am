## Process this file with automake to produce Makefile.in

INCLUDES = \
-I$(top_srcdir)/src/lib \
@imlib_cflags@

bin_PROGRAMS = epsilon

epsilon_SOURCES = \
epsilon_main.c 

epsilon_LDADD = \
$(top_builddir)/src/lib/libepsilon.la @imlib_libs@

epsilon_DEPENDENCIES = $(top_builddir)/src/lib/libepsilon.la
