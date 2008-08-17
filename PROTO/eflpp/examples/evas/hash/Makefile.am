## Process this file with automake to produce Makefile.in

INCLUDES =

AM_CXXFLAGS =\
         @PACKAGE_CFLAGS@ \
	 -I$(top_srcdir)/src/ecore/ \
	 -I$(top_srcdir)/src/evas/ \
	 -I$(top_srcdir)/src/common/ \
	 -Wall


bin_PROGRAMS = eflpp_evas_hash

eflpp_evas_hash_SOURCES = \
	main.cpp	

eflpp_evas_hash_LDFLAGS = @PACKAGE_LIBS@

eflpp_evas_hash_LDADD = $(top_builddir)/src/libeflpp.la

eflpp_evas_hash_DEPENDENCIES = $(top_builddir)/src/libeflpp.la
