
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/bin \
-I$(top_builddir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DSYSTEM_CONFIG_DIR=\"$(sysconfdir)\" \
@ELSA_CLIENT_CFLAGS@

bin_PROGRAMS = elsa_client

elsa_client_SOURCES = \
elsa_client.h         \
elsa_client.c         \
elsa_connect.h        \
elsa_connect.c        \
elsa_gui.h            \
elsa_gui.c

elsa_client_LDFLAGS =
elsa_client_LDADD = @ELSA_CLIENT_LIBS@

