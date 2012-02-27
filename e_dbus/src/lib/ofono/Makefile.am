MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@

lib_LTLIBRARIES = libeofono.la
includes_HEADERS = E_Ofono.h
includesdir = $(includedir)/e_dbus-@VMAJ@

libeofono_la_SOURCES = \
e_ofono.c \
e_ofono_element.c \
e_ofono_manager.c \
e_ofono_modem.c \
e_ofono_network_reg.c \
e_ofono_sms.c

libeofono_la_LIBADD = \
@EDBUS_LIBS@ $(top_builddir)/src/lib/dbus/libedbus.la

libeofono_la_LDFLAGS = -version-info @version_info@ @release_info@

EXTRA_DIST = e_ofono_private.h
