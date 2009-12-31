MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src/lib @EUPNP_CFLAGS@ @ELM_CFLAGS@
AM_CFLAGS = -I$(top_srcdir)/src/lib @EUPNP_CFLAGS@ @ELM_CFLAGS@

bin_PROGRAMS = igd

igd_SOURCES = igd.c
igd_LDADD = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la @ELM_LIBS@
igd_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
