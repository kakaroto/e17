AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@SMS_CFLAGS@

bin_PROGRAMS = sms

sms_SOURCES = main.c
sms_LDADD = @SMS_LIBS@
