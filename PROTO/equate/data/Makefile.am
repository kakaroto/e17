
MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = . images

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/images

EDCS = equate.edc text.edc
EDJS = $(EDCS:%.edc=%.edj)
SUFFIXES = .edc .edj

themedir = $(pkgdatadir)/themes
theme_DATA = $(EDJS)

all: $(EDCS)

.edc.edj:
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

EXTRA_DIST = $(EDCS)

clean-local:
	rm -f $(EDJS)
