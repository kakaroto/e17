MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

if USE_MODULE_EUPNP_ECORE
  SUBDIRS += eupnp_ecore
endif

if USE_MODULE_EUPNP_AV
  SUBDIRS += eupnp_av
endif

AM_CPPFLAGS = -I$(top_srcdir)/src/lib @EINA_CFLAGS@ @LIBXML2_CFLAGS@
AM_CFLAGS = -I$(top_srcdir)/src/lib @EINA_CFLAGS@ @LIBXML2_CFLAGS@

lib_LTLIBRARIES = libeupnp.la

EUPNPHEADERS = \
	Eupnp.h \
	eupnp_http_message.h \
	eupnp_udp_transport.h \
	eupnp_core.h

noinst_HEADERS = eupnp_private.h

libeupnp_la_SOURCES = \
	eupnp.c \
	eupnp_utils.c \
	eupnp_ssdp.c \
	eupnp_http_message.c \
	eupnp_udp_transport.c \
	eupnp_control_point.c \
	eupnp_device_parser.c \
	eupnp_device_info.c \
	eupnp_service_info.c \
	eupnp_service_parser.c \
	eupnp_service_proxy.c \
	eupnp_event_bus.c \
	eupnp_event_server.c \
	eupnp_core.c

libeupnp_la_LIBADD = @EINA_LIBS@ @LIBXML2_LIBS@
libeupnp_la_LDFLAGS = -version-info @version_info@

installed_mainheaderdir = $(includedir)/eupnp-@VMAJ@
dist_installed_mainheader_DATA = Eupnp.h

installed_headersdir = $(includedir)/eupnp-@VMAJ@
dist_installed_headers_DATA = $(EUPNPHEADERS)

