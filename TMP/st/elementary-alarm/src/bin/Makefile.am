AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ALARM_CFLAGS@

bin_PROGRAMS = elementary_alarm

elementary_alarm_SOURCES = main.c
elementary_alarm_LDADD = @ALARM_LIBS@
