## Process this file with automake to produce Makefile.in

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile efsddemo

INCLUDES = -I$(top_srcdir)/efsd

bin_PROGRAMS = efsddemo

efsddemo_SOURCES = \
efsddemo.c

efsddemo_LDADD = -L$(top_builddir)/efsd/.libs -lefsd @ecore_libs@
