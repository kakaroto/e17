SUBDIRS = 

MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

noinst_LTLIBRARIES = libeon_script.la

EXTRA_DIST = 

libeon_script_la_SOURCES =
libeon_script_la_DEPENDENCIES = 
libeon_script_la_LIBADD = 

if BUILD_SCRIPT_NEKO

SUBDIRS += neko

libeon_script_la_DEPENDENCIES += $(top_builddir)/src/lib/script/neko/libeon_script_neko.la
libeon_script_la_LIBADD += $(top_builddir)/src/lib/script/neko/libeon_script_neko.la

endif
