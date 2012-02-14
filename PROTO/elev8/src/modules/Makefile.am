AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_TMP_DIR=\"/tmp\" \
@ELEV8_CFLAGS@

lib_LTLIBRARIES = libelev8-dbus.la \
				  libelev8-http.la \
				  libelev8-elm.la

libelev8_dbus_la_SOURCES = dbus.cc

libelev8_http_la_SOURCES = http.cc

libelev8_elm_la_SOURCES = elm.cc

libelev8_dbus_la_LIBADD = @ELEV8_LIBS@ @V8_LIBS@
libelev8_dbus_la_LDFLAGS = -module -avoid-version
libelev8_dbus_la_LIBTOOLFLAGS = --tag=disable-static


libelev8_http_la_LIBADD = @ELEV8_LIBS@ @V8_LIBS@
libelev8_http_la_LDFLAGS = -module -avoid-version
libelev8_http_la_LIBTOOLFLAGS = --tag=disable-static

libelev8_elm_la_LIBADD = @ELEV8_LIBS@ @V8_LIBS@
libelev8_elm_la_LDFLAGS = -module -avoid-version
libelev8_elm_la_LIBTOOLFLAGS = --tag=disable-static
