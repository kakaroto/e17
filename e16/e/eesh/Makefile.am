
bin_PROGRAMS = eesh

eesh_SOURCES = \
	E.h			\
	comms.c                 \
	globals.c               \
	lists.c                 \
	main.c                  \
	setup.c                 \
	memory.c                \
	file.c                  \
	alert.c

LDADD = $(X_PRE_LIBS) $(X_LIBS) -lX11 $(X_EXTRA_LIBS) -lm

INCLUDES=$(IMLIB_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/intl

DEFS=-DENLIGHTENMENT_ROOT=\"@ENLIGHTENMENT_ROOT@\"

uninstall-local:
	for i in $(bin_PROGRAMS); do \
	  rm -f $(exec_prefix)/bin/$$i; \
	done
