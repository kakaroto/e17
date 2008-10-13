
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_text_entry \
@TEXT_ENTRY_CFLAGS@ @EINA_CFLAGS@

pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_text_entry.la

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Text_Entry.h

libesmart_text_entry_la_SOURCES = \
esmart_text_entry.c \
Esmart_Text_Entry.h

libesmart_text_entry_la_LDFLAGS = -no-undefined -version-info @version_info@
libesmart_text_entry_la_LIBADD = @EINA_LIBS@ @TEXT_ENTRY_LIBS@

