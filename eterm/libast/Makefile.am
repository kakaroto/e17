# $Id$

AUTOMAKE_OPTIONS = foreign
SUBDIRS = include src test

bin_SCRIPTS = libast-config

EXTRA_DIST = README

test: all
	cd test && $(MAKE) test

.PHONY: test
