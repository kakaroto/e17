# $Id$

AUTOMAKE_OPTIONS = foreign
SUBDIRS = include src test

bin_SCRIPTS = libast-config

EXTRA_DIST = README

test: all
	cd test && $(MAKE) test

perf: all
	cd test && $(MAKE) perf

.PHONY: test
