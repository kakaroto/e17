MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@

if BUILD_ECONNMAN

lib_LTLIBRARIES = libeconnman.la
include_HEADERS = E_Connman.h

libeconnman_la_SOURCES = \
e_connman.c \
e_connman_device.c \
e_connman_element.c \
e_connman_manager.c \
e_connman_network.c \
e_connman_profile.c \
e_connman_service.c \
e_connman_technology.c

libeconnman_la_LIBADD = \
$(top_builddir)/src/lib/dbus/libedbus.la

libeconnman_la_LDFLAGS = -version-info @econnman_version_info@ @econnman_release_info@

endif

EXTRA_DIST = e_connman_private.h
