if FSSTD
edocdatadir = $(pkgdatadir)/E-docs
else
edocdatadir = $(prefix)/enlightenment/E-docs
endif

edocdata_DATA = @edocdata@
edocdata_SCRIPTS = @edocexec@

EXTRA_DIST = $(edocdata_DATA)
EXTRA_SCRIPTS = 

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(edocdatadir)

# Japanese
	gzip -d -c < $(srcdir)/MAIN-ja | (cd $(DESTDIR)$(edocdatadir); tar -xmf -)
	rm -f $(srcdir)/MAIN-ja
