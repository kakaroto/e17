MAINTAINERCLEANFILES = Makefile.in

EMAIL_CFLAGS = \
-I$(top_srcdir)/src/lib \
@EMAIL_CFLAGS@
AM_CPPFLAGS = -I$(top_srcdir)/src/lib @EMAIL_CFLAGS@

noinst_PROGRAMS = test_pop3 test_smtp
EXTRA_PROGRAMS = test_pop3 test_smtp

test_pop3_SOURCES = test_pop3.c getpass_x.c
test_smtp_SOURCES = test_smtp.c getpass_x.c
test_pop3_LDADD = $(top_builddir)/src/lib/libemail.la @EMAIL_LIBS@
test_smtp_LDADD = $(top_builddir)/src/lib/libemail.la @EMAIL_LIBS@
