## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LDFLAGS		     = -L$(top_builddir)/src -L$(top_builddir)/src/.libs -L/usr/X11R6/lib
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src \
                       -I$(top_srcdir)/loaders \
                       -I/usr/X11R6/include $(X_CFLAGS) \
                       -I$(prefix)/include -I$(includedir)

pkgdir		     = $(libdir)/imlib2_loaders/filter
pkg_LTLIBRARIES	     = testfilter.la bump_map.la colormod.la

testfilter_la_SOURCES  = filter_test.c
testfilter_la_LDFLAGS  = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
testfilter_la_LIBADD   = -lImlib2

bump_map_la_SOURCES    = filter_bumpmap.c
bump_map_la_LDFLAGS    = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
bump_map_la_LIBADD     = -lImlib2

colormod_la_SOURCES    = filter_colormod.c
colormod_la_LDFLAGS    = -no-undefined -module -avoid-version -L$(top_builddir)/src -L$(top_builddir)/src/.libs
colormod_la_LIBADD     = -lImlib2
