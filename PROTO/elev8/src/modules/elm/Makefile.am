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
mod_LTLIBRARIES = libelm.la

libelm_la_SOURCES = elev8_elm.cc

libelm_la_LIBADD = @ELEV8_LIBS@ @V8_LIBS@
libelm_la_LDFLAGS = -module -avoid-version -no-undefined
libelm_la_LIBTOOLFLAGS = --tag=disable-static
