MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/themes -fd $(top_srcdir)/data/themes

filesdir = $(datadir)/$(PACKAGE)
files_DATA = default.edj

images =
fonts =

EXTRA_DIST = default.edc $(images) $(fonts)

default.edj: Makefile $(images) $(fonts) default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj

clean-local:
	rm -f default.edj
