MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
-I $(top_srcdir)/src/lib/ukit \
@EDBUS_CFLAGS@

lib_LTLIBRARIES = libehal.la
includes_HEADERS = E_Hal.h
includesdir = $(includedir)/e_dbus-@VMAJ@

libehal_la_SOURCES = \
e_hal_device.c  \
e_hal_manager.c \
e_hal_util.c \
e_hal_main.c

libehal_la_LIBADD = \
$(top_builddir)/src/lib/dbus/libedbus.la

libehal_la_LDFLAGS = -version-info @version_info@ @release_info@

EXTRA_DIST = e_hal_private.h
