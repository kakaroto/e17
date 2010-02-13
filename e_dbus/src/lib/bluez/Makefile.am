MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@ @EVAS_CFLAGS@ @EINA_CFLAGS@

if BUILD_EBLUEZ

lib_LTLIBRARIES = libebluez.la
include_HEADERS = E_Bluez.h

libebluez_la_SOURCES = \
E_Bluez.h \
e_bluez_private.h \
e_bluez.c \
e_bluez_element.c \
e_bluez_manager.c \
e_bluez_adapter.c

libebluez_la_LIBADD = \
@EDBUS_LIBS@ @EVAS_LIBS@ \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EINA_LIBS@

libebluez_la_LDFLAGS = -version-info @version_info@ @ebluez_release_info@

endif
