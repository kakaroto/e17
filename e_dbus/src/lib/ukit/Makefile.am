MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@

if BUILD_EUKIT

lib_LTLIBRARIES = libeukit.la
includes_HEADERS = E_Ukit.h
includesdir = $(includedir)/e_dbus-@VMAJ@

libeukit_la_SOURCES = \
e_udisks.c  \
e_upower.c  \
e_ukit_util.c \
e_ukit_main.c \
e_ukit_private_util.c

libeukit_la_LIBADD = \
@EDBUS_LIBS@ $(top_builddir)/src/lib/dbus/libedbus.la

libeukit_la_LDFLAGS = -version-info @version_info@ @release_info@

endif

EXTRA_DIST = e_ukit_private.h
