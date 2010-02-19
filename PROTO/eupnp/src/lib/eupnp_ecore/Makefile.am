MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src/lib \
	@EINA_CFLAGS@ @ECORE_CFLAGS@ @ECORECON_CFLAGS@ @E_UPNP_BUILD@

lib_LTLIBRARIES = libeupnp_ecore.la

EUPNPECOREHEADERS = Eupnp_Ecore.h

libeupnp_ecore_la_SOURCES = \
	eupnp_ecore.c

libeupnp_ecore_la_LIBADD = @EINA_LIBS@ @ECORE_LIBS@ @ECORECON_LIBS@
libeupnp_ecore_la_LDFLAGS = -version-info @version_info@

installed_headersdir = $(includedir)/eupnp-@VMAJ@
dist_installed_headers_DATA = $(EUPNPECOREHEADERS)
