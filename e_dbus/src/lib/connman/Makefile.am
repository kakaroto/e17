MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@ @EVAS_CFLAGS@ @EINA_CFLAGS@

if BUILD_ECONNMAN

lib_LTLIBRARIES = libeconnman.la
include_HEADERS = E_Connman.h

libeconnman_la_SOURCES = \
E_Connman.h \
e_connman_private.h \
e_connman.c \
e_connman_connection.c \
e_connman_device.c \
e_connman_element.c \
e_connman_manager.c \
e_connman_network.c \
e_connman_profile.c

libeconnman_la_LIBADD = \
@EDBUS_LIBS@ @EVAS_LIBS@ \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EINA_LIBS@

libeconnman_la_LDFLAGS = -version-info @version_info@

endif
