SUBDIRS = configs

if FSSTD
ethemedir = $(prefix)/share/enlightenment/themes
else
ethemedir = $(prefix)/enlightenment/themes
endif

EXTRA_DIST = BrushedMetal-Tigert.etheme ShinyMetal.etheme

install-data-local:
	test -d $(DESTDIR)$(ethemedir) || mkdir $(DESTDIR)$(ethemedir)
	gzip -d -c < $(srcdir)/BrushedMetal-Tigert.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf BrushedMetal-Tigert; mkdir BrushedMetal-Tigert; cd BrushedMetal-Tigert; tar -xmf -)
	gzip -d -c < $(srcdir)/ShinyMetal.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf ShinyMetal; mkdir ShinyMetal; cd ShinyMetal; tar -xmf -)
	-chown -R $(USER) $(DESTDIR)$(ethemedir)
