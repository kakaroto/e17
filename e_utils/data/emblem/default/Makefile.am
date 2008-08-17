MAINTAINERCLEANFILES = Makefile.in

NAME = default

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/emblem/$(NAME)/images \
-fd $(top_srcdir)/data/emblem/fonts

filesdir = $(pkgdatadir)/data/emblem
files_DATA = $(NAME).edj

$(NAME).edj: Makefile $(NAME).edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/emblem/$(NAME)/$(NAME).edc \
	$(top_builddir)/data/emblem/$(NAME)/$(NAME).edj

clean-local:
	rm -f *.edj

