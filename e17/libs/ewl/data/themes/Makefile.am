SUBDIRS = default skeleton
MAINTAINERCLEANFILES = Makefile.in

EDCS =  default.edc skeleton.edc

EETS = $(EDCS:%.edc=%.eet)

EXTRA_DIST = $(EDCS)
CLEANFILES = $(EETS)

themedir = $(pkgdatadir)/themes/
theme_DATA = $(EETS)

all: $(EETS)

SUFFIXES = .edc .eet
.edc.eet:
	(cd $(top_srcdir)/data/themes ; edje_cc -v -id $(@:%.eet=%)/bits/images $< $(top_builddir)/data/themes/$@)

