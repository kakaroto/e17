MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\"

INCLUDES = \
-I. \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EKBD_CFLAGS@

includes_HEADERS = Ekbd.h
includesdir = $(includedir)/ekbd

lib_LTLIBRARIES = libekbd.la

libekbd_la_SOURCES  = \
ekbd_private.h \
ekbd_send.h   \
ekbd_send.c   \
ekbd_layout.h \
ekbd_layout.c \
ekbd_smart.c

libekbd_la_DEPENDENCIES = $(top_builddir)/config.h
libekbd_la_LDFLAGS = @EKBD_LIBS@

clean-local:
	@rm -rf *.gcno

