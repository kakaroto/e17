MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I $(top_srcdir)/src/lib/dbus \
@EDBUS_CFLAGS@ @EINA_CFLAGS@

if BUILD_ENM

lib_LTLIBRARIES = libenm.la
include_HEADERS = E_Nm.h

libenm_la_SOURCES = \
e_nm_private.h \
e_nm.c \
e_nm_manager.c \
e_nm_device.c \
e_nm_ip4_config.c \
e_nm_util.c


libenm_la_LIBADD = \
@EDBUS_LIBS@ \
$(top_builddir)/src/lib/dbus/libedbus.la \
@EINA_LIBS@
libenm_la_LDFLAGS      = -version-info @version_info@


libenm_la_DEPENDENCIES = \
$(top_builddir)/src/lib/dbus/libedbus.la

endif
