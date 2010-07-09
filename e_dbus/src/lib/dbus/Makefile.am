MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
@EFL_EDBUS_BUILD@ \
@DBUS_VERSION_CFLAGS@ \
@EDBUS_CFLAGS@


lib_LTLIBRARIES = libedbus.la
includes_HEADERS = E_DBus.h
includesdir = $(includedir)/e_dbus-@VMAJ@

libedbus_la_SOURCES = \
e_dbus.c \
e_dbus_message.c \
e_dbus_methods.c \
e_dbus_interfaces.c \
e_dbus_object.c \
e_dbus_util.c \
e_dbus_signal.c


libedbus_la_LIBADD = @EDBUS_LIBS@
libedbus_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -version-info @version_info@ @release_info@

EXTRA_DIST = e_dbus_private.h
