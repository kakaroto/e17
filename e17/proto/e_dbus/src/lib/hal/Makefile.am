INCLUDES =  \
@EDBUS_CFLAGS@ \
-I $(top_srcdir)/src/lib/dbus

lib_LTLIBRARIES = libehal.la
include_HEADERS = E_Hal.h

libehal_la_SOURCES = \
E_Hal.h \
e_hal_private.h \
e_hal_device.c  \
e_hal_manager.c \
e_hal_util.c 

libehal_la_LIBADD = \
@EDBUS_LIBS@ \
$(top_builddir)/src/lib/dbus/libedbus.la
