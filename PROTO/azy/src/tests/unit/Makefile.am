EXTRA_DIST = \
  tests.h

check_PROGRAMS = t001_content

# t001

t001_content_CFLAGS = \
  $(ECORE_CFLAGS) \
  $(XML_CFLAGS) \
  -I$(top_builddir) \
  -I$(top_srcdir)/src/include \
  -I$(top_srcdir)/src/include/extras

t001_content_SOURCES = \
  t001_content.c \
  phony_lib.c \
  $(top_srcdir)/src/lib/azy_content.c \
  $(top_srcdir)/src/lib/azy_content_json.c \
  $(top_srcdir)/src/lib/azy_content_xml.c \
  $(top_srcdir)/src/lib/azy_value.c

t001_content_LDADD = \
  $(ECORE_LIBS) \
  $(XML_LIBS) \
  $(top_builddir)/src/lib/extras/libcJSON.la \
  $(top_builddir)/src/lib/extras/libb64.la

t001_content_LDFLAGS = -static

MAINTAINERCLEANFILES = Makefile.in
