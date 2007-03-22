MAINTAINERCLEANFILES = Makefile.in

INCLUDES =  \
@ECORE_CFLAGS@ \
@DBUS_CFLAGS@ \
-I $(top_builddir)/src/lib/dbus

lib_LTLIBRARIES = libenm.la
include_HEADERS = E_Nm.h

libenm_la_SOURCES = \
e_nm_private.h \
e_nm.c \
e_nm_manager.c \
e_nm_device.c \
e_nm_util.c 


libenm_la_LIBADD = \
@ECORE_LIBS@ \
@DBUS_LIBS@ \
$(top_builddir)/src/lib/dbus/libedbus.la


libenm_la_DEPENDENCIES = \
$(top_builddir)/src/lib/dbus/libedbus.la
