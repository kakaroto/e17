MAINTAINERCLEANFILES = Makefile.in


if EYESIGHT_BUILD_POPPLER
if !EYESIGHT_STATIC_BUILD_POPPLER

pkgdir = $(libdir)/eyesight

pkg_LTLIBRARIES = pdf.la
pdf_la_SOURCES = \
eyesight_pdf.cpp \
eyesight_pdf.h
pdf_la_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
@EYESIGHT_CFLAGS@ \
@POPPLER_CFLAGS@
pdf_la_CXXFLAGS = -fno-rtti -fno-exceptions
pdf_la_LIBADD  = @EYESIGHT_LIBS@ @POPPLER_LIBS@ $(top_builddir)/src/lib/libeyesight.la -lm
pdf_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -module -avoid-version
pdf_la_LIBTOOLFLAGS = --tag=disable-static
endif
endif
