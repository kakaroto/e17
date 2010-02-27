MAINTAINERCLEANFILES = Makefile.in

EDBUS_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
-I$(top_srcdir)/src/lib/notification \
-I$(top_srcdir)/src/lib/connman \
-I$(top_srcdir)/src/lib/bluez \
-I$(top_srcdir)/src/lib/ofono \
@EDBUS_CFLAGS@ \
@EINA_CFLAGS@

LIBS = \
@EDBUS_LIBS@ \
@EINA_LIBS@

EDBUS_PROG = e_dbus_test e_dbus_test_client
if BUILD_ENOTIFY
ENOTIFY_PROG = e_dbus_notify e_dbus_notification_daemon e-notify-send
endif
if BUILD_ECONNMAN
ECONNMAN_PROG = e_dbus_connman_test
ECONNMAN_PROG_NOINST = e_dbus_connman_test_api
endif
if BUILD_EBLUEZ
EBLUEZ_PROG = e_dbus_bluez_test
endif
if BUILD_EOFONO
EOFONO_PROG = e_dbus_ofono_test
endif

bin_PROGRAMS = \
$(EDBUS_PROG) \
$(ENOTIFY_PROG) \
$(ECONNMAN_PROG) \
$(EBLUEZ_PROG) \
$(EOFONO_PROG)

noinst_PROGRAMS = \
$(ECONNMAN_PROG_NOINST)

e_dbus_test_SOURCES = \
test.c

e_dbus_test_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_test_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la
e_dbus_test_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la

e_dbus_test_client_SOURCES = \
test_client.c

e_dbus_test_client_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_test_client_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la
e_dbus_test_client_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la

if BUILD_ENOTIFY
e_dbus_notify_SOURCES = \
	notify.c

e_dbus_notify_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_notify_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/notification/libenotify.la
e_dbus_notify_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/notification/libenotify.la

e_dbus_notification_daemon_SOURCES = \
	notification_daemon.c

e_dbus_notification_daemon_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_notification_daemon_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/notification/libenotify.la
e_dbus_notification_daemon_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/notification/libenotify.la

e_notify_send_SOURCES = \
	notify-send.c

e_notify_send_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_notify_send_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/notification/libenotify.la
e_notify_send_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/notification/libenotify.la
endif

if BUILD_ECONNMAN
e_dbus_connman_test_SOURCES = e_dbus_connman_test.c
e_dbus_connman_test_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_connman_test_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/connman/libeconnman.la
e_dbus_connman_test_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/connman/libeconnman.la

e_dbus_connman_test_api_SOURCES = e_dbus_connman_test_api.c
e_dbus_connman_test_api_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_connman_test_api_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/connman/libeconnman.la
e_dbus_connman_test_api_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/connman/libeconnman.la
endif

if BUILD_EBLUEZ
e_dbus_bluez_test_SOURCES = e_dbus_bluez_test.c
e_dbus_bluez_test_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_bluez_test_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/bluez/libebluez.la
e_dbus_bluez_test_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/bluez/libebluez.la
endif

if BUILD_EOFONO
e_dbus_ofono_test_SOURCES = e_dbus_ofono_test.c
e_dbus_ofono_test_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_ofono_test_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/ofono/libeofono.la
e_dbus_ofono_test_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/ofono/libeofono.la
endif
