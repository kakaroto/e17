MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = images

EDCS = ewl_embed_test.edc
EDJS = $(EDCS:%.edc=%.edj)

EXTRA_DIST = $(EDCS)

CLEANFILES = $(EDJS)

if EWL_TESTS_BUILD

themedir = $(pkgdatadir)/themes/
theme_DATA = $(EDJS)

all: $(EDJS)

SUFFIXES = .edc .edj
.edc.edj:
	edje_cc -v -id $(top_srcdir)/data/themes/$(*F)/images $(top_srcdir)/data/themes/$(*F)/$< $(top_builddir)/data/themes/$(*F)/$(@F)

endif
