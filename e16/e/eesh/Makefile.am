
bin_PROGRAMS = eesh

eesh_SOURCES = E.h comms.c globals.c lists.c main.c setup.c memory.c

LDADD = $(X_LIBS) -lX11

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/intl $(X_CFLAGS)

DEFS=-DENLIGHTENMENT_ROOT=\"@ENLIGHTENMENT_ROOT@\"

uninstall-local:
	for i in $(bin_PROGRAMS); do \
	  rm -f $(exec_prefix)/bin/$$i; \
	done
