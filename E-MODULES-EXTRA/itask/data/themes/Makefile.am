MAINTAINERCLEANFILES = Makefile.in

filesdir = $(datadir)
files_DATA = \
itask.edc \
itask.edj


EXTRA_DIST = $(files_DATA) $(wildcard images/*.png)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -f *.edj
