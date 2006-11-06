MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = icons.edc entrance_edit_gui.desktop.in
SUBDIRS = icons
EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/icons/

filesdir = $(pkgdatadir)/icons
files_DATA = icons.edj

icons.edj: Makefile icons.edc 
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/icons.edc \
	$(top_builddir)/data/icons.edj

dotdesktopdir = $(prefix)/share/applications
dotdesktop_DATA = entrance_edit_gui.desktop

dotdesktopiconsdir = $(prefix)/share/icons
dotdesktopicons_DATA = entrance_edit_gui.png

clean-local:
	rm -f *.edj
