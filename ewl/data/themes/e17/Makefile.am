MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = groups \
	  fonts \
	  images

EDCS =  e17.edc
EDJS = $(EDCS:%.edc=%.edj)

e17_DEPS = \
        groups/inc/button-images-helper.edc \
        groups/inc/focus-images.edc \
        groups/inc/glint-images.edc \
        groups/inc/menu-images.edc \
        groups/inc/button-parts-helper.edc \
        groups/inc/focus-parts.edc \
        groups/inc/glint-parts.edc \
        groups/inc/menu-parts.edc \
        groups/inc/button-programs-helper.edc \
        groups/inc/focus-programs.edc \
        groups/inc/glint-programs.edc \
        groups/inc/menu-programs.edc \
        groups/border.edc \
        groups/button.edc \
        groups/combo.edc \
        groups/label.edc \
        groups/menu.edc \
        groups/notebook.edc \
        groups/progress.edc \
        groups/scroll.edc \
        groups/separator.edc \
        groups/shadow.edc \
        groups/spacer.edc \
        groups/spinner.edc \
        groups/text.edc \
        groups/toolbar.edc \
        groups/tree.edc \
        groups/window.edc

EXTRA_DIST = $(EDCS) 

CLEANFILES = $(EDJS)

themedir = $(pkgdatadir)/themes/
theme_DATA = $(EDJS)

all: $(EDJS)

e17.edj: ${e17_DEPS}

SUFFIXES = .edc .edj
.edc.edj:
	edje_cc -v -id $(top_srcdir)/data/themes/$(*F)/images -fd $(top_srcdir)/data/themes/$(*F) $(top_srcdir)/data/themes/$(*F)/$< $(top_builddir)/data/themes/$(*F)/$(@F)

