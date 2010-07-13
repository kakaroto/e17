
AM_CFLAGS = -Wall -W -g -D_GNU_SOURCE @ELSA_CFLAGS@

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


elsa_INCLUDES =

elsa_LDFLAGS = @DLLDFLAGS@
elsa_LDADD = @ELSA_LIBS@ -lpam

MAINTAINERCLEANFILES = Makefile.in
