SUBDIRS = configs

if FSSTD
ethemedir = $(prefix)/share/enlightenment/themes
else
ethemedir = $(prefix)/enlightenment/themes
endif

#etheme_DATA = icE.etheme Clean.etheme BrushedMetal-Tigert.etheme

EXTRA_DIST = icE.etheme Clean.etheme BrushedMetal-Tigert.etheme \
             eStep_Classic.etheme eStep_New.etheme CleanBig.etheme \
	     ShinyMetal.etheme

install-data-local:
	test -d $(DESTDIR)$(ethemedir) || mkdir $(DESTDIR)$(ethemedir)
	gzip -d -c < $(srcdir)/icE.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf icE; mkdir icE; cd icE; tar -xmf -)
	gzip -d -c < $(srcdir)/Clean.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf Clean; mkdir Clean; cd Clean; tar -xmf -)
	gzip -d -c < $(srcdir)/BrushedMetal-Tigert.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf BrushedMetal-Tigert; mkdir BrushedMetal-Tigert; cd BrushedMetal-Tigert; tar -xmf -)
	gzip -d -c < $(srcdir)/eStep_Classic.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf eStep_Classic; mkdir eStep_Classic; cd eStep_Classic; tar -xmf -)
	gzip -d -c < $(srcdir)/eStep_New.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf eStep_New; mkdir eStep_New; cd eStep_New; tar -xmf -)
	gzip -d -c < $(srcdir)/CleanBig.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf CleanBig; mkdir CleanBig; cd CleanBig; tar -xmf -)
	gzip -d -c < $(srcdir)/ShinyMetal.etheme | (cd $(DESTDIR)$(ethemedir); rm -rf ShinyMetal; mkdir ShinyMetal; cd ShinyMetal; tar -xmf -)
	-chown -R $(USER) $(DESTDIR)$(ethemedir)
