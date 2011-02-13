EDJE_CC := edje_cc
INSTALL := install
PKG_CONFIG := pkg-config

SRCDIR := .
BUILDDIR := .
E_DESTDIR := $(shell $(PKG_CONFIG) --variable=prefix enlightenment)/share/enlightenment/data/themes
E_HOMEDIR := ~/.e/e/themes
ELM_DESTDIR := $(shell $(PKG_CONFIG) --variable=prefix elementary)/share/elementary/data/themes
ELM_HOMEDIR := ~/.elementary/themes

COMMON_EDCS := $(wildcard $(SRCDIR)/*.edc)
E_EDCS      := $(wildcard $(SRCDIR)/e/*.edc)
ELM_EDCS    := $(wildcard $(SRCDIR)/elm/*.edc)

IMAGES := $(wildcard $(SRCDIR)/images/*.png $(SRCDIR)/images/*.jpg)

.PHONY: all clean install install-system install-home install-e-system install-elm-home install-elm-system install-elm-home

all: efenniht.edj elm-efenniht.edj

clean:
	rm -f efenniht.edj elm-efenniht.edj


efenniht.edj: $(COMMON_EDCS) $(E_EDCS) $(IMAGES) Makefile
	$(EDJE_CC) -v -id images/ \
		$(SRCDIR)/efenniht.edc \
		$(BUILDDIR)/efenniht.edj

elm-efenniht.edj: $(COMMON_EDCS) $(ELM_EDCS) $(IMAGES) Makefile
	$(EDJE_CC) -v -id images/ -id ../../elementary/data/themes/ \
		$(SRCDIR)/elm-efenniht.edc \
		$(BUILDDIR)/elm-efenniht.edj

install:
	@echo "Choose install-system or install-home"

install-system: install-e-system install-elm-system

install-home: install-e-home install-elm-home


install-e-system: $(BUILDDIR)/efenniht.edj
	$(INSTALL) -D  -C -m 0644 -o root -g root \
		$(BUILDDIR)/efenniht.edj \
		$(E_DESTDIR)/efenniht.edj

install-e-home: $(BUILDDIR)/efenniht.edj
	$(INSTALL) -D  -C -m 0644 \
		$(BUILDDIR)/efenniht.edj \
		$(E_HOMEDIR)/efenniht.edj


install-elm-system: $(BUILDDIR)/elm-efenniht.edj
	$(INSTALL) -D  -C -m 0644 -o root -g root \
		$(BUILDDIR)/elm-efenniht.edj \
		$(ELM_DESTDIR)/elm-efenniht.edj
	@echo "you need export ELM_THEME='$(ELM_DESTDIR)/elm-efenniht.edj'"

install-elm-home: $(BUILDDIR)/elm-efenniht.edj
	$(INSTALL) -D  -C -m 0644 \
		$(BUILDDIR)/elm-efenniht.edj \
		$(ELM_HOMEDIR)/elm-efenniht.edj
	@echo "you need export ELM_THEME='$(ELM_HOMEDIR)/elm-efenniht.edj'"
