MAINTAINERCLEANFILES = Makefile.in

EDCS =  e17.edc
#EDCS = default.edc skeleton.edc zero.edc e17.edc

EDJS = $(EDCS:%.edc=%.edj)

EXTRA_DIST = $(EDCS) $(basename $(EDCS))
CLEANFILES = $(EDJS)

themedir = $(pkgdatadir)/themes/
theme_DATA = $(EDJS)

all: $(EDJS)

SUFFIXES = .edc .edj
.edc.edj:
	(cd $(top_srcdir)/data/themes ; edje_cc -v -id $(top_srcdir)/data/themes/$(notdir $(<:%.edc=%))/bits/images $(notdir $<) $(top_builddir)/data/themes/$(notdir $@))
