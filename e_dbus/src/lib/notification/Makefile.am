MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@ \
@EVAS_CFLAGS@

lib_LTLIBRARIES = libenotify.la
includes_HEADERS = E_Notify.h E_Notification_Daemon.h
includesdir = $(includedir)/e_dbus-@VMAJ@

libenotify_la_SOURCES = \
notification.c \
marshal.c \
client.c \
daemon.c

libenotify_la_LIBADD = \
@EDBUS_LIBS@ $(top_builddir)/src/lib/dbus/libedbus.la \
@EVAS_LIBS@

libenotify_la_LDFLAGS = -version-info @version_info@ @release_info@

EXTRA_DIST = e_notify_private.h
