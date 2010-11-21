MAINTAINERCLEANFILES = Makefile.in

EDBUS_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
-I$(top_srcdir)/src/lib/notification \
-I$(top_srcdir)/src/lib/connman \
-I$(top_srcdir)/src/lib/bluez \
-I$(top_srcdir)/src/lib/ofono \
-I$(top_srcdir)/src/lib/ukit \
@EDBUS_CFLAGS@

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
if BUILD_EUKIT
EUKIT_PROG = e_dbus_ukit_test
endif

bin_PROGRAMS = \
$(EDBUS_PROG) \
$(ENOTIFY_PROG) \
$(ECONNMAN_PROG) \
$(EBLUEZ_PROG) \
$(EOFONO_PROG) \
$(EUKIT_PROG)

noinst_PROGRAMS = \
$(ECONNMAN_PROG_NOINST)

e_dbus_test_SOURCES = \
test.c

e_dbus_test_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_test_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(EDBUS_LIBS)

e_dbus_test_client_SOURCES = \
test_client.c

e_dbus_test_client_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_test_client_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(EDBUS_LIBS)

if BUILD_EBLUEZ
e_dbus_bluez_test_SOURCES = e_dbus_bluez_test.c
e_dbus_bluez_test_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_bluez_test_LDADD = $(top_builddir)/src/lib/bluez/libebluez.la $(EDBUS_LIBS)
endif

if BUILD_ECONNMAN
e_dbus_connman_test_SOURCES = e_dbus_connman_test.c
e_dbus_connman_test_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_connman_test_LDADD = $(top_builddir)/src/lib/connman/libeconnman.la $(EDBUS_LIBS)

e_dbus_connman_test_api_SOURCES = e_dbus_connman_test_api.c
e_dbus_connman_test_api_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_connman_test_api_LDADD = $(top_builddir)/src/lib/connman/libeconnman.la $(EDBUS_LIBS)
endif

if BUILD_ENOTIFY

ENOTIFY_CPPFLAGS = $(EDBUS_CPPFLAGS) $(EVAS_CFLAGS)
ENOTIFY_LIBS = $(top_builddir)/src/lib/notification/libenotify.la

e_dbus_notify_SOURCES = notify.c
e_dbus_notify_CPPFLAGS = $(ENOTIFY_CPPFLAGS)
e_dbus_notify_LDADD = $(ENOTIFY_LIBS) $(EDBUS_LIBS)

e_dbus_notification_daemon_SOURCES = notification_daemon.c
e_dbus_notification_daemon_CPPFLAGS = $(ENOTIFY_CPPFLAGS)
e_dbus_notification_daemon_LDADD = $(ENOTIFY_LIBS) $(EDBUS_LIBS)

e_notify_send_SOURCES = notify-send.c
e_notify_send_CPPFLAGS = $(ENOTIFY_CPPFLAGS)
e_notify_send_LDADD =  $(ENOTIFY_LIBS) $(EDBUS_LIBS)
endif

if BUILD_EOFONO
e_dbus_ofono_test_SOURCES = e_dbus_ofono_test.c
e_dbus_ofono_test_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_ofono_test_LDADD = $(top_builddir)/src/lib/ofono/libeofono.la $(EDBUS_LIBS)
endif

if BUILD_EUKIT
e_dbus_ukit_test_SOURCES = e_dbus_ukit_test.c
e_dbus_ukit_test_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_ukit_test_LDADD = $(top_builddir)/src/lib/ukit/libeukit.la $(EDBUS_LIBS)
endif
