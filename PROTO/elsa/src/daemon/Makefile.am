
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/daemon \
-I$(top_builddir)/src/daemon \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DSYSTEM_CONFIG_DIR=\"$(sysconfdir)\" \
-DPACKAGE_BIN_DIR=\"$(libdir)/$(PACKAGE)\" \
@ELSA_CFLAGS@

sbin_PROGRAMS = elsa

internal_bindir=$(libdir)/elsa
internal_bin_PROGRAMS = elsa_wait

elsa_SOURCES =     \
elsa_config.h      \
elsa_config.c      \
elsa_session.h     \
elsa_session.c     \
elsa_xserver.h     \
elsa_xserver.c     \
elsa_pam.h         \
elsa_pam.c         \
../event/elsa_event.c \
../event/elsa_event.h \
elsa_server.h     \
elsa_server.c     \
elsa_history.h    \
elsa_history.c    \
elsa_action.h     \
elsa_action.c     \
elsa.h            \
elsa.c

elsa_LDFLAGS =
elsa_LDADD = @ELSA_LIBS@ -lpam -lrt

elsa_wait_SOURCES = \
elsa_wait.c

elsa_wait_LDFLAGS =
elsa_wait_LDADD =
