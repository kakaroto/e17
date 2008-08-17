MAINTAINERCLEANFILES = Makefile.in

NAME = default

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/exige/$(NAME)/images \
-fd $(top_srcdir)/data/exige/$(NAME)/fonts

filesdir = $(pkgdatadir)/data/exige
files_DATA = $(NAME).edj

$(NAME).edj: Makefile $(NAME).edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/exige/$(NAME)/$(NAME).edc \
	$(top_builddir)/data/exige/$(NAME)/$(NAME).edj

clean-local:
	rm -f *.edj

