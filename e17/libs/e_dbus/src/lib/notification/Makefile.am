MAINTAINERCLEANFILES = Makefile.in
INCLUDES =  \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@

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
@EDBUS_LIBS@ \
$(top_builddir)/src/lib/dbus/libedbus.la
