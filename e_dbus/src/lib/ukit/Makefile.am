MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@

if BUILD_EUKIT

lib_LTLIBRARIES = libeukit.la
include_HEADERS = E_Ukit.h

libeukit_la_SOURCES = \
e_udisks.c  \
e_upower.c  \
e_ukit_util.c \
e_ukit_main.c \
e_ukit_private_util.c

libeukit_la_LIBADD = \
$(top_builddir)/src/lib/dbus/libedbus.la

libeukit_la_LDFLAGS = -version-info @eukit_version_info@ @eukit_release_info@

endif

EXTRA_DIST = e_ukit_private.h
