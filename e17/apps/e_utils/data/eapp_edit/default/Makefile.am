MAINTAINERCLEANFILES = Makefile.in

NAME = default

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/eapp_edit/$(NAME)/images \
-fd $(top_srcdir)/data/eapp_edit/fonts

filesdir = $(pkgdatadir)/data/e_utils_eapp_edit
files_DATA = $(NAME).edj

$(NAME).edj: Makefile $(NAME).edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/eapp_edit/$(NAME)/$(NAME).edc \
	$(top_builddir)/data/eapp_edit/$(NAME)/$(NAME).edj

clean-local:
	rm -f *.edj

