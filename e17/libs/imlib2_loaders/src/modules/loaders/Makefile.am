MAINTAINERCLEANFILES = Makefile.in
AUTOMAKE_OPTIONS     = 1.4 foreign

INCLUDES             = -I. \
                       -I$(top_srcdir) \
                       -I$(top_srcdir)/src/modules/loaders \
		       @EET_CFLAGS@ \
                       @my_includes@

pkgdir               = $(libdir)/imlib2/loaders

if BUILD_EDB_LOADER
EDB_L                = db.la
endif
if BUILD_EET_LOADER
EET_L                = eet.la
endif
if BUILD_XCF_LOADER
XCF_L                = xcf.la
endif

pkg_LTLIBRARIES      = \
$(EDB_L) \
$(EET_L) \
$(XCF_L) \
ico.la \
ani.la

db_la_SOURCES       = loader_db.c common.h image.h
db_la_INCLUDES      = @EDB_CFLAGS@
db_la_LDFLAGS       = -module -avoid-version
db_la_LIBADD        = @EDB_LIBS@

eet_la_SOURCES      = loader_eet.c common.h image.h
eet_la_INCLUDES     = @EET_CFLAGS@
eet_la_LDFLAGS      = -module -avoid-version
eet_la_LIBADD       = @EET_LIBS@

xcf_la_SOURCES      = loader_xcf.c loader_xcf_pixelfuncs.c image.h common.h color_values.h
xcf_la_LDFLAGS      = -module -avoid-version
xcf_la_LIBADD       = 

ico_la_SOURCES      = loader_ico.c common.h image.h
ico_la_LDFLAGS      = -module -avoid-version
ico_la_LIBADD       = 

ani_la_SOURCES      = loader_ani.c common.h image.h
ani_la_LDFLAGS      = -module -avoid-version
ani_la_LIBADD       = 
