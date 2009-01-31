DESKTOPS = e16.desktop e16-gnome.desktop e16-kde.desktop
DESKTOPS_IN = e16.desktop.in e16-gnome.desktop.in e16-kde.desktop.in
XCLIENTS = Xclients.e16.sh Xclients.e16-gnome.sh Xclients.e16-kde.sh
STARTUPS = starte16-gnome starte16-kde
ICONS = e16.png

miscdir = $(pkgdatadir)/misc
misc_DATA = $(ICONS) $(DESKTOPS)
misc_SCRIPTS = $(XCLIENTS) starte16 e16-dbus-cmd

xsessiondir = $(datadir)/xsessions
#xsession_DATA = $(DESKTOPS)

# Is this Fedora only stuff?
#xclientsdir = $(sysconfdir)/X11/xinit/Xclients.d
#xclients_SCRIPTS = $(XCLIENTS)

EXTRA_DIST = $(DESKTOPS_IN) $(XCLIENTS) $(SCRIPTS) $(ICONS)

CLEANFILES = $(DESKTOPS)

e16.desktop: $(top_srcdir)/misc/e16.desktop.in Makefile
e16-gnome.desktop: $(top_srcdir)/misc/e16-gnome.desktop.in Makefile
e16-kde.desktop: $(top_srcdir)/misc/e16-kde.desktop.in Makefile

$(DESKTOPS):
	sed -e "s=@pkgdatadir@=${pkgdatadir}=" $(top_srcdir)/misc/$@.in > $@

install-data-hook:
	cd $(DESTDIR)$(bindir); rm -f starte16; $(LN_S) $(miscdir)/starte16 .
	$(mkinstalldirs) $(DESTDIR)$(xsessiondir)
	cd $(DESTDIR)$(xsessiondir); rm -f $(DESKTOPS); \
	  for f in $(DESKTOPS); do $(LN_S) $(miscdir)/$$f .; done
	cd $(DESTDIR)$(miscdir); rm -f $(STARTUPS); \
	  for f in $(STARTUPS); do $(LN_S) starte16 $$f; done

uninstall-local:
	rm -f $(DESKTOPS)
	cd $(DESTDIR)$(bindir); rm -f starte16
	cd $(DESTDIR)$(xsessiondir); rm -f $(DESKTOPS)
	cd $(DESTDIR)$(miscdir); rm -f $(STARTUPS)
