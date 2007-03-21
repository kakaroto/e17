pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_text_entry.la

INCLUDES = -I$(top_srcdir)/src/lib/esmart_text_entry \
			  @TEXT_ENTRY_CFLAGS@

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Text_Entry.h

libesmart_text_entry_la_SOURCES = esmart_text_entry.c \
											Esmart_Text_Entry.h

libesmart_text_entry_la_LDFLAGS = -version-info 0:9:0
libesmart_text_entry_la_LIBADD = @TEXT_ENTRY_LIBS@

