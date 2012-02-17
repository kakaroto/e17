AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-Wall -Wextra \
-I$(top_srcdir) \
-I$(top_srcdir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_TMP_DIR=\"/tmp\" \
@ELEV8_CFLAGS@

moddir = $(libdir)/$(PACKAGE)/$(MODULE_ARCH)
mod_LTLIBRARIES = libelev8-dbus.la

libelev8_dbus_la_SOURCES = elev8_dbus.cc

libelev8_dbus_la_LIBADD = @ELEV8_LIBS@ @V8_LIBS@
libelev8_dbus_la_LDFLAGS = -module -avoid-version -no-undefined
libelev8_dbus_la_LIBTOOLFLAGS = --tag=disable-static
