FILES = \
emphasis.glade \
config.glade

filesdir = $(pkgdatadir)
files_DATA = $(FILES)
EXTRA_DIST = $(FILES)

desktopdir   = $(datadir)/applications
desktop_DATA = emphasis.desktop

#http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
icondir = $(datadir)/icons/hicolor/48x48/apps
icon_DATA = emphasis.png

scalableicondir = $(datadir)/icons/hicolor/scalable/apps
scalableicon_DATA = emphasis.svg


SUBDIRS = images 

MAINTAINERCLEANFILES = Makefile.in \
							  emphasis.gladep config.gladep \
							  emphasis.glade.bak config.glade.bak \
							  emphasis.gladep.bak config.gladep.bak
