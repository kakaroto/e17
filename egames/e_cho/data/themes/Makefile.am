MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/themes -fd $(top_srcdir)/data/themes -id $(top_srcdir)/data/themes/images

filesdir = $(datadir)/$(PACKAGE)
files_DATA = default.edj efl_devels_theme.edj muted_theme.edj

images =

fonts =

EXTRA_DIST = default.edc efl_devels_theme.edc muted_theme.edc $(images) $(fonts)

default.edj: Makefile $(images) $(fonts) default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj

efl_devels_theme.edj: Makefile $(images) $(fonts) efl_devels_theme.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/efl_devels_theme.edc \
	$(top_builddir)/data/themes/efl_devels_theme.edj

muted_theme.edj: Makefile $(images) $(fonts) muted_theme.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/muted_theme.edc \
	$(top_builddir)/data/themes/muted_theme.edj

clean-local:
	rm -f default.edj efl_devels_theme.edj muted_theme.edj
