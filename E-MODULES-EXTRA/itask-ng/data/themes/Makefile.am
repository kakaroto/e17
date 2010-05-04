MAINTAINERCLEANFILES = Makefile.in

EDJE_FLAGS = -v -id images -fd .

filesdir = $(datadir)
files_DATA = \
itask-ng.edj


EXTRA_DIST = $(files_DATA)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -f *.edj


