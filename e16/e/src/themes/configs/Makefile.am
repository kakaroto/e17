SUBDIRS = pix backup

configdata_DATA = \
actionclasses.cfg \
backup-borders.cfg \
backup-colormodifiers.cfg \
backup-cursors.cfg \
backup-imageclasses.cfg \
backup-textclasses.cfg \
backup-tooltips.cfg \
definitions \
control.cfg \
icondefs.cfg \
keybindings.cfg \
menus.cfg

configdatadir = $(ENLIGHTENMENT_ROOT)/config

TRANSLATIONS = config.ja config.pl
EXTRA_DIST = $(configdata_DATA) $(TRANSLATIONS)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(configdatadir)
	for f in $(TRANSLATIONS); do \
		gzip -d -c < $(srcdir)/$$f | \
			(cd $(DESTDIR)$(configdatadir); tar -xmf -) \
	done

uninstall-local:
	for f in $(TRANSLATIONS); do \
		gzip -d -c < $(srcdir)/$$f | \
			(cd $(DESTDIR)$(configdatadir); rm -f `tar -tf -` 2>/dev/null) \
	done
