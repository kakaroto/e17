MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = 					\
-I$(top_srcdir)/src/lib 			\
-I$(top_srcdir)/src/include 			\
-I$(top_builddir)/src/include 			\
-I$(top_builddir)/src/lib 			\
@CHECK_CFLAGS@ @EINA_CFLAGS@ @LIBXML2_CFLAGS@

if EFL_ENABLE_TESTS

check_PROGRAMS = eupnp_suite

eupnp_suite_SOURCES = 	\
eupnp_suite.c 		\
eupnp_av_test.c

eupnp_suite_LDADD = @LIBXML2_LIBS@ @EINA_LIBS@ @CHECK_LIBS@ $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_av/libeupnp_av.la


endif

EXTRA_DIST = eupnp_suite.h
