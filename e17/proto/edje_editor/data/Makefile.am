MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = edje_editor.edc
SUBDIRS = fonts
	  
EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/images \
-fd $(top_srcdir)/data/fonts

filesdir = $(pkgdatadir)
files_DATA = edje_editor.edj

desktopdir   = $(datadir)/applications
desktop_DATA = edje_editor.desktop

#http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
icondir = $(datadir)/icons/hicolor/48x48/apps
icon_DATA = edje_editor.png


edje_editor.edj: Makefile
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/edje_editor.edc \
	$(top_builddir)/data/edje_editor.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache
