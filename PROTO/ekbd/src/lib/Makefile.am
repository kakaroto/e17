MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EKBD_CFLAGS@

includes_HEADERS = Ekbd.h
includesdir = $(includedir)/ekbd

lib_LTLIBRARIES = libekbd.la

libekbd_la_SOURCES  = \
ekbd_private.h \
ekbd_layout.h \
ekbd_layout.c \
ekbd_smart.c

libekbd_la_LIBADD = @EKBD_LIBS@
libekbd_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -version-info @version_info@

if HAVE_X11
libekbd_la_LIBADD += @EKBD_X11_CFLAGS@
libekbd_la_LDFLAGS += @EKBD_X11_LIBS@
libekbd_la_SOURCES += ekbd_send.h ekbd_send.c
endif
