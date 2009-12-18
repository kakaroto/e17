MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/test \
-fd $(top_srcdir)/test

filesdir = $(datadir)/editje/data/sample
files_DATA = \
sample.edj \
sample2.edj \
sample-anim.edj

EXTRA_DIST = \
DejaVuSans-Bold.ttf \
DejaVuSansMono.ttf \
DejaVuSans.ttf \
image.png \
sample2.edc \
sample-anim.edc \
sample.edc \
shadow_under.png

sample.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/test/sample.edc \
	$(top_builddir)/test/sample.edj

sample2.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/test/sample2.edc \
	$(top_builddir)/test/sample2.edj

sample-anim.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/test/sample-anim.edc \
	$(top_builddir)/test/sample-anim.edj

clean-local:
	rm -f *.edj
