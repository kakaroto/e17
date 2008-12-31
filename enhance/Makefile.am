## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS       = 1.7 foreign

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                config.h.in config.sub configure install-sh \
		depcomp libtool missing
		
EXTRA_DIST = README AUTHORS
		
SUBDIRS = src

bin_SCRIPTS = enhance-config

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = enhance.pc
