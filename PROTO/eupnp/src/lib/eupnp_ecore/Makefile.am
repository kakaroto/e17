MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src/lib @EINA_CFLAGS@ @ECORE_CFLAGS@ @ECORECON_CFLAGS@
AM_CFLAGS = -I$(top_srcdir)/src/lib @EINA_CFLAGS@ @ECORE_CFLAGS@ @ECORECON_CFLAGS@

lib_LTLIBRARIES = libeupnp_ecore.la

include_HEADERS = \
	eupnp_ecore.h

libeupnp_ecore_la_SOURCES = \
	eupnp_ecore.c

libeupnp_ecore_la_LIBADD = @EINA_LIBS@ @ECORE_LIBS@ @ECORECON_LIBS@
libeupnp_ecore_la_LDFLAGS = -version-info @version_info@
