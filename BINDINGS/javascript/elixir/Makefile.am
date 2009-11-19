SUBDIRS = src build-aux doc

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in build-stamp configure-stamp depcomp \
		       elixir_docs.tar.gz \
		       README \
		       elixir.c \
		       elixir.pc \
		       elixir.spec

bin_SCRIPTS = elixir-config

EXTRA_DIST = AUTHORS COPYING COPYING-PLAIN elixir.c.in \
	README.in README \
	elixir.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = elixir.pc

ACLOCAL_AMFLAGS = -I m4
