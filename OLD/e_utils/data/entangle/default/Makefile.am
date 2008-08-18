MAINTAINERCLEANFILES = Makefile.in

NAME = default

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/entangle/$(NAME)/images \
-fd $(top_srcdir)/data/entangle/fonts

filesdir = $(pkgdatadir)/data/entangle
files_DATA = $(NAME).edj

$(NAME).edj: Makefile $(NAME).edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/entangle/$(NAME)/$(NAME).edc \
	$(top_builddir)/data/entangle/$(NAME)/$(NAME).edj

clean-local:
	rm -f *.edj

