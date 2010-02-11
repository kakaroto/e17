include $(top_srcdir)/Makefile.decl
NULL =

SUBDIRS = vapi tests

if MAKE_LIBRARY
  SUBDIRS += eflvala
endif

if MAKE_EXAMPLES
  SUBDIRS += examples
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eflvala-1.0.pc

EXTRA_DIST += eflvala-1.0.pc

MAINTAINERCLEANFILES = aclocal.m4 compile config.guess config.sub \
                       configure depcomp install-sh ltmain.sh     \
                       Makefile.in missing config.h.in            \
                       mkinstalldirs \
                       eflvala-1.0.pc \
                       $(NULL)

