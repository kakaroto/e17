SUBDIRS = extras

lib_LTLIBRARIES = \
  libazy.la

AM_CFLAGS = \
  $(AZY_CFLAGS) \
 -I$(top_srcdir) \
 -I$(top_srcdir)/src/include \
 -I$(top_srcdir)/src/include/extras
AM_CXXFLAGS = $(AM_CFLAGS)

if HAVE_XML
xml_c = azy_content_xml.cpp
PUGI = extras/libpugixml.la
else
xml_c =
PUGI =
endif


libazy_la_LIBADD = \
  $(AZY_LIBS) \
  $(PUGI) \
  -lm \
  extras/libcJSON.la \
  extras/libb64.la

libazy_la_LDFLAGS = -version-info @version_info@ @release_info@ -no-undefined --as-needed


libazy_la_SOURCES = \
  azy_lib.c \
  azy_events.c \
  azy_client_events.c \
  azy_server_events.c \
  azy_value.c \
  azy_client.c \
  azy_content.c \
  azy_content_json.c \
  $(xml_c) \
  azy_server.c \
  azy_server_module.c \
  azy_net.c \
  azy_rss.c \
  azy_rss_item.c \
  azy_utils.c

MAINTAINERCLEANFILES = Makefile.in
