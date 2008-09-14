## Process this file with automake to produce Makefile.in

AM_CFLAGS = \
-Wall -W -g -D_GNU_SOURCE @EXALT_CFLAGS@ \
-I$(top_srcdir)/src/lib


sbin_PROGRAMS = exalt-daemon

exalt_daemon_INCLUDES = \
daemon.h \
cb_ethernet.h \
cb_wireless.h \
cb_wirelessnetwork.h \
define.h \
cb_dns.h \
boot_process.h \
cb_bootprocess.h

exalt_daemon_SOURCES = \
daemon.c \
cb_ethernet.c \
cb_wireless.c \
cb_wirelessnetwork.c \
cb_dns.c \
boot_process.c \
cb_bootprocess.c

exalt_daemon_LDFLAGS = @DLLDFLAGS@
exalt_daemon_LDADD = $(top_builddir)/src/lib/libexalt.la @EXALT_LIBS@

MAINTAINERCLEANFILES = Makefile.in
