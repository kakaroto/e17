## Process this file with automake to produce Makefile.in

bin_PROGRAMS = efsdsh
bin_SCRIPTS = magic2db

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile magic.db pattern.db magic2db efsdsh

INCLUDES = -I$(top_srcdir)/efsd

efsdsh_SOURCES = efsdsh.c
efsdsh_LDADD = -L$(top_builddir)/efsd/.libs -lefsd


all-local: magic.db pattern.db

magic.db: $(top_srcdir)/tools/magic.txt magic2db
	chmod +x magic2db
	if test -f magic.db; then \
	  rm -f magic.db; \
	fi
	./magic2db $(top_srcdir)/tools/magic.txt magic.db

pattern.db: $(top_srcdir)/tools/build_pattern_db
	chmod +x build_pattern_db
	./build_pattern_db pattern.db

distclean-local:
	rm -f magic.db
	rm -f patterns.db

clean-local:
	rm -f magic.db
	rm -f patterns.db

maintainer-clean-local:
	rm -f magic.db
	rm -f patterns.db

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	if test -e magic.db; then \
	  $(INSTALL) magic.db $(DESTDIR)$(pkgdatadir)/magic.db; \
	fi
	if test -e pattern.db; then \
	  $(INSTALL) pattern.db $(DESTDIR)$(pkgdatadir)/pattern.db; \
	fi

EXTRA_DIST = magic.txt
