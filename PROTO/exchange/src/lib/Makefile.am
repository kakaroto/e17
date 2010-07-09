MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -Wall \
-I. \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EFL_EXCHANGE_BUILD@ \
@EXCHANGE_CFLAGS@ \
@EXCHANGE_CPPFLAGS@

lib_LTLIBRARIES = libexchange.la

EXCHANGEHEADERS = \
exchange_local_theme.h \
exchange_remote_theme.h \
exchange_login.h \
exchange_theme_group.h \
exchange_private.h \
exchange_async.h \
Exchange.h

EXCHANGESOURCES = \
exchange.c \
exchange_local_theme.c \
exchange_remote_theme.c \
exchange_login.c \
exchange_theme_group.c \
exchange_async.c \
$(EXCHANGEHEADERS)

libexchange_la_SOURCES = \
	$(EXCHANGESOURCES)

includes_HEADERS = $(EXCHANGEHEADERS)
includesdir = $(includedir)/exchange-@VMAJ@

libexchange_la_LIBADD = @EXCHANGE_LIBS@
libexchange_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -version-info @version_info@
