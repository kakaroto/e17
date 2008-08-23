
AUTOMAKE_OPTIONS = foreign

SUBDIRS = api epplets

EXTRA_DIST = autogen.sh epplets.spec

changelog:
	cvs2cl -l "-d>20050101" --gmt --separate-header --tagdates --no-indent
