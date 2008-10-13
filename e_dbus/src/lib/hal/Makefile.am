MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@ @EINA_CFLAGS@

if BUILD_EHAL

lib_LTLIBRARIES = libehal.la
include_HEADERS = E_Hal.h

libehal_la_SOURCES = \
E_Hal.h \
e_hal_device.c  \
e_hal_manager.c \
e_hal_util.c 

libehal_la_LIBADD = \
@EDBUS_LIBS@ \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EINA_LIBS@
libehal_la_LDFLAGS      = -version-info @version_info@

endif
