## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LDFLAGS		     = -L/usr/X11R6/lib
INCLUDES	     = -I/usr/X11R6/include -I$(top_srcdir)/libltdl \
                       $(X_CFLAGS) -I$(prefix)/include -I$(includedir) \
		       -I. -I$(top_srcdir) -I$(top_srcdir)/src \
		       -I$(top_srcdir)/loaders

pkgdir		     = $(libdir)/loaders/filter
pkg_LTLIBRARIES	     = testfilter.la bump_map.la colormod.la

testfilter_la_SOURCES  = filter_test.c
testfilter_la_LDFLAGS  = -no-undefined -module -avoid-version -L$(top_builddir)/src
testfilter_la_LIBADD    =

bump_map_la_SOURCES    = filter_bumpmap.c
bump_map_la_LDFLAGS    = -no-undefined -module -avoid-version -L$(top_builddir)/src
bump_map_la_LIBADD      =

colormod_la_SOURCES    = filter_colormod.c
colormod_la_LDFLAGS    = -no-undefined -module -avoid-version -L$(top_builddir)/src
colormod_la_LIBADD      =
