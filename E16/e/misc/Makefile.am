DESKTOPS_APP = e16.desktop
DESKTOPS_SES = e16.desktop e16-gnome.desktop e16-kde.desktop
DESKTOPS_IN = e16.desktop-app.in e16.desktop.in e16-gnome.desktop.in e16-kde.desktop.in
XCLIENTS = Xclients.e16.sh Xclients.e16-gnome.sh Xclients.e16-kde.sh
STARTUPS = starte16-gnome starte16-kde
ICONS = e16.png

DESKTOPS = $(DESKTOPS_APP:.desktop=.desktop-app) $(DESKTOPS_SES)

miscdir = $(pkgdatadir)/misc
misc_DATA = $(ICONS) $(DESKTOPS)
misc_SCRIPTS = $(XCLIENTS) starte16 e16-dbus-cmd

applicationdir = $(datadir)/applications
xsessiondir = $(datadir)/xsessions

# Is this Fedora only stuff?
#xclientsdir = $(sysconfdir)/X11/xinit/Xclients.d
#xclients_SCRIPTS = $(XCLIENTS)

EXTRA_DIST = $(DESKTOPS_IN) $(XCLIENTS) $(SCRIPTS) $(ICONS)

CLEANFILES = $(DESKTOPS)

e16.desktop-app: $(srcdir)/e16.desktop-app.in Makefile
e16.desktop: $(srcdir)/e16.desktop.in Makefile
e16-gnome.desktop: $(srcdir)/e16-gnome.desktop.in Makefile
e16-kde.desktop: $(srcdir)/e16-kde.desktop.in Makefile

$(DESKTOPS):
	sed -e "s=@pkgdatadir@=${pkgdatadir}=" $(srcdir)/$@.in > $@

install-data-hook:
	cd $(DESTDIR)$(bindir); rm -f starte16; $(LN_S) $(miscdir)/starte16 .
	$(mkinstalldirs) $(DESTDIR)$(xsessiondir)
	cd $(DESTDIR)$(xsessiondir); rm -f $(DESKTOPS_SES); \
	  for f in $(DESKTOPS_SES); do $(LN_S) $(miscdir)/$$f .; done
	cd $(DESTDIR)$(miscdir); rm -f $(STARTUPS); \
	  for f in $(STARTUPS); do $(LN_S) starte16 $$f; done
	$(mkinstalldirs) $(DESTDIR)$(applicationdir)
	cd $(DESTDIR)$(applicationdir); rm -f $(DESKTOPS_APP); \
	  for f in $(DESKTOPS_APP); do $(LN_S) $(miscdir)/$$f-app $$f; done
	cd $(DESTDIR)$(miscdir); rm -f $(STARTUPS); \
	  for f in $(STARTUPS); do $(LN_S) starte16 $$f; done

uninstall-local:
	rm -f $(DESKTOPS)
	cd $(DESTDIR)$(bindir); rm -f starte16
	cd $(DESTDIR)$(applicationdir); rm -f $(DESKTOPS_APP)
	cd $(DESTDIR)$(xsessiondir); rm -f $(DESKTOPS_SES)
	cd $(DESTDIR)$(miscdir); rm -f $(STARTUPS)
