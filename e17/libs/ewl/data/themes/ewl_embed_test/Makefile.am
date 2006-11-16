MAINTAINERCLEANFILES = Makefile.in

EDCS = ewl_embed_test.edc
EDJS = $(EDCS:%.edc=%.edj)

EXTRA_DIST = $(EDCS) $(basename $(EDCS))
CLEANFILES = $(EDJS)

themedir = $(pkgdatadir)/themes/
theme_DATA = $(EDJS)

all: $(EDJS)

SUFFIXES = .edc .edj
.edc.edj:
	edje_cc -v -id $(top_srcdir)/data/themes/$(notdir $(<:%.edc=%))/images $(top_srcdir)/data/themes/$(notdir $(<:%.edc=%))/$< $(top_builddir)/data/themes/$(notdir $(<:%.edc=%))/$(notdir $@)

