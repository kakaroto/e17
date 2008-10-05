AUTOMAKE_OPTIONS = foreign

MAINTAINERCLEANFILES = aclocal.m4 config.* configure depcomp \
		       install-sh ltmain.sh missing mkinstalldirs \
		       Makefile.in */Makefile.in */*/Makefile.in \
		       po/Makefile.in.in po/Makevars.template po/Rules-quot \
		       po/*.header po/*.sed po/*.sin po/e16.pot \
		       *~ */*~

SUBDIRS = dox eesh epp src lib config themes man scripts misc po

EXTRA_DIST = \
sample-scripts/bouncingball.pl \
sample-scripts/lcdmover.sh \
sample-scripts/testroller.pl \
sample-scripts/shade-pagers.pl \
AUTHORS COMPLIANCE COPYING ChangeLog README \
docs/README-0.16.8

ACLOCAL_AMFLAGS = -I m4

changelog:
	cvs2cl --gmt --no-hide-branch-additions --separate-header --no-indent	
