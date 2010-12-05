# $Id: Makefile.am 863 2008-05-25 14:38:16Z helly $

noinst_PROGRAMS = re2c
re2c_SOURCES = code.cc dfa.cc main.cc actions.cc scanner.re substr.cc\
	translate.cc scanner.cc mbo_getopt.cc \
	basics.h dfa.h globals.h ins.h parser.h re_parser.yy re.h scanner.h \
	substr.h token.h mbo_getopt.h code.h stream_lc.h code_names.h
EXTRA_DIST = \
  CHANGELOG \
  re_parser.yy \
  bootstrap/scanner.cc
BUILT_SOURCES = scanner.cc re_parser.h

AM_YFLAGS = -d

RE2C         = re2c$(EXEEXT)
RE2CFLAGS    = -bi

CLEANFILES   = re_parser.c re_parser.h scanner.cc re2c.1 .version htdocs/manual.html

DISTCLEANFILES = makerpm re2c.spec README scanner.cc re2c$(EXEEXT)
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.h.in config.h.in~ config_w32.h.in configure depcomp install-sh missing

scanner.cc: re_parser.h

scanner.cc: scanner.re
	@if test -x ./re2c$(EXEEXT); then \
		echo "re2c $(RE2CFLAGS) -o $@ scanner.re"; \
		./re2c $(RE2CFLAGS) -o $@ scanner.re && cp $@ bootstrap/; \
	else \
		echo "cp -f bootstrap/$@ $@"; \
		cp -f bootstrap/$@ $@; \
	fi
