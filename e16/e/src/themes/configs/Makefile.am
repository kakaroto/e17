SUBDIRS = pix backup

configdata_DATA = \
actionclasses.cfg \
actionclasses.cfg.pl \
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
keybindings.cfg.pl \
menus.cfg \
menus.cfg.pl \
keybindings.gmc.cfg \
keybindings.nogmc.cfg \
keybindings.gmc.cfg.pl \
keybindings.nogmc.cfg.pl \
control.kde.cfg \
control.nokde.cfg \
control.rh.cfg \
keybindings.rh.cfg \
keybindings.rh.cfg.pl \
menus.rh.cfg

configdatadir = $(ENLIGHTENMENT_ROOT)/config

EXTRA_DIST = $(configdata_DATA) config.ja

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(configdatadir)
# Japanese
	gzip -d -c < $(srcdir)/config.ja | (cd $(DESTDIR)$(configdatadir); tar -xmf -)

uninstall-local:
# Japanese
	-gzip -d -c < $(srcdir)/config.ja | (cd $(DESTDIR)$(configdatadir); rm -f `tar -tf -` 2>/dev/null)
