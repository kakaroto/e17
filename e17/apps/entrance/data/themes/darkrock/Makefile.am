MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = img

NAME = darkrock

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/$(NAME)/img \
-fd $(top_srcdir)/data/fonts \
@EDJE_DEF@

filesdir = $(pkgdatadir)/themes
files_DATA = $(NAME).edj

EXTRA_DIST = $(NAME).edc

$(NAME).edj: Makefile $(NAME).edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/$(NAME)/$(NAME).edc \
	$(top_builddir)/data/themes/$(NAME)/$(NAME).edj

clean-local:
	rm -f *.edj
