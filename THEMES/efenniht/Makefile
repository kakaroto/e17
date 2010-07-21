EDJE_CC := edje_cc
INSTALL := install
PKG_CONFIG := pkg-config

SRCDIR := .
BUILDDIR := .
DESTDIR := $(shell $(PKG_CONFIG) --variable=prefix enlightenment)/share/enlightenment/data/themes
HOMEDIR := ~/.e/e/themes

COMMON_EDCS := $(wildcard $(SRCDIR)/*.edc)
E_EDCS := $(wildcard $(SRCDIR)/e/*.edc)
ELM_EDCS := $(wildcard $(SRCDIR)/*.edc)
IMAGES := $(wildcard $(SRCDIR)/images/*.png $(SRCDIR)/images/*.jpg)

efenniht.edj: $(COMMON_EDCS) $(E_EDCS) $(IMAGES) Makefile
	$(EDJE_CC) -v -id images/ \
		$(SRCDIR)/efenniht.edc \
		$(BUILDDIR)/efenniht.edj

.PHONY: install install-system install-home
install:
	@echo "Choose install-system or install-home"

install-system: $(BUILDDIR)/efenniht.edj
	$(INSTALL) -D  -C -m 0644 -o root -g root \
		$(BUILDDIR)/efenniht.edj \
		$(DESTDIR)/efenniht.edj

install-home: $(BUILDDIR)/efenniht.edj
	$(INSTALL) -D  -C -m 0644 \
		$(BUILDDIR)/efenniht.edj \
		$(HOMEDIR)/efenniht.edj
