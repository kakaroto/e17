MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = images

EDCS = ephoto.edc
EDJS = $(EDCS:%.edc=%.edj)

ephoto_DEPS = ephoto.edc

EXTRA_DIST = $(EDCS)

CLEANFILES = $(EDJS)

themedir = $(pkgdatadir)/themes/default
theme_DATA = $(EDJS)

all: $(EDJS)

ephoto.edj: ${ephoto_DEPS}

SUFFIXES = .edc .edj
.edc.edj:
	$(edje_cc) -v -id $(top_srcdir)/data/themes/default/images -fd $(top_srcdir)/data/themes/default/fonts $< $(top_builddir)/data/themes/default/$(@F)
