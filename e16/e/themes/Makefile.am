
THEME = winter
THEME_FILES = winter.etheme

ethemedir = $(datadir)/themes

EXTRA_DIST = $(THEME_FILES) 

THEME_DIR = $(DESTDIR)$(ENLIGHTENMENT_ROOT)/themes/$(THEME)

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
