SUBDIRS = configs

if FSSTD
ethemedir = $(prefix)/share/enlightenment/themes
else
ethemedir = $(prefix)/enlightenment/themes
endif

EXTRA_DIST = BrushedMetal-Tigert.etheme ShinyMetal.etheme BlueSteel.etheme Ganymede.etheme

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(ethemedir)
	gzip -d -c < $(srcdir)/BrushedMetal-Tigert.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf BrushedMetal-Tigert; mkdir BrushedMetal-Tigert; cd BrushedMetal-Tigert; tar -xmf -)
	gzip -d -c < $(srcdir)/ShinyMetal.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf ShinyMetal; mkdir ShinyMetal; cd ShinyMetal; tar -xmf -)
	gzip -d -c < $(srcdir)/BlueSteel.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf BlueSteel; mkdir BlueSteel; cd BlueSteel; tar -xmf -)
	gzip -d -c < $(srcdir)/Ganymede.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf Ganymede; mkdir Ganymede; cd Ganymede; tar -xmf -)
	if [ -n "$(USER)" ]; then chown -R $(USER) $(DESTDIR)$(ethemedir); else chown -R root $(DESTDIR)$(ethemedir); fi
