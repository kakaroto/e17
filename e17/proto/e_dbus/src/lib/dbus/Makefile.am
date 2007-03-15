INCLUDES =  \
@ECORE_CFLAGS@ \
@DBUS_CFLAGS@ \
-g -Werror

lib_LTLIBRARIES = libedbus.la

include_HEADERS = E_DBus.h

libedbus_la_SOURCES = \
	E_DBus.h \
	e_dbus.c \
	e_dbus_message.c \
	e_dbus_methods.c \
	e_dbus_object.c \
	e_dbus_signal.c


libedbus_la_LIBADD = @ECORE_LIBS@ @DBUS_LIBS@ -lm
