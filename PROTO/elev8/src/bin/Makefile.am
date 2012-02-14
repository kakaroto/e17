AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_TMP_DIR=\"/tmp\" \
@ELEV8_CFLAGS@

bin_PROGRAMS = elev8

elev8_SOURCES = \
	main.cc main.h \
	elev8_utils.cc elev8_utils.h \
	downloader.cc downloader.h \
	elev8_elm.cc \
	elev8_http.cc elev8_http.h \
	elev8_dbus.cc elev8_dbus.h

elev8_LDADD = @ELEV8_LIBS@ @V8_LIBS@
