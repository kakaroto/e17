
MAINTAINERCLEANFILES = Makefile.in

noinst_PROGRAMS = cmapdump fontdump
BUILT_SOURCES = cmapdump fontdump

cmapdump_SOURCES = mupdf-0.5/mupdf/cmapdump.c
cmapdump_CPPFLAGS = \
-I$(top_srcdir)/src/lib/mupdf/mupdf-0.5/fitz \
-I$(top_builddir)/src/lib/mupdf/mupdf-0.5/fitz
cmapdump_LDADD = libfitz.la

fontdump_SOURCES = mupdf-0.5/mupdf/fontdump.c
fontdump_CPPFLAGS = \
-I$(top_srcdir)/src/lib/mupdf/mupdf-0.5/fitz \
-I$(top_builddir)/src/lib/mupdf/mupdf-0.5/fitz
#fontdump_LDADD = libfitz.la

noinst_LTLIBRARIES = libfitz.la libfitzdraw.la libcmaps.la libfonts.la libmupdf.la libepdf_mupdf.la

libfitz_la_SOURCES = \
mupdf-0.5/fitz/base_cpudep.c \
mupdf-0.5/fitz/base_error.c \
mupdf-0.5/fitz/base_hash.c \
mupdf-0.5/fitz/base_matrix.c \
mupdf-0.5/fitz/base_memory.c \
mupdf-0.5/fitz/base_rect.c \
mupdf-0.5/fitz/base_string.c \
mupdf-0.5/fitz/base_unicode.c \
mupdf-0.5/fitz/crypt_aes.c \
mupdf-0.5/fitz/crypt_arc4.c \
mupdf-0.5/fitz/crypt_crc32.c \
mupdf-0.5/fitz/crypt_md5.c \
mupdf-0.5/fitz/obj_array.c \
mupdf-0.5/fitz/obj_dict.c \
mupdf-0.5/fitz/obj_parse.c \
mupdf-0.5/fitz/obj_print.c \
mupdf-0.5/fitz/obj_simple.c \
mupdf-0.5/fitz/stm_buffer.c \
mupdf-0.5/fitz/stm_filter.c \
mupdf-0.5/fitz/stm_open.c \
mupdf-0.5/fitz/stm_read.c \
mupdf-0.5/fitz/stm_misc.c \
mupdf-0.5/fitz/filt_pipeline.c \
mupdf-0.5/fitz/filt_basic.c \
mupdf-0.5/fitz/filt_arc4.c \
mupdf-0.5/fitz/filt_aesd.c \
mupdf-0.5/fitz/filt_dctd.c \
mupdf-0.5/fitz/filt_faxd.c \
mupdf-0.5/fitz/filt_faxdtab.c \
mupdf-0.5/fitz/filt_flate.c \
mupdf-0.5/fitz/filt_lzwd.c \
mupdf-0.5/fitz/filt_predict.c \
mupdf-0.5/fitz/node_toxml.c \
mupdf-0.5/fitz/node_misc1.c \
mupdf-0.5/fitz/node_misc2.c \
mupdf-0.5/fitz/node_path.c \
mupdf-0.5/fitz/node_text.c \
mupdf-0.5/fitz/node_tree.c \
mupdf-0.5/fitz/res_colorspace.c \
mupdf-0.5/fitz/res_font.c \
mupdf-0.5/fitz/res_image.c \
mupdf-0.5/fitz/res_shade.c

libfitz_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib/mupdf/mupdf-0.5/fitz \
-I$(top_builddir)/src/lib/mupdf/mupdf-0.5/fitz \
-I$(top_srcdir)/src/lib/mupdf/mupdf-0.5/mupdf \
-I$(top_builddir)/src/lib/mupdf/mupdf-0.5/mupdf \
@FREETYPE_CFLAGS@
libfitz_la_CFLAGS = -std=c99
libfitz_la_LIBADD = @FREETYPE_LIBS@

libfitzdraw_la_SOURCES = \
mupdf-0.5/fitzdraw/glyphcache.c \
mupdf-0.5/fitzdraw/pixmap.c \
mupdf-0.5/fitzdraw/porterduff.c \
mupdf-0.5/fitzdraw/meshdraw.c \
mupdf-0.5/fitzdraw/imagedraw.c \
mupdf-0.5/fitzdraw/imageunpack.c \
mupdf-0.5/fitzdraw/imagescale.c \
mupdf-0.5/fitzdraw/pathscan.c \
mupdf-0.5/fitzdraw/pathfill.c \
mupdf-0.5/fitzdraw/pathstroke.c \
mupdf-0.5/fitzdraw/render.c \
mupdf-0.5/fitzdraw/blendmodes.c

libfitzdraw_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib/mupdf/mupdf-0.5/fitz \
-I$(top_builddir)/src/lib/mupdf/mupdf-0.5/fitz \
-I$(top_srcdir)/src/lib/mupdf/mupdf-0.5/mupdf \
-I$(top_builddir)/src/lib/mupdf/mupdf-0.5/mupdf
libfitzdraw_la_CFLAGS = -std=c99

libcmaps_la_SOURCES = \
cmap_tounicode.c \
cmap_cns.c \
cmap_gb.c \
cmap_japan.c \
cmap_korea.c

libcmaps_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib/mupdf/mupdf-0.5/fitz \
-I$(top_builddir)/src/lib/mupdf/mupdf-0.5/fitz \
-I$(top_srcdir)/src/lib/mupdf/mupdf-0.5/mupdf \
-I$(top_builddir)/src/lib/mupdf/mupdf-0.5/mupdf

cmap_tounicode_files = \
mupdf-0.5/cmaps/Adobe-CNS1-UCS2 \
mupdf-0.5/cmaps/Adobe-GB1-UCS2 \
mupdf-0.5/cmaps/Adobe-Japan1-UCS2 \
mupdf-0.5/cmaps/Adobe-Korea1-UCS2 

cmap_cns_files = \
mupdf-0.5/cmaps/Adobe-CNS1-0 \
mupdf-0.5/cmaps/Adobe-CNS1-1 \
mupdf-0.5/cmaps/Adobe-CNS1-2 \
mupdf-0.5/cmaps/Adobe-CNS1-3 \
mupdf-0.5/cmaps/Adobe-CNS1-4 \
mupdf-0.5/cmaps/Adobe-CNS1-5 \
mupdf-0.5/cmaps/Adobe-CNS1-6 \
mupdf-0.5/cmaps/B5-H \
mupdf-0.5/cmaps/B5-V \
mupdf-0.5/cmaps/B5pc-H \
mupdf-0.5/cmaps/B5pc-V \
mupdf-0.5/cmaps/CNS-EUC-H \
mupdf-0.5/cmaps/CNS-EUC-V \
mupdf-0.5/cmaps/CNS1-H \
mupdf-0.5/cmaps/CNS1-V \
mupdf-0.5/cmaps/CNS2-H \
mupdf-0.5/cmaps/CNS2-V \
mupdf-0.5/cmaps/ETen-B5-H \
mupdf-0.5/cmaps/ETen-B5-V \
mupdf-0.5/cmaps/ETenms-B5-H \
mupdf-0.5/cmaps/ETenms-B5-V \
mupdf-0.5/cmaps/ETHK-B5-H \
mupdf-0.5/cmaps/ETHK-B5-V \
mupdf-0.5/cmaps/HKdla-B5-H \
mupdf-0.5/cmaps/HKdla-B5-V \
mupdf-0.5/cmaps/HKdlb-B5-H \
mupdf-0.5/cmaps/HKdlb-B5-V \
mupdf-0.5/cmaps/HKgccs-B5-H \
mupdf-0.5/cmaps/HKgccs-B5-V \
mupdf-0.5/cmaps/HKm314-B5-H \
mupdf-0.5/cmaps/HKm314-B5-V \
mupdf-0.5/cmaps/HKm471-B5-H \
mupdf-0.5/cmaps/HKm471-B5-V \
mupdf-0.5/cmaps/HKscs-B5-H \
mupdf-0.5/cmaps/HKscs-B5-V \
mupdf-0.5/cmaps/UniCNS-UCS2-H \
mupdf-0.5/cmaps/UniCNS-UCS2-V \
mupdf-0.5/cmaps/UniCNS-UTF16-H \
mupdf-0.5/cmaps/UniCNS-UTF16-V

cmap_gb_files = \
mupdf-0.5/cmaps/Adobe-GB1-0 \
mupdf-0.5/cmaps/Adobe-GB1-1 \
mupdf-0.5/cmaps/Adobe-GB1-2 \
mupdf-0.5/cmaps/Adobe-GB1-3 \
mupdf-0.5/cmaps/Adobe-GB1-4 \
mupdf-0.5/cmaps/Adobe-GB1-5 \
mupdf-0.5/cmaps/GB-EUC-H \
mupdf-0.5/cmaps/GB-EUC-V \
mupdf-0.5/cmaps/GB-H \
mupdf-0.5/cmaps/GB-V \
mupdf-0.5/cmaps/GBK-EUC-H \
mupdf-0.5/cmaps/GBK-EUC-V \
mupdf-0.5/cmaps/GBK2K-H \
mupdf-0.5/cmaps/GBK2K-V \
mupdf-0.5/cmaps/GBKp-EUC-H \
mupdf-0.5/cmaps/GBKp-EUC-V \
mupdf-0.5/cmaps/GBpc-EUC-H \
mupdf-0.5/cmaps/GBpc-EUC-V \
mupdf-0.5/cmaps/GBT-EUC-H \
mupdf-0.5/cmaps/GBT-EUC-V \
mupdf-0.5/cmaps/GBT-H \
mupdf-0.5/cmaps/GBT-V \
mupdf-0.5/cmaps/GBTpc-EUC-H \
mupdf-0.5/cmaps/GBTpc-EUC-V \
mupdf-0.5/cmaps/UniGB-UCS2-H \
mupdf-0.5/cmaps/UniGB-UCS2-V \
mupdf-0.5/cmaps/UniGB-UTF16-H \
mupdf-0.5/cmaps/UniGB-UTF16-V

cmap_japan_files = \
mupdf-0.5/cmaps/78-EUC-H \
mupdf-0.5/cmaps/78-EUC-V \
mupdf-0.5/cmaps/78-H \
mupdf-0.5/cmaps/78-RKSJ-H \
mupdf-0.5/cmaps/78-RKSJ-V \
mupdf-0.5/cmaps/78-V \
mupdf-0.5/cmaps/78ms-RKSJ-H \
mupdf-0.5/cmaps/78ms-RKSJ-V \
mupdf-0.5/cmaps/83pv-RKSJ-H \
mupdf-0.5/cmaps/90ms-RKSJ-H \
mupdf-0.5/cmaps/90ms-RKSJ-V \
mupdf-0.5/cmaps/90msp-RKSJ-H \
mupdf-0.5/cmaps/90msp-RKSJ-V \
mupdf-0.5/cmaps/90pv-RKSJ-H \
mupdf-0.5/cmaps/90pv-RKSJ-V \
mupdf-0.5/cmaps/Add-H \
mupdf-0.5/cmaps/Add-RKSJ-H \
mupdf-0.5/cmaps/Add-RKSJ-V \
mupdf-0.5/cmaps/Add-V \
mupdf-0.5/cmaps/Adobe-Japan1-0 \
mupdf-0.5/cmaps/Adobe-Japan1-1 \
mupdf-0.5/cmaps/Adobe-Japan1-2 \
mupdf-0.5/cmaps/Adobe-Japan1-3 \
mupdf-0.5/cmaps/Adobe-Japan1-4 \
mupdf-0.5/cmaps/Adobe-Japan1-5 \
mupdf-0.5/cmaps/Adobe-Japan1-6 \
mupdf-0.5/cmaps/EUC-H \
mupdf-0.5/cmaps/EUC-V \
mupdf-0.5/cmaps/Ext-H \
mupdf-0.5/cmaps/Ext-RKSJ-H \
mupdf-0.5/cmaps/Ext-RKSJ-V \
mupdf-0.5/cmaps/Ext-V \
mupdf-0.5/cmaps/H \
mupdf-0.5/cmaps/Hankaku \
mupdf-0.5/cmaps/Hiragana \
mupdf-0.5/cmaps/Katakana \
mupdf-0.5/cmaps/NWP-H \
mupdf-0.5/cmaps/NWP-V \
mupdf-0.5/cmaps/RKSJ-H \
mupdf-0.5/cmaps/RKSJ-V \
mupdf-0.5/cmaps/Roman \
mupdf-0.5/cmaps/UniJIS-UCS2-H \
mupdf-0.5/cmaps/UniJIS-UCS2-HW-H \
mupdf-0.5/cmaps/UniJIS-UCS2-HW-V \
mupdf-0.5/cmaps/UniJIS-UCS2-V \
mupdf-0.5/cmaps/UniJISPro-UCS2-HW-V \
mupdf-0.5/cmaps/UniJISPro-UCS2-V \
mupdf-0.5/cmaps/V \
mupdf-0.5/cmaps/WP-Symbol \
mupdf-0.5/cmaps/Adobe-Japan2-0 \
mupdf-0.5/cmaps/Hojo-EUC-H \
mupdf-0.5/cmaps/Hojo-EUC-V \
mupdf-0.5/cmaps/Hojo-H \
mupdf-0.5/cmaps/Hojo-V \
mupdf-0.5/cmaps/UniHojo-UCS2-H \
mupdf-0.5/cmaps/UniHojo-UCS2-V \
mupdf-0.5/cmaps/UniHojo-UTF16-H \
mupdf-0.5/cmaps/UniHojo-UTF16-V \
mupdf-0.5/cmaps/UniJIS-UTF16-H \
mupdf-0.5/cmaps/UniJIS-UTF16-V

cmap_korea_files = \
mupdf-0.5/cmaps/Adobe-Korea1-0 \
mupdf-0.5/cmaps/Adobe-Korea1-1 \
mupdf-0.5/cmaps/Adobe-Korea1-2 \
mupdf-0.5/cmaps/KSC-EUC-H \
mupdf-0.5/cmaps/KSC-EUC-V \
mupdf-0.5/cmaps/KSC-H \
mupdf-0.5/cmaps/KSC-Johab-H \
mupdf-0.5/cmaps/KSC-Johab-V \
mupdf-0.5/cmaps/KSC-V \
mupdf-0.5/cmaps/KSCms-UHC-H \
mupdf-0.5/cmaps/KSCms-UHC-HW-H \
mupdf-0.5/cmaps/KSCms-UHC-HW-V \
mupdf-0.5/cmaps/KSCms-UHC-V \
mupdf-0.5/cmaps/KSCpc-EUC-H \
mupdf-0.5/cmaps/KSCpc-EUC-V \
mupdf-0.5/cmaps/UniKS-UCS2-H \
mupdf-0.5/cmaps/UniKS-UCS2-V \
mupdf-0.5/cmaps/UniKS-UTF16-H \
mupdf-0.5/cmaps/UniKS-UTF16-V

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
font_misc.c \
font_mono.c \
font_serif.c \
font_sans.c

if BUILD_MUPDF_CJK_FONTS

libfonts_la_SOURCES += font_cjk.c

endif

font_misc_files = \
mupdf-0.5/fonts/Dingbats.cff \
mupdf-0.5/fonts/StandardSymL.cff \
mupdf-0.5/fonts/URWChanceryL-MediItal.cff

font_mono_files = \
mupdf-0.5/fonts/NimbusMonL-Regu.cff \
mupdf-0.5/fonts/NimbusMonL-ReguObli.cff \
mupdf-0.5/fonts/NimbusMonL-Bold.cff \
mupdf-0.5/fonts/NimbusMonL-BoldObli.cff

font_serif_files = \
mupdf-0.5/fonts/NimbusRomNo9L-Regu.cff \
mupdf-0.5/fonts/NimbusRomNo9L-ReguItal.cff \
mupdf-0.5/fonts/NimbusRomNo9L-Medi.cff \
mupdf-0.5/fonts/NimbusRomNo9L-MediItal.cff

font_sans_files = \
mupdf-0.5/fonts/NimbusSanL-Bold.cff \
mupdf-0.5/fonts/NimbusSanL-BoldItal.cff \
mupdf-0.5/fonts/NimbusSanL-Regu.cff \
mupdf-0.5/fonts/NimbusSanL-ReguItal.cff

font_cjk_files = \
mupdf-0.5/fonts/droid/DroidSansFallback.ttf

font_misc.c: fontdump $(font_misc_files)
	./fontdump $@ $(font_misc_files)

font_mono.c: fontdump $(font_mono_files)
	./fontdump $@ $(font_mono_files)

font_serif.c: fontdump $(font_serif_files)
	./fontdump $@ $(font_serif_files)

font_sans.c: fontdump $(font_sans_files)
	./fontdump $@ $(font_sans_files)

font_cjk.c: fontdump $(font_cjk_files)
	./fontdump $@ $(font_cjk_files)



libmupdf_la_SOURCES = \
mupdf-0.5/mupdf/pdf_crypt.c \
mupdf-0.5/mupdf/pdf_debug.c \
mupdf-0.5/mupdf/pdf_lex.c \
mupdf-0.5/mupdf/pdf_nametree.c \
mupdf-0.5/mupdf/pdf_open.c \
mupdf-0.5/mupdf/pdf_parse.c \
mupdf-0.5/mupdf/pdf_repair.c \
mupdf-0.5/mupdf/pdf_stream.c \
mupdf-0.5/mupdf/pdf_xref.c \
mupdf-0.5/mupdf/pdf_annot.c \
mupdf-0.5/mupdf/pdf_outline.c \
mupdf-0.5/mupdf/pdf_cmap.c \
mupdf-0.5/mupdf/pdf_cmap_parse.c \
mupdf-0.5/mupdf/pdf_cmap_load.c \
mupdf-0.5/mupdf/pdf_cmap_table.c \
mupdf-0.5/mupdf/pdf_fontagl.c \
mupdf-0.5/mupdf/pdf_fontenc.c \
mupdf-0.5/mupdf/pdf_unicode.c \
mupdf-0.5/mupdf/pdf_font.c \
mupdf-0.5/mupdf/pdf_type3.c \
mupdf-0.5/mupdf/pdf_fontmtx.c \
mupdf-0.5/mupdf/pdf_fontfile.c \
mupdf-0.5/mupdf/pdf_function.c \
mupdf-0.5/mupdf/pdf_colorspace1.c \
mupdf-0.5/mupdf/pdf_colorspace2.c \
mupdf-0.5/mupdf/pdf_image.c \
mupdf-0.5/mupdf/pdf_pattern.c \
mupdf-0.5/mupdf/pdf_shade.c \
mupdf-0.5/mupdf/pdf_shade1.c \
mupdf-0.5/mupdf/pdf_shade4.c \
mupdf-0.5/mupdf/pdf_xobject.c \
mupdf-0.5/mupdf/pdf_build.c \
mupdf-0.5/mupdf/pdf_interpret.c \
mupdf-0.5/mupdf/pdf_page.c \
mupdf-0.5/mupdf/pdf_pagetree.c \
mupdf-0.5/mupdf/pdf_store.c

libmupdf_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib/mupdf/mupdf-0.5/fitz \
-I$(top_builddir)/src/lib/mupdf/mupdf-0.5/fitz \
-I$(top_srcdir)/src/lib/mupdf/mupdf-0.5/mupdf \
-I$(top_builddir)/src/lib/mupdf/mupdf-0.5/mupdf \
@BUILD_MUPDF_CJK_FONTS@ \
@FREETYPE_CFLAGS@
libmupdf_la_LIBADD = @FREETYPE_LIBS@

libepdf_mupdf_la_SOURCES = \
epdf_mupdf_document.c \
epdf_mupdf_fontinfo.c \
epdf_mupdf_index.c \
epdf_mupdf_main.c \
epdf_mupdf_page.c

libepdf_mupdf_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/lib/mupdf \
-I$(top_builddir)/src/lib/mupdf \
-I$(top_srcdir)/src/lib/mupdf/mupdf-0.5/fitz \
-I$(top_builddir)/src/lib/mupdf/mupdf-0.5/fitz \
-I$(top_srcdir)/src/lib/mupdf/mupdf-0.5/mupdf \
-I$(top_builddir)/src/lib/mupdf/mupdf-0.5/mupdf \
@EPDF_CFLAGS@

libepdf_mupdf_la_LIBADD = @EPDF_LIBS@
libepdf_mupdf_la_LDFLAGS = -no-undefined @lt_enable_auto_import@

clean-local:
	-rm -f cmap_*.c font_*.c

EXTRA_DIST = epdf_mupdf_private.h
