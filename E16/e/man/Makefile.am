man_MANS = # e16.1

EXTRA_DIST = e16.1

e16.man: e16.pod
	pod2man --center="Enlightenment Man Pages" \
		--release="Enlightenment @E_PKG_VERSION@" $< > $@

e16.pod: $(top_srcdir)/man/MAIN
	$(top_srcdir)/man/e2mp < $< > $@
