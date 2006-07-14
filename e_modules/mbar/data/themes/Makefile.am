MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id images -fd fonts

filesdir = $(datadir)
files_DATA = \
mbar.edj

EXTRA_DIST = $(files_DATA) \
				$(wildcard images/*.png) \
				fonts/VeraBd.ttf \
				mbar.edc

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -f *.edj
