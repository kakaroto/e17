MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = img fnt

NAME = default

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/$(NAME)/img \
-fd $(top_srcdir)/data/themes/$(NAME)/fnt \
@EDJE_DEF@

filesdir = $(pkgdatadir)/themes
files_DATA = $(NAME).eet

EXTRA_DIST = $(NAME).edc

$(NAME).eet: Makefile $(NAME).edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/$(NAME)/$(NAME).edc \
	$(top_builddir)/data/themes/$(NAME)/$(NAME).eet

clean-local:
	rm -f *.eet
