DESKTOPS = e16.desktop e16-gnome.desktop e16-kde.desktop
DESKTOPS_IN = e16.desktop.in e16-gnome.desktop.in e16-kde.desktop.in
XCLIENTS = Xclients.e16.sh Xclients.e16-gnome.sh Xclients.e16-kde.sh
STARTUP = starte16
ICONS = e16.png

bin_SCRIPTS = starte16

miscdir = $(ENLIGHTENMENT_ROOT)/misc
misc_DATA = $(ICONS) $(DESKTOPS)
misc_SCRIPTS = $(XCLIENTS)

xsessiondir = $(datadir)/xsessions
xsession_DATA = $(DESKTOPS)

# Is this Fedora only stuff?
#xclientsdir = $(sysconfdir)/X11/xinit/Xclients.d
#xclients_SCRIPTS = $(XCLIENTS)

EXTRA_DIST = $(DESKTOPS_IN) $(SCRIPTS) $(ICONS)
