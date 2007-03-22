INCLUDES =  \
@ECORE_CFLAGS@ \
@DBUS_CFLAGS@ \
-I $(top_builddir)/src/lib/dbus

lib_LTLIBRARIES = libehal.la
include_HEADERS = E_Hal.h

libehal_la_SOURCES = \
E_Hal.h \
e_hal_private.h \
e_hal_device.c  \
e_hal_manager.c \
e_hal_util.c 

libehal_la_LIBADD = \
@ECORE_LIBS@ \
@DBUS_LIBS@ \
$(top_builddir)/src/lib/dbus/libedbus.la
