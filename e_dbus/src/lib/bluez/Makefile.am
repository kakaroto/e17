MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@

lib_LTLIBRARIES = libebluez.la
includes_HEADERS = E_Bluez.h
includesdir = $(includedir)/e_dbus-@VMAJ@

libebluez_la_SOURCES = \
e_bluez.c \
e_bluez_element.c \
e_bluez_manager.c \
e_bluez_adapter.c \
e_bluez_device.c \
e_bluez_devicefound.c

libebluez_la_LIBADD = \
@EDBUS_LIBS@ $(top_builddir)/src/lib/dbus/libedbus.la

libebluez_la_LDFLAGS = -version-info @version_info@ @release_info@

EXTRA_DIST = e_bluez_private.h
