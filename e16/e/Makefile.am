SUBDIRS = intl dox eesh epp src config themes man scripts misc po

EXTRA_DIST = \
timestamp.h \
sample-scripts/bouncingball.pl \
sample-scripts/lcdmover.sh \
sample-scripts/testroller.pl \
sample-scripts/shade-pagers.pl \
e16.spec.in \
configure configure.in config.h.in \
ABOUT-NLS AUTHORS COMPLIANCE COPYING ChangeLog INSTALL NEWS README \
docs/README-0.16.8

ACLOCAL_AMFLAGS = -I m4

changelog:
	cvs2cl --gmt --no-hide-branch-additions --separate-header --no-indent	
