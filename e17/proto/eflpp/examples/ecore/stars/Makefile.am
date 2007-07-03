## Process this file with automake to produce Makefile.in

INCLUDES =

AM_CXXFLAGS =\
         @PACKAGE_CFLAGS@ \
	 -I$(top_srcdir)/src/ecore/ \
	 -I$(top_srcdir)/src/evas/ \
	 -I$(top_srcdir)/src/common/ \
	 -Wall


bin_PROGRAMS = eflpp_ecore_stars

eflpp_ecore_stars_SOURCES = \
	main.cpp	

eflpp_ecore_stars_LDFLAGS = @PACKAGE_LIBS@

eflpp_ecore_stars_LDADD = $(top_builddir)/src/libeflpp.la

eflpp_ecore_stars_DEPENDENCIES = $(top_builddir)/src/libeflpp.la
