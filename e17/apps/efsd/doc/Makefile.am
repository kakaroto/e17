## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in 

EXTRA_DIST = efsd-manual.sgml

MANUAL = $(top_srcdir)/efsd/libefsd.c

manual: efsd-manual.raw $(MANUAL)
	TOPDIR=@LINUX_SRC@ @DOCGEN@ $(MANUAL) <efsd-manual.raw >efsd-manual.sgml
