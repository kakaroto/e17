AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS =\
 -I$(top_srcdir) \
 -I$(top_srcdir)/src \
 -I$(top_srcdir)/src/bin \
 -DPACKAGE=\"eyesight\" \
 -DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
 -DPACKAGE_PLUGIN_DIR=\"$(libdir)/$(PACKAGE)/plugins\" \
 -DLOCALEDIR=\"$(localedir)\" \
 -Wall -Werror -g\
 @EYESIGHT_CFLAGS@

bin_PROGRAMS = eyesight

eyesight_SOURCES = main.c conf.c main_window.c error.c plugin.c file.c
eyesight_LDADD = @EYESIGHT_LIBS@ @intl_libs@ @dlopen_libs@

EXTRA_DIST = conf.h error.h eyesight_private.h file.h main.h\
	 main_window.h plugin.h plugin_private.h
