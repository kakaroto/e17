man_MANS = enlightenment.1

EXTRA_DIST = enlightenment.1

enlightenment.man: enlightenment.pod
	pod2man --center="Enlightenment Man Pages" \
		--release="Enlightenment @ENLIGHTENMENT_VERSION@" $< > $@

enlightenment.pod: $(top_srcdir)/man/MAIN
	$(top_srcdir)/man/e2mp < $< > $@
