MAINTAINERCLEANFILES = Makefile.in

EDJE_FLAGS = -v \
             -id $(top_srcdir)/data/themes/images \
             -fd $(top_srcdir)/data/themes/fonts

EXTRA_DIST = \
taskbar.edc \
$(wildcard fonts/*.ttf) \
$(wildcard images/*.png) 

filesdir = $(datadir)
files_DATA = \
taskbar.edj

taskbar.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/taskbar.edc \
	$(top_builddir)/data/themes/taskbar.edj

clean-local:
	rm -f taskbar.edj


