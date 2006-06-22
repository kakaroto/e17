MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id images -fd fonts

filesdir = $(datadir)
files_DATA = \
fonts/VeraBd.ttf \
$(wildcard taskbar.ed?) \
$(wildcard taskbar2.ed?)


EXTRA_DIST = $(files_DATA) $(wildcard images/*.png)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -f *.edj


