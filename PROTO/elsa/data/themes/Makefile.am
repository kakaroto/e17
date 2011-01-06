AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/themes -fd $(top_srcdir)/data/themes

filesdir = $(datadir)/elsa/themes
files_DATA = default.edj

EXTRA_DIST = \
default.edc \
elsa_background.jpg \
elsa_foreground.png \
box_login.png \
box_login_shine.png \
check.png \
arrow_left.png \
logo_b.png \
logo_w.png \
star.png \
moon.png


default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj

clean-local:
	rm -f *.edj

