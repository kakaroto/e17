## Process this file with automake to produce Makefile.in

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile efsddemo

INCLUDES = -I$(top_srcdir)/efsd

bin_PROGRAMS = efsddemo ecore_efsddemo

efsddemo_SOURCES = \
efsddemo.c

ecore_efsddemo_SOURCES = \
ecore_efsddemo.c

efsddemo_LDADD = -L$(top_builddir)/efsd/ -lefsd @ecore_libs@
ecore_efsddemo_LDADD = -L$(top_builddir)/efsd/ -lefsd @ecore_libs@
