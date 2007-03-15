MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir)/src/lib/dbus \
-I$(top_srcdir)/src/lib/hal \
@ECORE_CFLAGS@ \
@EFREET_CFLAGS@ \
@EWL_CFLAGS@ \
@DBUS_CFLAGS@ \
-g -Werror

LIBS = \
@ECORE_LIBS@ \
@EFREET_LIBS@ \
@EWL_LIBS@ \
@DBUS_LIBS@

bin_PROGRAMS = e_dbus_test \
							 e_dbus_hal

e_dbus_test_SOURCES = \
test.c

e_dbus_test_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(LIBS)
e_dbus_test_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la


HALHEADERS = E_Hal.h

HALSOURCES = \
hal.c

e_dbus_hal_SOURCES = \
	$(HALSOURCES)

e_dbus_hal_LDADD = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/hal/libehal.la $(LIBS) 
e_dbus_hal_DEPENDENCIES = $(top_builddir)/src/lib/dbus/libedbus.la $(top_builddir)/src/lib/hal/libehal.la
