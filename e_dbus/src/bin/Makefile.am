MAINTAINERCLEANFILES = Makefile.in

EDBUS_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dbus \
-I$(top_srcdir)/src/lib/hal \
-I$(top_srcdir)/src/lib/nm \
-I$(top_srcdir)/src/lib/notification \
-I$(top_srcdir)/src/lib/connman \
@EDBUS_CFLAGS@ \
@EINA_CFLAGS@

LIBS = \
@EDBUS_LIBS@ \
@EINA_LIBS@

EDBUS_PROG = e_dbus_test e_dbus_test_client
if BUILD_EHAL
EHAL_PROG = e_dbus_hal
endif
if BUILD_ENM
ENM_PROG = e_dbus_nm
endif
if BUILD_ENOTIFY
ENOTIFY_PROG = e_dbus_notify e_dbus_notification_daemon e-notify-send
endif
if BUILD_ECONNMAN
ECONNMAN_PROG = e_dbus_connman_test
ECONNMAN_PROG_NOINST = e_dbus_connman_test_api
endif

bin_PROGRAMS = \
$(EDBUS_PROG) \
$(EHAL_PROG) \
$(ENM_PROG) \
$(ENOTIFY_PROG) \
$(ECONNMAN_PROG)

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


if BUILD_EHAL
HALSOURCES = \
hal.c

e_dbus_hal_CPPFLAGS = \
$(EDBUS_CPPFLAGS) \
@HAL_TEST_GUI_CFLAGS@

e_dbus_hal_LIBS = \
$(top_builddir)/src/lib/dbus/libedbus.la \
@HAL_TEST_GUI_LIBS@

e_dbus_hal_SOURCES = \
	$(HALSOURCES)

e_dbus_hal_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/hal/libehal.la $(e_dbus_hal_LIBS) 
e_dbus_hal_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/hal/libehal.la
endif


if BUILD_ENM
NMSOURCES = \
nm.c

e_dbus_nm_SOURCES = \
	$(NMSOURCES)

e_dbus_nm_CPPFLAGS = $(EDBUS_CPPFLAGS)
e_dbus_nm_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/nm/libenm.la
e_dbus_nm_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/nm/libenm.la
endif


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
