## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in 

EXTRA_DIST = efsd-manual.sgml

MANUAL = $(top_srcdir)/efsd/libefsd.h

if HAVE_DOCGEN
efsd-manual.sgml: efsd-manual.raw $(MANUAL)
	TOPDIR=@LINUX_SRC@ @DOCGEN@ $(MANUAL) <efsd-manual.raw >efsd-manual.sgml
endif

if HAVE_JADE
html-manual: efsd-manual.sgml html-customizations.dsl
	$(mkinstalldirs) ./html
	cd html && @JADE@ -t sgml -d ../html-customizations.dsl ../efsd-manual.sgml
endif

