EXTRA_DIST = \
Makefile \
changelog \
control \
compat \
copyright \
xfonts-nexus.dirs \
xfonts-nexus.files \
xfonts-nexus.postinst \
xfonts-nexus.postrm \
rules

all:

clean:
	rm -f *.substvars *.debhelper files
	rm -r -f xfonts-nexus tmp

dist:
	mkdir "$(DISTDIR)/"
	cp $(EXTRA_DIST) "$(DISTDIR)/"
	for i in $(SUBDIRS); do \
		DISTDIR="../$(DISTDIR)/$i" make -C $$i dist; \
	done
