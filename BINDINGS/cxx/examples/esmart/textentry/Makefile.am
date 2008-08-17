## Process this file with automake to produce Makefile.in

INCLUDES =

AM_CXXFLAGS =\
         @PACKAGE_CFLAGS@ \
	 -I$(top_srcdir)/src/ecore/ \
	 -I$(top_srcdir)/src/evas/ \
	 -I$(top_srcdir)/src/edje/ \
	 -I$(top_srcdir)/src/esmart/ \
	 -I$(top_srcdir)/src/common/ \
	 -Wall


bin_PROGRAMS = eflpp_esmart_textentry

eflpp_esmart_textentry_SOURCES = \
	main.cpp	

eflpp_esmart_textentry_LDFLAGS = @PACKAGE_LIBS@

eflpp_esmart_textentry_LDADD = $(top_builddir)/src/libeflpp.la

eflpp_esmart_textentry_DEPENDENCIES = $(top_builddir)/src/libeflpp.la
