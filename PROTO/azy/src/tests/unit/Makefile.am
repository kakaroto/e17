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
  t001_content.c

if HAVE_XML
if ! HAVE_LIBXML
  pugi_ld = -lstdc++
else
  pugi_ld =
endif
endif

t001_content_LDADD = \
  $(ECORE_LIBS) \
  $(XML_LIBS) \
  $(pugi_ld) \
  $(top_builddir)/src/lib/libazy.la

t001_content_LDFLAGS = -static

MAINTAINERCLEANFILES = Makefile.in
