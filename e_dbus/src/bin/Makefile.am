MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\"

EDBUS_CPPFLAGS = @EDBUS_CFLAGS@

bin_PROGRAMS =

if BUILD_EDBUS_TEST
bin_PROGRAMS += e_dbus_test
endif

if BUILD_EDBUS_TEST_CLIENT
bin_PROGRAMS += e_dbus_test_client
endif

if BUILD_EDBUS_ASYNC_TEST
bin_PROGRAMS += e_dbus_async_server_test
bin_PROGRAMS += e_dbus_async_client_test
endif

if BUILD_EDBUS_BLUEZ_TEST
bin_PROGRAMS += e_dbus_bluez_test
endif

if BUILD_EDBUS_CONNMAN0_7X_TEST
bin_PROGRAMS += e_dbus_connman0_7x_test
endif

if BUILD_EDBUS_NOTIFICATION_DAEMON_TEST
bin_PROGRAMS += e_dbus_notification_daemon
endif

if BUILD_EDBUS_NOTIFY_SEND
bin_PROGRAMS += e-notify-send
endif

if BUILD_EDBUS_NOTIFY_TEST
bin_PROGRAMS += e_dbus_notify
endif

if BUILD_EDBUS_OFONO_TEST
bin_PROGRAMS += e_dbus_ofono_test
endif

if BUILD_EDBUS_UKIT_TEST
bin_PROGRAMS += e_dbus_ukit_test
endif

if BUILD_EDBUS_PERFORMANCE_TEST
bin_PROGRAMS += e_dbus_performance
endif

noinst_PROGRAMS =

if BUILD_EDBUS_CONNMAN0_7X_TEST
noinst_PROGRAMS += e_dbus_connman0_7x_test_api
endif

if BUILD_EDBUS_TEST
e_dbus_test_SOURCES = test.c
e_dbus_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
@EDBUS_TEST_CFLAGS@
e_dbus_test_LDADD = \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EDBUS_TEST_LIBS@
endif

if BUILD_EDBUS_TEST_CLIENT
e_dbus_test_client_SOURCES = test_client.c
e_dbus_test_client_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
@EDBUS_TEST_CLIENT_CFLAGS@
e_dbus_test_client_LDADD = \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EDBUS_TEST_CLIENT_LIBS@
endif

if BUILD_EDBUS_ASYNC_TEST
e_dbus_async_server_test_SOURCES = async_server_test.c
e_dbus_async_server_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
@EDBUS_ASYNC_TEST_CFLAGS@
e_dbus_async_server_test_LDADD = \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EDBUS_ASYNC_TEST_LIBS@

e_dbus_async_client_test_SOURCES = async_client_test.c
e_dbus_async_client_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
@EDBUS_ASYNC_TEST_CFLAGS@
e_dbus_async_client_test_LDADD = \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EDBUS_ASYNC_TEST_LIBS@
endif

if BUILD_EDBUS_BLUEZ_TEST
e_dbus_bluez_test_SOURCES = e_dbus_bluez_test.c
e_dbus_bluez_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
-I$(top_srcdir)/src/lib/bluez \
@EDBUS_BLUEZ_TEST_CFLAGS@
e_dbus_bluez_test_LDADD = \
$(top_builddir)/src/lib/dbus/libedbus.la \
$(top_builddir)/src/lib/bluez/libebluez.la \
@EDBUS_BLUEZ_TEST_LIBS@
endif

if BUILD_EDBUS_CONNMAN0_7X_TEST
e_dbus_connman0_7x_test_SOURCES = e_dbus_connman0_7x_test.c
e_dbus_connman0_7x_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
-I$(top_srcdir)/src/lib \
@EDBUS_CONNMAN0_7X_TEST_CFLAGS@
e_dbus_connman0_7x_test_LDADD = \
$(top_builddir)/src/lib/dbus/libedbus.la \
$(top_builddir)/src/lib/connman0_7x/libeconnman0_7x.la \
@EDBUS_CONNMAN0_7X_TEST_LIBS@

e_dbus_connman0_7x_test_api_SOURCES = e_dbus_connman0_7x_test_api.c
e_dbus_connman0_7x_test_api_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
-I$(top_srcdir)/src/lib \
@EDBUS_CONNMAN0_7X_TEST_CFLAGS@
e_dbus_connman0_7x_test_api_LDADD = \
$(top_builddir)/src/lib/dbus/libedbus.la \
$(top_builddir)/src/lib/connman0_7x/libeconnman0_7x.la \
@EDBUS_CONNMAN0_7X_TEST_LIBS@
endif

ENOTIFY_LIBS = $(top_builddir)/src/lib/notification/libenotify.la

if BUILD_EDBUS_NOTIFICATION_DAEMON_TEST
e_dbus_notification_daemon_SOURCES = notification_daemon.c
e_dbus_notification_daemon_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
-I$(top_srcdir)/src/lib/notification \
@EDBUS_NOTIFICATION_DAEMON_TEST_CFLAGS@
e_dbus_notification_daemon_LDADD = \
$(ENOTIFY_LIBS) \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EDBUS_NOTIFICATION_DAEMON_TEST_LIBS@
endif

if BUILD_EDBUS_NOTIFY_SEND
e_notify_send_SOURCES = notify-send.c
e_notify_send_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
-I$(top_srcdir)/src/lib/notification \
@EDBUS_NOTIFY_SEND_CFLAGS@
e_notify_send_LDADD = \
$(ENOTIFY_LIBS) \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EDBUS_NOTIFY_SEND_LIBS@
endif

if BUILD_EDBUS_NOTIFY_TEST
EXTRA_DIST = logo.png
logodir = $(datadir)/e_dbus
logo_DATA = logo.png
e_dbus_notify_SOURCES = notify.c
e_dbus_notify_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
-I$(top_srcdir)/src/lib/notification \
@EDBUS_NOTIFY_TEST_CFLAGS@
e_dbus_notify_LDADD = \
$(ENOTIFY_LIBS) \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EDBUS_NOTIFY_TEST_LIBS@
endif

if BUILD_EDBUS_OFONO_TEST
e_dbus_ofono_test_SOURCES = e_dbus_ofono_test.c
e_dbus_ofono_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
-I$(top_srcdir)/src/lib/ofono \
@EDBUS_OFONO_TEST_CFLAGS@
e_dbus_ofono_test_LDADD = \
$(top_builddir)/src/lib/dbus/libedbus.la \
$(top_builddir)/src/lib/ofono/libeofono.la \
@EDBUS_OFONO_TEST_LIBS@
endif

if BUILD_EDBUS_UKIT_TEST
e_dbus_ukit_test_SOURCES = e_dbus_ukit_test.c
e_dbus_ukit_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
-I$(top_srcdir)/src/lib/ukit \
@EDBUS_UKIT_TEST_CFLAGS@
e_dbus_ukit_test_LDADD = \
$(top_builddir)/src/lib/dbus/libedbus.la \
$(top_builddir)/src/lib/ukit/libeukit.la \
@EDBUS_UKIT_TEST_LIBS@
endif

if BUILD_EDBUS_PERFORMANCE_TEST
e_dbus_performance_SOURCES = performance.c
e_dbus_performance_CFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
@EDBUS_PERFORMANCE_TEST_CFLAGS@
e_dbus_performance_LDADD = \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EDBUS_PERFORMANCE_TEST_LIBS@
endif

