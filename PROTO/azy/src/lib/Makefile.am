SUBDIRS = extras

lib_LTLIBRARIES = \
  libazy.la

libazy_la_CFLAGS = \
  $(ECORE_CFLAGS) \
  $(XML_CFLAGS) \
 -I$(top_srcdir) \
 -I$(top_srcdir)/src/include \
 -I$(top_srcdir)/src/include/extras

libazy_la_LIBADD = \
  $(ECORE_LIBS) \
  $(XML_LIBS) \
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
  azy_content_xml.c \
  azy_server.c \
  azy_net.c \
  azy_utils.c

MAINTAINERCLEANFILES = Makefile.in
