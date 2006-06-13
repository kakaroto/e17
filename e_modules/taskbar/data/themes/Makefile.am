MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id images -fd fonts

filesdir = $(datadir)
files_DATA = \
fonts/VeraBd.ttf \
$(wildcard taskbar.ed?)


EXTRA_DIST = $(files_DATA) $(wildcard images/*.png)
	                   
taskbar.edj: Makefile taskbar.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 taskbar.edc \
	 taskbar.edj

clean-local:
	rm -f *.edj


