MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =


if EYESIGHT_BUILD_PDF
if !EYESIGHT_STATIC_BUILD_PDF

includesdir = $(includedir)/eyesight
includes_HEADERS = Eyesight_Module_Pdf.h

pkgdir = $(libdir)/eyesight


pkg_LTLIBRARIES = pdf.la

if HAVE_PDF_BACKEND_MUPDF

SUBDIRS += mupdf-0.9

pdf_la_SOURCES = \
eyesight_pdf_mupdf.c \
eyesight_pdf_mupdf.h
pdf_la_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.9/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.9/fitz \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.9/pdf \
-I$(top_builddir)/src/modules/pdf/mupdf-0.9/pdf \
@EFL_EYESIGHT_BUILD@ \
@EYESIGHT_CFLAGS@
pdf_la_LIBADD  = \
@EYESIGHT_LIBS@ \
$(top_builddir)/src/lib/libeyesight.la \
$(top_builddir)/src/modules/pdf/mupdf-0.9/libfitz.la \
$(top_builddir)/src/modules/pdf/mupdf-0.9/libmupdf.la \
-lm
pdf_la_LINK = $(LINK) $(pdf_la_LDFLAGS)

else

pdf_la_SOURCES = \
eyesight_pdf_poppler.cpp \
eyesight_pdf_poppler.h
pdf_la_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
@EFL_EYESIGHT_BUILD@ \
@EYESIGHT_CFLAGS@ \
@POPPLER_CFLAGS@
pdf_la_CXXFLAGS = -fno-rtti -fno-exceptions
pdf_la_LIBADD  = \
@EYESIGHT_LIBS@ \
@POPPLER_LIBS@ \
$(top_builddir)/src/lib/libeyesight.la \
-lm
pdf_la_LINK = $(CXXLINK) $(pdf_la_LDFLAGS)

endif

pdf_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -module -avoid-version
pdf_la_LIBTOOLFLAGS = --tag=disable-static

install-data-hook:
	rm -f $(DESTDIR)$(pkgdir)/$(pkg_LTLIBRARIES)

endif
endif

EXTRA_DIST = mupdf-0.9
