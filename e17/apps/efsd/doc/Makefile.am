## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in 

EXTRA_DIST = efsd-manual.sgml

MANUAL = $(top_srcdir)/efsd/libefsd.c

if HAVE_DOCGEN
manual: efsd-manual.raw $(MANUAL)
	TOPDIR=@LINUX_SRC@ @DOCGEN@ $(MANUAL) <efsd-manual.raw >efsd-manual.sgml

if HAVE_JADE
html-manual: manual html-customizations.dsl
	$(mkinstalldirs) ./html
	cd html && @JADE@ -t sgml -d ../html-customizations.dsl ../efsd-manual.sgml
endif

endif
