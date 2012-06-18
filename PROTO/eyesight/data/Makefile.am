
MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS_VERBOSE_ =
EDJE_FLAGS_VERBOSE_0 =
EDJE_FLAGS_VERBOSE_1 = -v
EDJE_FLAGS = $(EDJE_FLAGS_VERBOSE_$(V)) -id $(top_srcdir)/data/images -fd $(top_srcdir)/data

filesdir = $(pkgdatadir)/themes
files_DATA = default.edj

icondir = $(pkgdatadir)/images
icon_DATA = images/document.png

EXTRA_DIST = \
default.edc \
images/exq-bglight.png \
images/exq-dot-glow.png \
images/exq-dot.png \
images/exq-logo.png \
images/document.png

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/default.edc \
	$(top_builddir)/data/default.edj

clean-local:
	rm -f *.edj
