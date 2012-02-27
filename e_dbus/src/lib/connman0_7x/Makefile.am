MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@

lib_LTLIBRARIES = libeconnman0_7x.la
includes_HEADERS = E_Connman.h
includesdir = $(includedir)/e_dbus-@VMAJ@/connman0_7x

libeconnman0_7x_la_SOURCES = \
e_connman.c \
e_connman_element.c \
e_connman_manager.c \
e_connman_profile.c \
e_connman_service.c \
e_connman_technology.c

libeconnman0_7x_la_LIBADD = \
@EDBUS_LIBS@ $(top_builddir)/src/lib/dbus/libedbus.la

libeconnman0_7x_la_LDFLAGS = -version-info @version_info@ @release_info@

EXTRA_DIST = e_connman_private.h
