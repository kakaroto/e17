MAINTAINERCLEANFILES = Makefile.in

#EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id images -fd .

filesdir = $(datadir)
files_DATA = \
itask.edc \
itask.edj


EXTRA_DIST = $(files_DATA) $(wildcard images/*.png)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -f *.edj


