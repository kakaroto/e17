PACKAGE=xfonts-nexus
VERSION=0.0.2

SUBDIRS = debian

ALIAS = nex6x10.alias
BDF = nex6x10.bdf
PCF = nex6x10.pcf

FONTDIR  ?= /usr/X11R6/lib/X11/fonts/misc
ALIASDIR ?= /etc/X11/fonts/misc

EXTRA_DIST = \
AUTHORS \
COPYING \
Makefile \
nexus.spec \
nex6x10.alias $(BDF) $(ALIAS)

all: font-clean
	bdftopcf $(BDF) > $(PCF)
	gzip --best $(PCF)
	
font-clean:
	rm -f $(PCF) $(PCF).gz
	
clean: font-clean
	rm -f build-stamp
	for i in $(SUBDIRS); do \
		make -C $$i clean; \
	done

install: all
	test -d "$(DESTDIR)/$(FONTDIR)/" || mkdir -p "$(DESTDIR)/$(FONTDIR)/"
	install -c -m 644 $(PCF).gz "$(DESTDIR)/$(FONTDIR)/"
	test -d "$(DESTDIR)/$(ALIASDIR)/" || mkdir -p "$(DESTDIR)/$(ALIASDIR)/"
	install -c -m 644 $(ALIAS) "$(DESTDIR)/$(ALIASDIR)/"
	(cd "$(DESTDIR)/$(FONTDIR)" && mkfontdir )
	@echo ""
	@echo "If you are running the xfont server you should restart it"
	@echo "Regardless, you should run \"xset fp rehash\" as the user"
	@echo "who is running the session."
	@echo "You can verify that this all worked by running:"
	@echo "xlsfonts | fgrep nexus"

uninstall:
	rm -f "$(DESTDIR)/$(FONTDIR)/$(PCF).gz"
	rm -f "$(DESTDIR)/$(ALIASDIR)/$(ALIAS) "
	(cd "$(DESTDIR)/$(FONTDIR)" && mkfontdir )
	@echo ""
	@echo "If you are running the xfont server you should restart it"
	@echo "Regardless, you should run \"xset fp rehash\" as the user"
	@echo "who is running the session."
	@echo "You can verify that this all worked by running:"
	@echo "xlsfonts | fgrep nexus"

distclean-1:
	rm -rf "$(PACKAGE)-$(VERSION)/" "$(PACKAGE)-$(VERSION).tar" \
		"$(PACKAGE)-$(VERSION).tar.gz"

distclean: distclean-1 clean

dist: distclean-1
	mkdir "$(PACKAGE)-$(VERSION)/"
	cp $(EXTRA_DIST) "$(PACKAGE)-$(VERSION)/"
	for i in $(SUBDIRS); do \
		DISTDIR="../$(PACKAGE)-$(VERSION)/$$i" make -C $$i dist; \
	done
	tar -cf "$(PACKAGE)-$(VERSION).tar" "$(PACKAGE)-$(VERSION)/"
	gzip --best "$(PACKAGE)-$(VERSION).tar"
	rm -rf "$(PACKAGE)-$(VERSION)/"

distcheck: dist
	tar -zxf "$(PACKAGE)-$(VERSION).tar.gz"
	mkdir "$(PACKAGE)-$(VERSION)/.build/"
	(cd "$(PACKAGE)-$(VERSION)/.build/" ;\
		DESTDIR="$(PWD)/../.done/" make -C ../ distclean all install)
	rm -r "$(PACKAGE)-$(VERSION)/"
	@echo "############################################################"
	@echo "# $(PACKAGE)-$(VERSION).tar.gz is ready for distribution"
	@echo "############################################################"

	 
