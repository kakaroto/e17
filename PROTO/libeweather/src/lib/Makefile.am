MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPLUGINS_DIR=\"$(pkglibdir)/plugins\" \
@EFL_EWEATHER_BUILD@ \
@EWEATHER_CFLAGS@

include_HEADERS = \
EWeather.h \
EWeather_Plugins.h \
EWeather_Smart.h

lib_LTLIBRARIES = libeweather.la

libeweather_la_SOURCES  = \
eweather.c \
plugins.c \
eweather_smart.c

libeweather_la_LIBADD = @EWEATHER_LIBS@ -lm
libeweather_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -version-info @version_info@
