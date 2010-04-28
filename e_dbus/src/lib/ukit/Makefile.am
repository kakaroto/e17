MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@ @EINA_CFLAGS@

if BUILD_EUKIT

lib_LTLIBRARIES = libeukit.la
include_HEADERS = E_Ukit.h

noinst_HEADERS = e_ukit_private.h e_ukit_privfunc.h

libeukit_la_SOURCES = \
E_Ukit.h \
e_udisks.c  \
e_upower.c  \
e_ukit_util.c \
e_ukit_main.c

libeukit_la_LIBADD = \
@EDBUS_LIBS@ \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EINA_LIBS@
libeukit_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -version-info @version_info@ @eukit_release_info@

endif
