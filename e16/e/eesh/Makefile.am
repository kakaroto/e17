if FSSTD
bindir = @bindir@
ENLIGHTENMENT_ROOT=$(pkglibdir)
else
bindir=$(prefix)/enlightenment/bin
ENLIGHTENMENT_ROOT=$(prefix)/enlightenment
endif

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

INCLUDES=-I$(top_srcdir) $(IMLIB_CFLAGS) -I$(includedir) -I..

DEFS=-DENLIGHTENMENT_ROOT=\"@ENLIGHTENMENT_ROOT@\"

#install-exec-local:
#	if [ x@USE_FSSTD@ = "xno" ]; then \
#	  ../mkinstalldirs $(exec_prefix)/bin; \
#	  for i in $(bin_PROGRAMS); do \
#	    rm -f $(exec_prefix)/bin/$$i; \
#	    $(LN_S) $(bindir)/$$i $(exec_prefix)/bin/$$i; \
#	  done; \
#	fi

uninstall-local:
	for i in $(bin_PROGRAMS); do \
	  rm -f $(exec_prefix)/bin/$$i; \
	done
