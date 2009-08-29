MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src/lib @EUPNP_CFLAGS@ @ELM_CFLAGS@
AM_CFLAGS = -I$(top_srcdir)/src/lib @EUPNP_CFLAGS@ @ELM_CFLAGS@

bin_PROGRAMS = igd_client \
               lighting_control

igd_client_SOURCES = igd_client.c
igd_client_LDADD = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la @ELM_LIBS@
igd_client_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la

lighting_control_SOURCES = lighting_control.c lighting_control_gui.c
lighting_control_LDADD = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la @ELM_LIBS@
lighting_control_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
