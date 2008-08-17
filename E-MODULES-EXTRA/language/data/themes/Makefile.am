MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = images

EDJE_FLAGS = -v -id ./images

filesdir = $(datadir)
files_DATA = \
language.edj

EXTRA_DIST = $(files_DATA) \
			language.edc \
			$(wildcard images/*.png)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -f *.edj
