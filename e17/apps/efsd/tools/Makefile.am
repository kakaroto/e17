## Process this file with automake to produce Makefile.in

bin_SCRIPTS = magic2db

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile

all-local: magic.db pattern.db

magic.db: magic.txt
	chmod 755 magic2db
	if test -e magic.db; then \
	  rm -f magic.db; \
	fi
	./magic2db magic.txt magic.db

pattern.db: build_pattern_db.sh
	chmod 755 build_pattern_db.sh
	./build_pattern_db.sh pattern.db

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

EXTRA_DIST = magic.txt build_pattern_db.sh
