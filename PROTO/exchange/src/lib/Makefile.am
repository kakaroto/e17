MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -Wall \
-I. \
-I$(top_srcdir)/src/lib \
@EXCHANGE_CFLAGS@ \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\"

lib_LTLIBRARIES = libexchange.la

EXCHANGEHEADERS = \
exchange_local_theme.h \
exchange_remote_theme.h \
exchange_login.h \
exchange_theme_group.h \
exchange_smart.h \
exchange_private.h \
exchange_async.h \
Exchange.h

EXCHANGESOURCES = \
exchange.c \
exchange_local_theme.c \
exchange_remote_theme.c \
exchange_login.c \
exchange_theme_group.c \
exchange_smart.c \
exchange_async.c \
$(EXCHANGEHEADERS)

libexchange_la_SOURCES = \
	$(EXCHANGESOURCES)

installed_headersdir = $(prefix)/include/exchange
installed_headers_DATA = $(EXCHANGEHEADERS)

libexchange_la_LIBADD = @EXCHANGE_LIBS@
libexchange_la_LDFLAGS = -version-info @version_info@
