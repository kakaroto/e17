
ethemedir = $(ENLIGHTENMENT_ROOT)/themes

EXTRA_DIST = BrushedMetal-Tigert.etheme ShinyMetal.etheme BlueSteel.etheme Ganymede.etheme  BrushedMetal-Tigert.etheme.ja ShinyMetal.etheme.ja BlueSteel.etheme.ja Ganymede.etheme.ja BrushedMetal-Tigert.etheme.pl ShinyMetal.etheme.pl

THEMES = BrushedMetal-Tigert ShinyMetal BlueSteel Ganymede
THEMES_JA = BrushedMetal-Tigert ShinyMetal BlueSteel Ganymede 
THEMES_PL = BrushedMetal-Tigert ShinyMetal

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(ethemedir)
	for t in $(THEMES); do \
		gzip -d -c < $(srcdir)/$$t.etheme | (cd $(DESTDIR)$(ethemedir); mkdir -p $$t; cd $$t; tar -xmf -); \
	done
# Japanese
	for t in $(THEMES_JA); do \
		gzip -d -c < $(srcdir)/$$t.etheme.ja | (cd $(DESTDIR)$(ethemedir); mkdir -p $$t; cd $$t; tar -xmf -); \
	done
#Polish
	for t in $(THEMES_PL); do \
		gzip -d -c < $(srcdir)/$$t.etheme.pl | (cd $(DESTDIR)$(ethemedir); mkdir -p $$t; cd $$t; tar -xmf -); \
	done
	if [ -n "$(USER)" ]; then chown -R $(USER) $(DESTDIR)$(ethemedir); else chown -R root $(DESTDIR)$(ethemedir); fi

uninstall-local:
	-for t in $(THEMES); do \
		gzip -d -c < $(srcdir)/$$t.etheme | (cd $(DESTDIR)$(ethemedir)/$$t; rm -f `tar -tf -` 2>/dev/null); \
	done
# Japanese
	-for t in $(THEMES_JA); do \
		gzip -d -c < $(srcdir)/$$t.etheme.ja | (cd $(DESTDIR)$(ethemedir)/$$t; rm -f `tar -tf -` 2>/dev/null); \
	done
#Polish
	-for t in $(THEMES_PL); do \
		gzip -d -c < $(srcdir)/$$t.etheme.pl | (cd $(DESTDIR)$(ethemedir)/$$t; rm -f `tar -tf -` 2>/dev/null); \
	done
