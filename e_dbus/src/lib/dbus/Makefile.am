MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
@EDBUS_CFLAGS@ @EINA_CFLAGS@

lib_LTLIBRARIES = libedbus.la
include_HEADERS = E_DBus.h

libedbus_la_SOURCES = \
	e_dbus_private.h \
	e_dbus.c \
	e_dbus_message.c \
	e_dbus_methods.c \
	e_dbus_interfaces.c \
	e_dbus_object.c \
	e_dbus_util.c \
	e_dbus_signal.c


libedbus_la_LIBADD = \
@EDBUS_LIBS@ @EINA_LIBS@
libedbus_la_LDFLAGS      = -version-info @version_info@

