AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@CONTACTS_CFLAGS@

bin_PROGRAMS = contacts

contacts_SOURCES = main.c
contacts_LDADD = @CONTACTS_LIBS@
