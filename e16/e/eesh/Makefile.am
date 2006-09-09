
bin_PROGRAMS = eesh

eesh_SOURCES = E.h comms.c main.c

LDADD = $(X_LIBS) -lX11

INCLUDES = -I$(top_builddir) -I$(top_srcdir) $(X_CFLAGS)

uninstall-local:
	for i in $(bin_PROGRAMS); do \
	  rm -f $(exec_prefix)/bin/$$i; \
	done
