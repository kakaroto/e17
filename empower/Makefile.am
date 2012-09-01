## Process this file with automake to produce Makefile.in
SUBDIRS = src data

if HAVE_PO
SUBDIRS += po
endif

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
                       ltconfig ltmain.sh missing mkinstalldirs \
                       stamp-h.in  empower.spec compile depcomp \
                       empower-*.tar.gz empower-* ABOUT-NLS* \
                       config.rpath m4/* m4

if HAVE_PO

MAINTAINERCLEANFILES += \
po/boldquot.sed \
po/en@boldquot.header \
po/en@quot.header \
po/insert-header.sin \
po/Makefile.in.in \
po/Makevars.template \
po/quot.sed \
po/remove-potcdate.sin \
po/Rules-quot \
po/empower.pot

endif

EXTRA_DIST = config.rpath README AUTHORS COPYING empower.spec

filesdir = $(datadir)/empower/
files_DATA = AUTHORS COPYING

ACLOCAL_AMFLAGS = -I m4