# $Id: Makefile.am 863 2008-05-25 14:38:16Z helly $

noinst_PROGRAMS = re2c
re2c_SOURCES = code.cc dfa.cc main.cc parser.cc actions.cc scanner.re substr.cc\
	translate.cc scanner.cc mbo_getopt.cc \
	basics.h dfa.h globals.h ins.h parser.h re.h scanner.h \
	substr.h token.h mbo_getopt.h code.h stream_lc.h code_names.h
BUILT_SOURCES = parser.cc scanner.cc

YFLAGS       = -d

RE2C         = re2c$(EXEEXT)
RE2CFLAGS    = -bi

CLEANFILES   = parser.cc y.tab.c y.tab.h scanner.cc re2c.1 .version htdocs/manual.html

DISTCLEANFILES = makerpm re2c.spec README scanner.cc re2c$(EXEEXT)
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.h.in config.h.in~ config_w32.h.in configure depcomp install-sh missing bootstrap/parser.cc bootstrap/y.tab.h


parser.cc: parser.y
	$(YACC) $(YFLAGS) parser.y || exit
	cat y.tab.c | sed -e 's/"y\.tab\.c"/"parser.cc"/g' -e 's/"\.\/parser\.y"/"parser.y"/g' > parser.cc
	rm -f y.tab.c
	mv -f y.tab.h y.tab.c
	cat y.tab.c | sed -e 's/"\.\/parser\.y"/"parser.y"/g' > y.tab.h
	rm -f y.tab.c
	if cmp -s parser.cc bootstrap/parser.cc; then :; else cp -f parser.cc bootstrap/parser.cc; fi
	if cmp -s y.tab.h y.tab.h; then :; else mv -f y.tab.h y.tab.h; fi
	if cmp -s y.tab.h bootstrap/y.tab.h; then :; else cp -f y.tab.h bootstrap/y.tab.h; fi

scanner.cc: scanner.re
	@if test -x ./re2c$(EXEEXT); then \
		echo "re2c $(RE2CFLAGS) -o $@ scanner.re"; \
		./re2c $(RE2CFLAGS) -o $@ scanner.re && cp $@ bootstrap/; \
	else \
		echo "cp -f bootstrap/$@ $@"; \
		cp -f bootstrap/$@ $@; \
	fi
