
AUTOMAKE_OPTIONS = foreign

SUBDIRS = api epplets

EXTRA_DIST = autogen.sh epplets.spec

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.* configure depcomp \
		       install-sh ltmain.sh missing */Makefile.in

changelog:
	cvs2cl -l "-d>20050101" --gmt --separate-header --tagdates --no-indent
