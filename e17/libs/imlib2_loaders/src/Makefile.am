## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile

CFLAGS_EDB           = @edb_includes@
CFLAGS_EET           = @eet_includes@
CFLAGS_IMLIB2        = @imlib2_includes@
CFLAGS_EXTRA         = -I.. -W -Wall 

LIBS_EDB             = @edb_libs@
LIBS_EET             = @eet_libs@
LIBS_IMLIB2          = @imlib2_libs@

LIBS_ALL             = $(LIBS_EET) $(LIBS_EDB) $(LIBS_IMLIB2)
CFLAGS_ALL           = $(CFLAGS_EET) $(CFLAGS_EDB) $(CFLAGS_IMLIB2) \
                       $(CFLAGS_EXTRA) $(X_CFLAGS)

INCLUDES             = $(CFLAGS_ALL)

pkgdir		     = $(libdir)/loaders/image

if XCF_LOADER
XCF_L               = xcf.la
endif

if EDB_LOADER
EDB_L               = db.la
endif

if EET_LOADER
EET_L               = eet.la
endif



pkg_LTLIBRARIES      = $(XCF_L) $(EDB_L) $(EET_L) ico.la ani.la

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
db_la_LIBADD	     = $(LIBS_IMLIB2) $(LIBS_EDB) -ledb -lz

eet_la_SOURCES       = \
	loader_eet.c \
	common.h \
	image.h
eet_la_LDFLAGS	     = -no-undefined -module -avoid-version
eet_la_LIBADD	     = $(LIBS_IMLIB2) $(LIBS_EET) -leet -lz

ico_la_SOURCES       = \
	loader_ico.c \
	image.h \
	common.h \
	color_values.h
ico_la_LDFLAGS	     = -no-undefined -module -avoid-version
ico_la_LIBADD	     = $(LIBS_IMLIB2)

ani_la_SOURCES       = \
	loader_ani.c \
	image.h \
	common.h \
	color_values.h
ani_la_LDFLAGS	     = -no-undefined -module -avoid-version
ani_la_LIBADD	     = $(LIBS_IMLIB2)
