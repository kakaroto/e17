MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src/lib @EINA_CFLAGS@ @LIBXML2_CFLAGS@
AM_CFLAGS = -I$(top_srcdir)/src/lib @EINA_CFLAGS@ @LIBXML2_CFLAGS@

lib_LTLIBRARIES = libeupnp_av.la

EUPNPAVHEADERS = Eupnp_AV.h

libeupnp_av_la_SOURCES = \
	eupnp_av.c

libeupnp_av_la_LIBADD = @EINA_LIBS@ @LIBXML2_LIBS@
libeupnp_av_la_LDFLAGS = -version-info @version_info@

installed_headersdir = $(includedir)/eupnp-@VMAJ@
dist_installed_headers_DATA = $(EUPNPAVHEADERS)
