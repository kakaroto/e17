MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = edje_cc
EDJE_FLAGS = -v -id $(top_srcdir)/data/ -fd $(top_srcdir)/data/

filesdir = $(pkgdatadir)
files_DATA = exchange_smart.edj

EXTRA_DIST = exchange_smart.edc \
             shadow.png \
             star.png \
             button.png \
             update.png \
             new.png \
             busy-1.png \
             busy-2.png \
             busy-3.png \
             busy-4.png \
             busy-5.png \
             busy-6.png \
             busy-7.png \
             busy-8.png \
             busy-9.png

exchange_smart.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
		$(top_srcdir)/data/exchange_smart.edc \
		$(top_builddir)/data/exchange_smart.edj

clean-local:
	rm -f exchange_smart.edj
