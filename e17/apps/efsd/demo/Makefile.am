## Process this file with automake to produce Makefile.in

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile

INCLUDES = \
	-I$(top_srcdir)/efsd

bin_PROGRAMS = efsddemo

efsddemo_SOURCES = \
efsddemo.c

efsddemo_LDADD = -L$(top_srcdir)/efsd/.libs -lefsd

EXTRA_DIST = run_efsddemo.sh

all-local: run_efsddemo.sh
	chmod 755 run_efsddemo.sh
