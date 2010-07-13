
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/bin \
-I$(top_builddir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ELSA_CFLAGS@

bin_PROGRAMS = elsa

elsa_SOURCES =     \
elsa_config.h      \
elsa_config.c      \
elsa_session.h     \
elsa_session.c     \
elsa_pam.h         \
elsa_pam.c         \
elsa_gui.h         \
elsa_gui.c         \
elsa.h             \
elsa.c

elsa_LDFLAGS =
elsa_LDADD = @ELSA_LIBS@ -lpam
