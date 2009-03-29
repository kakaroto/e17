AUTOMAKE_OPTIONS = foreign

DISTCLEANFILES = install-sh missing
MAINTAINERCLEANFILES = aclocal.m4 configure Makefile.in

THEME = winter
THEME_FILES = $(THEME).etheme

BUILT_SOURCES = $(THEME).etheme
EXTRA_DIST = etheme-$(THEME).spec $(THEME_FILES)

THEME_DIR = $(DESTDIR)$(datadir)/e16/themes/$(THEME)

install-data-local:
	$(mkinstalldirs) $(THEME_DIR)
	for tf in $(THEME_FILES); do \
		gzip -d -c < $(srcdir)/$$tf | (cd $(THEME_DIR); tar -xmf -); \
	done
	-if [ -n "$(USER)" ]; then chown -R $(USER) $(THEME_DIR); else chown -R root $(THEME_DIR); fi

uninstall-local:
	-for tf in $(THEME_FILES); do \
		gzip -d -c < $(srcdir)/$$tf | (cd $(THEME_DIR); \
			rm -f `tar -tf -` 2>/dev/null); \
	done

.PHONY: etheme $(THEME).etheme
etheme: $(THEME).etheme
$(THEME).etheme:
	test ! -d data || (cd data; tar -czvf ../$(THEME).etheme --exclude .svn *)
