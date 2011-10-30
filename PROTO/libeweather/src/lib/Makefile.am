
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\"

INCLUDES = \
-I. \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@E_CFLAGS@


include_HEADERS = EWeather.h \
    EWeather_Plugins.h \
    EWeather_Smart.h

lib_LTLIBRARIES = libeweather.la

libeweather_la_SOURCES  = \
eweather.c \
plugins.c \
eweather_smart.c

libeweather_la_DEPENDENCIES = $(top_builddir)/config.h
libeweather_la_LDFLAGS = @E_LIBS@ -lm

clean-local:
	@rm -rf *.gcno
