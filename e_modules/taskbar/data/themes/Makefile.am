MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id images -fd fonts

EXTRA_DIST = \
taskbar.edc 

filesdir = $(datadir)
files_DATA = \
fonts/VeraBd.ttf \
taskbar.edj \
taskbar.edc

taskbar.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/taskbar.edc \
	$(top_builddir)/data/themes/taskbar.edj

clean:
	rm -f taskbar.edj


