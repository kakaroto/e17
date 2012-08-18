MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

if EYESIGHT_BUILD_PDF

SUBDIRS += mupdf-1.1

if !EYESIGHT_STATIC_BUILD_PDF

includesdir = $(includedir)/eyesight
includes_HEADERS = Eyesight_Module_Pdf.h

pkgdir = $(libdir)/eyesight


pkg_LTLIBRARIES = pdf.la

pdf_la_SOURCES = \
eyesight_pdf.c \
eyesight_pdf.h
pdf_la_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/modules/pdf/mupdf-1.1/cbz \
-I$(top_builddir)/src/modules/pdf/mupdf-1.1/cbz \
-I$(top_srcdir)/src/modules/pdf/mupdf-1.1/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-1.1/fitz \
-I$(top_srcdir)/src/modules/pdf/mupdf-1.1/pdf \
-I$(top_builddir)/src/modules/pdf/mupdf-1.1/pdf \
-I$(top_srcdir)/src/modules/pdf/mupdf-1.1/xps \
-I$(top_builddir)/src/modules/pdf/mupdf-1.1/xps \
@EFL_EYESIGHT_BUILD@ \
@EYESIGHT_CFLAGS@
pdf_la_LIBADD  = \
@EYESIGHT_LIBS@ \
$(top_builddir)/src/lib/libeyesight.la \
$(top_builddir)/src/modules/pdf/mupdf-1.1/libfitz.la \
-lm

pdf_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -module -avoid-version
pdf_la_LIBTOOLFLAGS = --tag=disable-static

install-data-hook:
	rm -f $(DESTDIR)$(pkgdir)/$(pkg_LTLIBRARIES)

endif
endif
