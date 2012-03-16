AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-Wall -Wextra \
-I$(top_srcdir) \
-I$(top_srcdir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_TMP_DIR=\"/tmp\" \
@EDBUS_CFLAGS@

moddir = $(libdir)/$(PACKAGE)/$(MODULE_ARCH)
mod_LTLIBRARIES = libdbus.la

libdbus_la_SOURCES = elev8_dbus.cc

libdbus_la_LIBADD = @EDBUS_LIBS@ @V8_LIBS@
libdbus_la_LDFLAGS = -module -avoid-version -no-undefined
libdbus_la_LIBTOOLFLAGS = --tag=disable-static
