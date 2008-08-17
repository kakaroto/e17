## Process this file with automake to produce Makefile.in

SUBDIRS = src po pixmaps help

e16menuedit2docdir = ${prefix}/${packagedocdir}
e16menuedit2doc_DATA = \
		README\
		COPYING\
		AUTHORS\
		ChangeLog\
		INSTALL\
		NEWS\
                TODO

GLADE_FILES = e16menuedit2.glade

e16menuedit2datadir = ${prefix}/${packagedatadir}/glade
e16menuedit2data_DATA = \
		$(GLADE_FILES)

INTLTOOL_DIST = intltool-extract.in \
		intltool-merge.in \
                intltool-update.in

EXTRA_DIST = $(INTLTOOL_DIST) \
	     $(GLADE_FILES) \
             xmldocs.make \
	     omf.make \
	     e16menuedit2.spec


