FILES = \
emphasis.glade \
config.glade

filesdir = $(pkgdatadir)
files_DATA = $(FILES)
EXTRA_DIST = $(FILES)

desktopdir   = $(datadir)/applications
desktop_DATA = emphasis.desktop

icondir = $(datadir)/pixmaps/
icon_DATA = emphasis.png emphasis.svg

SUBDIRS = images 

MAINTAINERCLEANFILES = Makefile.in \
							  emphasis.gladep config.gladep \
							  emphasis.glade.bak config.glade.bak \
							  emphasis.gladep.bak config.gladep.bak
