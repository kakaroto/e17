MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# Gettext
datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

INCLUDES	= \
 -Wall -g @EXALT_CFLAGS@

lib_LTLIBRARIES = libexalt.la

EXTRA_DIST =

EXALTHEADERS = \
libexalt.h \
exalt_dns.h \
exalt_ethernet.h \
exalt_sys_conf.h \
exalt_wireless.h \
exalt_wireless_network.h \
exalt_connection.h \
exalt_vpnc.h \
iw/iwlib.h

libexalt_la_SOURCES = \
nettools/proc.c \
wpa_supplicant/wpa_ctrl.c \
libexalt.c \
exalt_dns.c \
exalt_ethernet.c \
exalt_regexp.c \
exalt_sys_conf.c \
exalt_wireless.c \
exalt_wireless_network.c \
exalt_vpnc.c \
libexalt_private.c \
libexalt_private.h \
exalt_connection.c \
exalt_wpa_supplicant.c \
iw/iwlib.c \
$(EXALTHEADERS)

installed_headersdir = $(prefix)/include/exalt
installed_headers_DATA = $(EXALTHEADERS)

libexalt_la_LIBADD  = @EXALT_LIBS@

libexalt_la_DEPENDENCIES =

libexalt_la_LDFLAGS = -version-info 1:0:0
