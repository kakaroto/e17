## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile

CFLAGS_EDB           = @edb_includes@
CFLAGS_IMLIB2        = @imlib2_includes@
CFLAGS_EXTRA         = -I.. -W -Wall 

LIBS_EDB             = @edb_libs@
LIBS_IMLIB2          = @imlib2_includes@

LIBS_ALL             = $(LIBS_EDB) $(LIBS_IMLIB2)
CFLAGS_ALL           = $(CFLAGS_EDB) $(CFLAGS_IMLIB2) $(CFLAGS_EXTRA) \
                       $(X_CFLAGS)

INCLUDES             = $(CFLAGS_ALL)

pkgdir		     = $(libdir)/loaders/image

if XCF_LOADER
XCF_L               = xcf.la
endif

if EDB_LOADER
EDB_L               = db.la
endif

pkg_LTLIBRARIES      = $(XCF_L) $(EDB_L)

xcf_la_SOURCES       = \
	loader_xcf.c \
	loader_xcf_pixelfuncs.c \
	image.h \
	common.h \
	color_values.h
xcf_la_LDFLAGS	     = -no-undefined -module -avoid-version
xcf_la_LIBADD	     = $(LIBS_IMLIB2)

db_la_SOURCES       = \
	loader_db.c \
    common.h \
    image.h
db_la_LDFLAGS	     = -no-undefined -module -avoid-version
db_la_LIBADD	     = $(LIBS_EDB) -ledb
