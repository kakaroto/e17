MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@ @EVAS_CFLAGS@ @EINA_CFLAGS@

if BUILD_ENOTIFY

lib_LTLIBRARIES = libenotify.la
include_HEADERS = E_Notify.h E_Notification_Daemon.h

libenotify_la_SOURCES = \
E_Notify.h \
E_Notification_Daemon.h \
e_notify_private.h \
notification.c \
marshal.c \
client.c \
daemon.c

libenotify_la_LIBADD = \
@EDBUS_LIBS@ @EVAS_LIBS@ \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EINA_LIBS@
libenotify_la_LDFLAGS      = -version-info @version_info@

endif
