# $Id: Makefile.am 863 2008-05-25 14:38:16Z helly $

noinst_PROGRAMS = re2c
re2c_SOURCES = code.cc dfa.cc main.cc actions.cc substr.cc \
	translate.cc mbo_getopt.cc scanner.cc \
	basics.h dfa.h globals.h ins.h parser.h re_parser.yy re.h scanner.h \
	substr.h token.h mbo_getopt.h code.h stream_lc.h code_names.h $(BUILT_SOURCES)
EXTRA_DIST = \
  CHANGELOG \
  re_parser.yy
BUILT_SOURCES = re_parser.h

AM_YFLAGS = -d

RE2C         = re2c$(EXEEXT)
RE2CFLAGS    = -bi

CLEANFILES   = re_parser.cc re_parser.h 
MAINTAINERCLEANFILES = Makefile.in
