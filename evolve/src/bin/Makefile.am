AUTOMAKE_OPTIONS	= 1.5 foreign
MAINTAINERCLEANFILES 	= Makefile.in

AM_CFLAGS	= -W -Wall -Wno-implicit
INCLUDES	= -I$(top_srcdir)/src/lib @ETK_CFLAGS@

bin_PROGRAMS	= evolve_test evolve_cc evolve_decc evolve

evolve_test_DEPENDENCIES     = $(top_builddir)/src/lib/libevolve.la
evolve_test_SOURCES 	     = evolve_test_main.c 
evolve_test_LDADD	     = $(top_builddir)/src/lib/libevolve.la @ETK_LIBS@
evolve_test_LDFLAGS	     = -export-dynamic

evolve_DEPENDENCIES          = $(top_builddir)/src/lib/libevolve.la
evolve_SOURCES 	       	     = evolve_main.c 
evolve_LDADD	     	     = $(top_builddir)/src/lib/libevolve.la @ETK_LIBS@
evolve_LDFLAGS	     	     = -export-dynamic

evolve_cc_DEPENDENCIES 	     = $(top_builddir)/src/lib/libevolve.la
evolve_cc_SOURCES 	     = evolve_cc_main.c 
evolve_cc_LDADD		     = $(top_builddir)/src/lib/libevolve.la @ETK_LIBS@

evolve_decc_DEPENDENCIES     = $(top_builddir)/src/lib/libevolve.la
evolve_decc_SOURCES 	     = evolve_decc_main.c 
evolve_decc_LDADD	     = $(top_builddir)/src/lib/libevolve.la @ETK_LIBS@
