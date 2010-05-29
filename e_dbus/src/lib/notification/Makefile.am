MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@ \
@EVAS_CFLAGS@

if BUILD_ENOTIFY

lib_LTLIBRARIES = libenotify.la
include_HEADERS = E_Notify.h E_Notification_Daemon.h

libenotify_la_SOURCES = \
notification.c \
marshal.c \
client.c \
daemon.c

libenotify_la_LIBADD = \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EVAS_LIBS@

libenotify_la_LDFLAGS = -version-info @version_info@ @release_info@

endif

EXTRA_DIST = e_notify_private.h
