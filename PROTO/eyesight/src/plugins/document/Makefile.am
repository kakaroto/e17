AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src -I$(top_srcdir)/src/bin \
	 -I$(top_srcdir)/src/plugins/document @EYESIGHT_CFLAGS@ @EPDF_CFLAGS@ \
    @EDVI_CFLAGS@ @EPS_CFLAGS@ -I/usr/local/include/eps

plugindir = $(prefix)/lib/eyesight/plugins

plugin_LTLIBRARIES =


if HAVE_EPDF
plugin_LTLIBRARIES += pdf.la
pdf_la_SOURCES = document.c animations.c toolbar.c view.c
pdf_la_CFLAGS = -Wall -DPDF -Werror -g
pdf_la_LDFLAGS = -module -avoid-version -no-undefined
pdf_la_LIBTOOLFLAGS = --tag=disable-static
pdf_la_LIBADD = @EYESIGHT_LIBS@ @EPDF_LIBS@ @intl_libs@ @dlopen_libs@
endif

if HAVE_EDVI
plugin_LTLIBRARIES += dvi.la
dvi_la_SOURCES = document.c animations.c toolbar.c view.c
dvi_la_CFLAGS = -Wall -DDVI -Werror -g
dvi_la_LDFLAGS = -module -avoid-version -no-undefined
dvi_la_LIBTOOLFLAGS = --tag=disable-static
dvi_la_LIBADD = @EYESIGHT_LIBS@ @EDVI_LIBS@ @intl_libs@ @dlopen_libs@
endif

if HAVE_EPS
plugin_LTLIBRARIES += ps.la
ps_la_SOURCES = document.c animations.c toolbar.c view.c
ps_la_CFLAGS = -Wall -DPS -Werror -g
ps_la_LDFLAGS = -module -avoid-version -no-undefined
ps_la_LIBTOOLFLAGS = --tag=disable-static
ps_la_LIBADD = @EYESIGHT_LIBS@ @EPS_LIBS@ @intl_libs@ @dlopen_libs@
endif

EXTRA_DIST = animations.h document.h toolbar.h defines.h view.h
