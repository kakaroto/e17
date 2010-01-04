MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = images

NAME = default

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/users/$(NAME)/images \
-fd $(top_srcdir)/data/fonts \
@EDJE_DEF@

filesdir = $(pkgdatadir)/users
files_DATA = $(NAME).edj

EXTRA_DIST = $(NAME).edc

$(NAME).edj: Makefile $(NAME).edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/users/$(NAME)/$(NAME).edc \
	$(top_builddir)/data/users/$(NAME)/$(NAME).edj

clean-local:
	rm -f *.edj
