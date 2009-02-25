AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ELEMENTARY_CFLAGS@ \
@EFSO_CFLAGS@

bin_PROGRAMS = elementary_sms

elementary_smsdir = $(libdir)
elementary_sms_LTLIBRARIES = elementary_sms.la
elementary_sms_la_SOURCES = main.c message.c list.c edit.c contact.c data.c common.h
elementary_sms_la_LIBADD = @ELEMENTARY_LIBS@ @EFSO_LIBS@
elementary_sms_la_CFLAGS =
elementary_sms_la_LDFLAGS = -module -avoid-version -no-undefined
elementary_sms_SOURCES = main.c common.h
elementary_sms_LDADD = @ELEMENTARY_LIBS@ @EFSO_LIBS@
elementary_sms_CFLAGS = -DELM_LIB_QUICKLAUNCH=1
