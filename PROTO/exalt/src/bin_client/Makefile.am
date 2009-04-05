## Process this file with automake to produce Makefile.in

AM_CFLAGS = \
-Wall -W -g -D_GNU_SOURCE @EXALT_CFLAGS@ \
-I$(top_srcdir)/src/lib_dbus  -I$(top_srcdir)/src/lib \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_LOCALE_DIR=\"$(localedir)\"


sbin_PROGRAMS = exalt-client

exalt_command_INCLUDES =

exalt_client_SOURCES = \
client.c \
iface_list.c \
wired.c \
wireless.c

exalt_client_LDFLAGS = @DLLDFLAGS@
exalt_client_LDADD = $(top_builddir)/src/lib_dbus/libexalt_dbus.la $(top_builddir)/src/lib/libexalt.la @EXALT_LIBS@

MAINTAINERCLEANFILES = Makefile.in
