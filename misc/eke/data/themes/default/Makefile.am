MAINTAINERCLEANFILES = Makefile.in

NAME = default

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/$(NAME)/images \
-fd $(top_srcdir)/data/fonts

filesdir = $(pkgdatadir)/themes
files_DATA = $(NAME).eet

$(NAME).eet: Makefile $(NAME).edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/$(NAME)/$(NAME).edc \
	$(top_builddir)/data/themes/$(NAME)/$(NAME).eet

clean-local:
	rm -f *.eet

