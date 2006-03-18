SUBDIRS = pix

configdata_DATA = \
actionclasses.cfg \
backup-borders.cfg \
backup-colormodifiers.cfg \
backup-cursors.cfg \
backup-imageclasses.cfg \
backup-textclasses.cfg \
backup-tooltips.cfg \
bindings.cfg \
definitions \
matches.cfg \
menus.cfg \
e16keyedit.db

configdatadir = $(ENLIGHTENMENT_ROOT)/config

TRANSLATIONS = # config.ja config.pl config.ko
EXTRA_DIST = $(configdata_DATA) $(TRANSLATIONS) strings.c

no-install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(configdatadir)
	for f in $(TRANSLATIONS); do \
		gzip -d -c < $(srcdir)/$$f | \
			(cd $(DESTDIR)$(configdatadir); tar -xmf -) \
	done

no-uninstall-local:
	for f in $(TRANSLATIONS); do \
		gzip -d -c < $(srcdir)/$$f | \
			(cd $(DESTDIR)$(configdatadir); rm -f `tar -tf -` 2>/dev/null) \
	done
