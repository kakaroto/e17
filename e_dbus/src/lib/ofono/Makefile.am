MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@ @EVAS_CFLAGS@ @EINA_CFLAGS@

if BUILD_EOFONO

lib_LTLIBRARIES = libeofono.la
include_HEADERS = E_Ofono.h

libeofono_la_SOURCES = \
E_Ofono.h \
e_ofono_private.h \
e_ofono.c \
e_ofono_element.c \
e_ofono_manager.c \
e_ofono_modem.c \
e_ofono_network_reg.c \
e_ofono_sms.c

libeofono_la_LIBADD = \
@EDBUS_LIBS@ @EVAS_LIBS@ \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EINA_LIBS@

libeofono_la_LDFLAGS = -version-info @version_info@ @eofono_release_info@

endif
