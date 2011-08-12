EDJE_CC := edje_cc
INSTALL := install
PKG_CONFIG := pkg-config

SRCDIR := .
BUILDDIR := .
E_DESTDIR := $(shell $(PKG_CONFIG) --variable=prefix enlightenment)/share/enlightenment/data/backgrounds
E_HOMEDIR := ~/.e/e/backgrounds

COMMON_EDCS := $(wildcard $(SRCDIR)/*.edc)
E_EDCS      := $(wildcard $(SRCDIR)/e/*.edc)

IMAGES := $(wildcard $(SRCDIR)/images/*.png $(SRCDIR)/images/*.jpg)

.PHONY: all clean install install-system install-home

all: klok.edj

clean:
	rm -f klok.edj


klok.edj: $(COMMON_EDCS)
	$(EDJE_CC) -v -id . -fd . \
		$(SRCDIR)/klok.edc \
		-o $(BUILDDIR)/klok.edj

install:
	@echo "Choose install-system or install-home"

install-system: $(BUILDDIR)/klok.edj
	$(INSTALL) -D  -C -m 0644 -o root -g root \
		$(BUILDDIR)/klok.edj \
		$(E_DESTDIR)/klok.edj

install-home: $(BUILDDIR)/klok.edj
	$(INSTALL) -D  -C -m 0644 \
		$(BUILDDIR)/klok.edj \
		$(E_HOMEDIR)/klok.edj
