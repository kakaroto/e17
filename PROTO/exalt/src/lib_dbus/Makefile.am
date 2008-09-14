MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# Gettext
datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

INCLUDES	= \
 -Wall -g @EXALT_CFLAGS@ \
-I$(top_srcdir)/src/lib

lib_LTLIBRARIES = libexalt_dbus.la

EXTRA_DIST =

EXALT_DBUSHEADERS = \
libexalt_dbus.h \
define.h \
exalt_dbus_ethernet.h \
exalt_dbus_wireless.h\
exalt_dbus_wireless_network.h \
exalt_dbus_dns.h \
exalt_dbus_bootprocess.h

libexalt_dbus_la_SOURCES = \
libexalt_dbus.c \
exalt_dbus_ethernet.c \
exalt_dbus_wireless.c \
exalt_dbus_wireless_network.c \
libexalt_dbus_private.h \
libexalt_dbus_private.c \
exalt_dbus_dns.c \
exalt_dbus_bootprocess.c \
$(EXALTHEADERS)

installed_headersdir = $(prefix)/include/exalt_dbus
installed_headers_DATA = $(EXALT_DBUSHEADERS)

libexalt_dbus_la_LIBADD  = $(top_builddir)/src/lib/libexalt.la @EXALT_LIBS@

libexalt_dbus_la_DEPENDENCIES = $(top_builddir)/src/lib/libexalt.la
libexalt_dbus_la_LDFLAGS = -version-info 1:0:0
