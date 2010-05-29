MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@

if BUILD_EHAL

lib_LTLIBRARIES = libehal.la
include_HEADERS = E_Hal.h

libehal_la_SOURCES = \
e_hal_device.c  \
e_hal_manager.c \
e_hal_util.c \
e_hal_main.c

libehal_la_LIBADD = \
$(top_builddir)/src/lib/dbus/libedbus.la

libehal_la_LDFLAGS = -version-info @version_info@ @release_info@

endif

EXTRA_DIST = e_hal_private.h
