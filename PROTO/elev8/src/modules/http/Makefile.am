AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-Wall -Wextra \
-I$(top_srcdir) \
-I$(top_srcdir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_TMP_DIR=\"/tmp\" \
@ECORECON_CFLAGS@

moddir = $(libdir)/$(PACKAGE)/$(MODULE_ARCH)
mod_LTLIBRARIES = libelev8_http.la

libelev8_http_la_SOURCES = elev8_http.cc

libelev8_http_la_LIBADD = @ECORECON_LIBS@ @V8_LIBS@
libelev8_http_la_LDFLAGS = -module -avoid-version -no-undefined
libelev8_http_la_LIBTOOLFLAGS = --tag=disable-static
