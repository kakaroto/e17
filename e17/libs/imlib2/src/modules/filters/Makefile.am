MAINTAINERCLEANFILES = Makefile.in
AUTOMAKE_OPTIONS     = 1.4 foreign

INCLUDES             = -I. \
                       -I$(top_srcdir) \
		       -I$(top_srcdir)/src/lib \
		       -I$(top_srcdir)/src/modules/filters \
		       @my_includes@
		       
pkgdir                = $(libdir)/imlib2/filters
pkg_LTLIBRARIES       = testfilter.la bumpmap.la colormod.la

testfilter_la_SOURCES = filter_test.c
testfilter_la_LDFLAGS = $(LDFLAGS) -module -avoid-version

bumpmap_la_SOURCES    = filter_bumpmap.c
bumpmap_la_LDFLAGS    = $(LDFLAGS) -module -avoid-version

colormod_la_SOURCES   = filter_colormod.c
colormod_la_LDFLAGS   = $(LDFLAGS) -module -avoid-version

