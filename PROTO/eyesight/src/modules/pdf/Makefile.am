MAINTAINERCLEANFILES = Makefile.in


if EYESIGHT_BUILD_PDF
if !EYESIGHT_STATIC_BUILD_PDF

includesdir = $(includedir)/eyesight
includes_HEADERS = Eyesight_Module_Pdf.h

pkgdir = $(libdir)/eyesight

pkg_LTLIBRARIES = pdf.la

if HAVE_PDF_BACKEND_MUPDF

noinst_PROGRAMS = cmapdump fontdump
BUILT_SOURCES = cmapdump$(EXEEXT) fontdump$(EXEEXT)

cmapdump_SOURCES = mupdf-0.7/mupdf/cmapdump.c
cmapdump_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/fitz

fontdump_SOURCES = mupdf-0.7/mupdf/fontdump.c
fontdump_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/fitz

noinst_LTLIBRARIES = libfitz.la libdraw.la libcmaps.la libfonts.la libmupdf.la

libfitz_la_SOURCES = \
mupdf-0.7/fitz/base_error.c \
mupdf-0.7/fitz/base_geometry.c \
mupdf-0.7/fitz/base_getopt.c \
mupdf-0.7/fitz/base_hash.c \
mupdf-0.7/fitz/base_memory.c \
mupdf-0.7/fitz/base_string.c \
mupdf-0.7/fitz/base_time.c \
mupdf-0.7/fitz/crypt_aes.c \
mupdf-0.7/fitz/crypt_arc4.c \
mupdf-0.7/fitz/crypt_md5.c \
mupdf-0.7/fitz/dev_bbox.c \
mupdf-0.7/fitz/dev_draw.c \
mupdf-0.7/fitz/dev_list.c \
mupdf-0.7/fitz/dev_null.c \
mupdf-0.7/fitz/dev_text.c \
mupdf-0.7/fitz/dev_trace.c \
mupdf-0.7/fitz/filt_basic.c \
mupdf-0.7/fitz/filt_dctd.c \
mupdf-0.7/fitz/filt_faxd.c \
mupdf-0.7/fitz/filt_flate.c \
mupdf-0.7/fitz/filt_jbig2d.c \
mupdf-0.7/fitz/filt_jpxd.c \
mupdf-0.7/fitz/filt_lzwd.c \
mupdf-0.7/fitz/filt_predict.c \
mupdf-0.7/fitz/obj_array.c \
mupdf-0.7/fitz/obj_dict.c \
mupdf-0.7/fitz/obj_print.c \
mupdf-0.7/fitz/obj_simple.c \
mupdf-0.7/fitz/res_colorspace.c \
mupdf-0.7/fitz/res_font.c \
mupdf-0.7/fitz/res_path.c \
mupdf-0.7/fitz/res_pixmap.c \
mupdf-0.7/fitz/res_shade.c \
mupdf-0.7/fitz/res_text.c \
mupdf-0.7/fitz/stm_buffer.c \
mupdf-0.7/fitz/stm_open.c \
mupdf-0.7/fitz/stm_read.c

libfitz_la_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/mupdf \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/mupdf \
@MUPDF_CFLAGS@
libfitz_la_LIBADD = @MUPDF_LIBS@

libdraw_la_SOURCES = \
mupdf-0.7/draw/archport.c \
mupdf-0.7/draw/blendmodes.c \
mupdf-0.7/draw/glyphcache.c \
mupdf-0.7/draw/imagedraw.c \
mupdf-0.7/draw/imagescale.c \
mupdf-0.7/draw/imagesmooth.c \
mupdf-0.7/draw/imageunpack.c \
mupdf-0.7/draw/meshdraw.c \
mupdf-0.7/draw/pathfill.c \
mupdf-0.7/draw/pathscan.c \
mupdf-0.7/draw/pathstroke.c \
mupdf-0.7/draw/porterduff.c

libdraw_la_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/draw \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/draw
libdraw_la_LIBADD =

libcmaps_la_SOURCES = \
cmap_tounicode.c \
cmap_cns.c \
cmap_gb.c \
cmap_japan.c \
cmap_korea.c

libcmaps_la_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/mupdf \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/mupdf

cmap_tounicode_files = \
mupdf-0.7/cmaps/Adobe-CNS1-UCS2 \
mupdf-0.7/cmaps/Adobe-GB1-UCS2 \
mupdf-0.7/cmaps/Adobe-Japan1-UCS2 \
mupdf-0.7/cmaps/Adobe-Korea1-UCS2

cmap_cns_files = \
mupdf-0.7/cmaps/Adobe-CNS1-0 \
mupdf-0.7/cmaps/Adobe-CNS1-1 \
mupdf-0.7/cmaps/Adobe-CNS1-2 \
mupdf-0.7/cmaps/Adobe-CNS1-3 \
mupdf-0.7/cmaps/Adobe-CNS1-4 \
mupdf-0.7/cmaps/Adobe-CNS1-5 \
mupdf-0.7/cmaps/Adobe-CNS1-6 \
mupdf-0.7/cmaps/B5-H \
mupdf-0.7/cmaps/B5-V \
mupdf-0.7/cmaps/B5pc-H \
mupdf-0.7/cmaps/B5pc-V \
mupdf-0.7/cmaps/CNS-EUC-H \
mupdf-0.7/cmaps/CNS-EUC-V \
mupdf-0.7/cmaps/CNS1-H \
mupdf-0.7/cmaps/CNS1-V \
mupdf-0.7/cmaps/CNS2-H \
mupdf-0.7/cmaps/CNS2-V \
mupdf-0.7/cmaps/ETen-B5-H \
mupdf-0.7/cmaps/ETen-B5-V \
mupdf-0.7/cmaps/ETenms-B5-H \
mupdf-0.7/cmaps/ETenms-B5-V \
mupdf-0.7/cmaps/ETHK-B5-H \
mupdf-0.7/cmaps/ETHK-B5-V \
mupdf-0.7/cmaps/HKdla-B5-H \
mupdf-0.7/cmaps/HKdla-B5-V \
mupdf-0.7/cmaps/HKdlb-B5-H \
mupdf-0.7/cmaps/HKdlb-B5-V \
mupdf-0.7/cmaps/HKgccs-B5-H \
mupdf-0.7/cmaps/HKgccs-B5-V \
mupdf-0.7/cmaps/HKm314-B5-H \
mupdf-0.7/cmaps/HKm314-B5-V \
mupdf-0.7/cmaps/HKm471-B5-H \
mupdf-0.7/cmaps/HKm471-B5-V \
mupdf-0.7/cmaps/HKscs-B5-H \
mupdf-0.7/cmaps/HKscs-B5-V \
mupdf-0.7/cmaps/UniCNS-UCS2-H \
mupdf-0.7/cmaps/UniCNS-UCS2-V \
mupdf-0.7/cmaps/UniCNS-UTF16-H \
mupdf-0.7/cmaps/UniCNS-UTF16-V

cmap_gb_files = \
mupdf-0.7/cmaps/Adobe-GB1-0 \
mupdf-0.7/cmaps/Adobe-GB1-1 \
mupdf-0.7/cmaps/Adobe-GB1-2 \
mupdf-0.7/cmaps/Adobe-GB1-3 \
mupdf-0.7/cmaps/Adobe-GB1-4 \
mupdf-0.7/cmaps/Adobe-GB1-5 \
mupdf-0.7/cmaps/GB-EUC-H \
mupdf-0.7/cmaps/GB-EUC-V \
mupdf-0.7/cmaps/GB-H \
mupdf-0.7/cmaps/GB-V \
mupdf-0.7/cmaps/GBK-EUC-H \
mupdf-0.7/cmaps/GBK-EUC-V \
mupdf-0.7/cmaps/GBK2K-H \
mupdf-0.7/cmaps/GBK2K-V \
mupdf-0.7/cmaps/GBKp-EUC-H \
mupdf-0.7/cmaps/GBKp-EUC-V \
mupdf-0.7/cmaps/GBpc-EUC-H \
mupdf-0.7/cmaps/GBpc-EUC-V \
mupdf-0.7/cmaps/GBT-EUC-H \
mupdf-0.7/cmaps/GBT-EUC-V \
mupdf-0.7/cmaps/GBT-H \
mupdf-0.7/cmaps/GBT-V \
mupdf-0.7/cmaps/GBTpc-EUC-H \
mupdf-0.7/cmaps/GBTpc-EUC-V \
mupdf-0.7/cmaps/UniGB-UCS2-H \
mupdf-0.7/cmaps/UniGB-UCS2-V \
mupdf-0.7/cmaps/UniGB-UTF16-H \
mupdf-0.7/cmaps/UniGB-UTF16-V

cmap_japan_files = \
mupdf-0.7/cmaps/78-EUC-H \
mupdf-0.7/cmaps/78-EUC-V \
mupdf-0.7/cmaps/78-H \
mupdf-0.7/cmaps/78-RKSJ-H \
mupdf-0.7/cmaps/78-RKSJ-V \
mupdf-0.7/cmaps/78-V \
mupdf-0.7/cmaps/78ms-RKSJ-H \
mupdf-0.7/cmaps/78ms-RKSJ-V \
mupdf-0.7/cmaps/83pv-RKSJ-H \
mupdf-0.7/cmaps/90ms-RKSJ-H \
mupdf-0.7/cmaps/90ms-RKSJ-V \
mupdf-0.7/cmaps/90msp-RKSJ-H \
mupdf-0.7/cmaps/90msp-RKSJ-V \
mupdf-0.7/cmaps/90pv-RKSJ-H \
mupdf-0.7/cmaps/90pv-RKSJ-V \
mupdf-0.7/cmaps/Add-H \
mupdf-0.7/cmaps/Add-RKSJ-H \
mupdf-0.7/cmaps/Add-RKSJ-V \
mupdf-0.7/cmaps/Add-V \
mupdf-0.7/cmaps/Adobe-Japan1-0 \
mupdf-0.7/cmaps/Adobe-Japan1-1 \
mupdf-0.7/cmaps/Adobe-Japan1-2 \
mupdf-0.7/cmaps/Adobe-Japan1-3 \
mupdf-0.7/cmaps/Adobe-Japan1-4 \
mupdf-0.7/cmaps/Adobe-Japan1-5 \
mupdf-0.7/cmaps/Adobe-Japan1-6 \
mupdf-0.7/cmaps/EUC-H \
mupdf-0.7/cmaps/EUC-V \
mupdf-0.7/cmaps/Ext-H \
mupdf-0.7/cmaps/Ext-RKSJ-H \
mupdf-0.7/cmaps/Ext-RKSJ-V \
mupdf-0.7/cmaps/Ext-V \
mupdf-0.7/cmaps/H \
mupdf-0.7/cmaps/Hankaku \
mupdf-0.7/cmaps/Hiragana \
mupdf-0.7/cmaps/Katakana \
mupdf-0.7/cmaps/NWP-H \
mupdf-0.7/cmaps/NWP-V \
mupdf-0.7/cmaps/RKSJ-H \
mupdf-0.7/cmaps/RKSJ-V \
mupdf-0.7/cmaps/Roman \
mupdf-0.7/cmaps/UniJIS-UCS2-H \
mupdf-0.7/cmaps/UniJIS-UCS2-HW-H \
mupdf-0.7/cmaps/UniJIS-UCS2-HW-V \
mupdf-0.7/cmaps/UniJIS-UCS2-V \
mupdf-0.7/cmaps/UniJISPro-UCS2-HW-V \
mupdf-0.7/cmaps/UniJISPro-UCS2-V \
mupdf-0.7/cmaps/V \
mupdf-0.7/cmaps/WP-Symbol \
mupdf-0.7/cmaps/Adobe-Japan2-0 \
mupdf-0.7/cmaps/Hojo-EUC-H \
mupdf-0.7/cmaps/Hojo-EUC-V \
mupdf-0.7/cmaps/Hojo-H \
mupdf-0.7/cmaps/Hojo-V \
mupdf-0.7/cmaps/UniHojo-UCS2-H \
mupdf-0.7/cmaps/UniHojo-UCS2-V \
mupdf-0.7/cmaps/UniHojo-UTF16-H \
mupdf-0.7/cmaps/UniHojo-UTF16-V \
mupdf-0.7/cmaps/UniJIS-UTF16-H \
mupdf-0.7/cmaps/UniJIS-UTF16-V

cmap_korea_files = \
mupdf-0.7/cmaps/Adobe-Korea1-0 \
mupdf-0.7/cmaps/Adobe-Korea1-1 \
mupdf-0.7/cmaps/Adobe-Korea1-2 \
mupdf-0.7/cmaps/KSC-EUC-H \
mupdf-0.7/cmaps/KSC-EUC-V \
mupdf-0.7/cmaps/KSC-H \
mupdf-0.7/cmaps/KSC-Johab-H \
mupdf-0.7/cmaps/KSC-Johab-V \
mupdf-0.7/cmaps/KSC-V \
mupdf-0.7/cmaps/KSCms-UHC-H \
mupdf-0.7/cmaps/KSCms-UHC-HW-H \
mupdf-0.7/cmaps/KSCms-UHC-HW-V \
mupdf-0.7/cmaps/KSCms-UHC-V \
mupdf-0.7/cmaps/KSCpc-EUC-H \
mupdf-0.7/cmaps/KSCpc-EUC-V \
mupdf-0.7/cmaps/UniKS-UCS2-H \
mupdf-0.7/cmaps/UniKS-UCS2-V \
mupdf-0.7/cmaps/UniKS-UTF16-H \
mupdf-0.7/cmaps/UniKS-UTF16-V

cmap_tounicode.c: cmapdump $(cmap_tounicode_files)
	./cmapdump $@ $(cmap_tounicode_files)

cmap_cns.c: cmapdump $(cmap_cns_files)
	./cmapdump $@ $(cmap_cns_files)

cmap_gb.c: cmapdump $(cmap_gb_files)
	./cmapdump $@ $(cmap_gb_files)

cmap_japan.c: cmapdump $(cmap_japan_files)
	./cmapdump $@ $(cmap_japan_files)

cmap_korea.c: cmapdump $(cmap_korea_files)
	./cmapdump $@ $(cmap_korea_files)

libfonts_la_SOURCES = \
font_base.c

libfonts_la_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/fitz

if BUILD_MUPDF_CJK_FONTS

libfonts_la_SOURCES += font_cjk.c

endif

font_base_files = \
mupdf-0.7/fonts/Dingbats.cff \
mupdf-0.7/fonts/NimbusMonL-Bold.cff \
mupdf-0.7/fonts/NimbusMonL-BoldObli.cff \
mupdf-0.7/fonts/NimbusMonL-Regu.cff \
mupdf-0.7/fonts/NimbusMonL-ReguObli.cff \
mupdf-0.7/fonts/NimbusRomNo9L-Medi.cff \
mupdf-0.7/fonts/NimbusRomNo9L-MediItal.cff \
mupdf-0.7/fonts/NimbusRomNo9L-Regu.cff \
mupdf-0.7/fonts/NimbusRomNo9L-ReguItal.cff \
mupdf-0.7/fonts/NimbusSanL-Bold.cff \
mupdf-0.7/fonts/NimbusSanL-BoldItal.cff \
mupdf-0.7/fonts/NimbusSanL-Regu.cff \
mupdf-0.7/fonts/NimbusSanL-ReguItal.cff \
mupdf-0.7/fonts/StandardSymL.cff \
mupdf-0.7/fonts/URWChanceryL-MediItal.cff

font_cjk_files = \
mupdf-0.7/fonts/droid/DroidSansFallback.ttf

font_base.c: fontdump $(font_base_files)
	./fontdump $@ $(font_base_files)

font_cjk.c: fontdump $(font_cjk_files)
	./fontdump $@ $(font_cjk_files)

libmupdf_la_SOURCES = \
mupdf-0.7/mupdf/pdf_annot.c \
mupdf-0.7/mupdf/pdf_build.c \
mupdf-0.7/mupdf/pdf_cmap.c \
mupdf-0.7/mupdf/pdf_cmap_load.c \
mupdf-0.7/mupdf/pdf_cmap_parse.c \
mupdf-0.7/mupdf/pdf_cmap_table.c \
mupdf-0.7/mupdf/pdf_colorspace.c \
mupdf-0.7/mupdf/pdf_crypt.c \
mupdf-0.7/mupdf/pdf_debug.c \
mupdf-0.7/mupdf/pdf_font.c \
mupdf-0.7/mupdf/pdf_fontagl.c \
mupdf-0.7/mupdf/pdf_fontenc.c \
mupdf-0.7/mupdf/pdf_fontfile.c \
mupdf-0.7/mupdf/pdf_fontmtx.c \
mupdf-0.7/mupdf/pdf_function.c \
mupdf-0.7/mupdf/pdf_image.c \
mupdf-0.7/mupdf/pdf_interpret.c \
mupdf-0.7/mupdf/pdf_lex.c \
mupdf-0.7/mupdf/pdf_nametree.c \
mupdf-0.7/mupdf/pdf_outline.c \
mupdf-0.7/mupdf/pdf_page.c \
mupdf-0.7/mupdf/pdf_pagetree.c \
mupdf-0.7/mupdf/pdf_parse.c \
mupdf-0.7/mupdf/pdf_pattern.c \
mupdf-0.7/mupdf/pdf_repair.c \
mupdf-0.7/mupdf/pdf_shade.c \
mupdf-0.7/mupdf/pdf_store.c \
mupdf-0.7/mupdf/pdf_stream.c \
mupdf-0.7/mupdf/pdf_type3.c \
mupdf-0.7/mupdf/pdf_unicode.c \
mupdf-0.7/mupdf/pdf_xobject.c \
mupdf-0.7/mupdf/pdf_xref.c

libmupdf_la_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/mupdf \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/mupdf \
@BUILD_MUPDF_CJK_FONTS@ \
@MUPDF_CFLAGS@
libmupdf_la_LIBADD = @MUPDF_LIBS@

pdf_la_SOURCES = \
eyesight_pdf_mupdf.c \
eyesight_pdf_mupdf.h
pdf_la_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/fitz \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.7/mupdf \
-I$(top_builddir)/src/modules/pdf/mupdf-0.7/mupdf \
@EFL_EYESIGHT_BUILD@ \
@EYESIGHT_CFLAGS@
pdf_la_LIBADD  = \
@EYESIGHT_LIBS@ \
$(top_builddir)/src/lib/libeyesight.la \
libfitz.la \
libdraw.la \
libcmaps.la \
libfonts.la \
libmupdf.la \
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

clean-local:
	-rm -f cmap_*.c font_*.c

EXTRA_DIST = mupdf-0.7