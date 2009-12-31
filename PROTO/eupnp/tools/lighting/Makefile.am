MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src/lib @EUPNP_CFLAGS@ @ELM_CFLAGS@
AM_CFLAGS = -I$(top_srcdir)/src/lib @EUPNP_CFLAGS@ @ELM_CFLAGS@

bin_PROGRAMS = lighting

lighting_SOURCES = lighting.c lighting_gui.c
lighting_LDADD = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la @ELM_LIBS@
lighting_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
