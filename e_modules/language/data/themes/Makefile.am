MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id ./images

filesdir = $(datadir)
files_DATA = \
language.edj

EXTRA_DIST = $(files_DATA) $(wildcard images/*.png)

language.edj: Makefile language.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	language.edc \
	language.edj

clean-local:
	rm -f *.edj
