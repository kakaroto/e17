SUBDIRS = configs

if FSSTD
ethemedir = $(prefix)/share/enlightenment/themes
else
ethemedir = $(prefix)/enlightenment/themes
endif

EXTRA_DIST = BrushedMetal-Tigert.etheme ShinyMetal.etheme BlueSteel.etheme Ganymede.etheme  BrushedMetal-Tigert.etheme.ja ShinyMetal.etheme.ja BlueSteel.etheme.ja Ganymede.etheme.ja BrushedMetal-Tigert.etheme.pl ShinyMetal.etheme.pl

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(ethemedir)
	gzip -d -c < $(srcdir)/BrushedMetal-Tigert.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf BrushedMetal-Tigert; mkdir BrushedMetal-Tigert; cd BrushedMetal-Tigert; tar -xmf -)
	gzip -d -c < $(srcdir)/ShinyMetal.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf ShinyMetal; mkdir ShinyMetal; cd ShinyMetal; tar -xmf -)
	gzip -d -c < $(srcdir)/BlueSteel.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf BlueSteel; mkdir BlueSteel; cd BlueSteel; tar -xmf -)
	gzip -d -c < $(srcdir)/Ganymede.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf Ganymede; mkdir Ganymede; cd Ganymede; tar -xmf -)

# Japanese
	gzip -d -c < $(srcdir)/BrushedMetal-Tigert.etheme.ja | (cd $(DESTDIR)$(ethemedir)/BrushedMetal-Tigert; tar -xmf -)
	gzip -d -c < $(srcdir)/ShinyMetal.etheme.ja | (cd $(DESTDIR)$(ethemedir)/ShinyMetal; tar -xmf -)
	gzip -d -c < $(srcdir)/BlueSteel.etheme.ja | (cd $(DESTDIR)$(ethemedir)/BlueSteel; tar -xmf -)
	gzip -d -c < $(srcdir)/Ganymede.etheme.ja | (cd $(DESTDIR)$(ethemedir)/Ganymede; tar -xmf -)

#Polish
	gzip -d -c < $(srcdir)/BrushedMetal-Tigert.etheme.pl | (cd $(DESTDIR)$(ethemedir)/BrushedMetal-Tigert; tar -xmf -)
	gzip -d -c < $(srcdir)/ShinyMetal.etheme.pl | (cd $(DESTDIR)$(ethemedir)/ShinyMetal; tar -xmf -)

	if [ -n "$(USER)" ]; then chown -R $(USER) $(DESTDIR)$(ethemedir); else chown -R root $(DESTDIR)$(ethemedir); fi
