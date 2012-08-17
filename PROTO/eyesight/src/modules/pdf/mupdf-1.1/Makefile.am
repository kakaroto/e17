MAINTAINERCLEANFILES = Makefile.in

noinst_PROGRAMS = cmapdump$(EXEEXT) fontdump$(EXEEXT)

BUILT_SOURCES = \
cmapdump$(EXEEXT) \
cmap_cns.h \
cmap_gb.h \
cmap_japan.h \
cmap_korea.h \
fontdump$(EXEEXT) \
font_base14.h \
font_droid.h \
font_cjk.h

cmapdump_SOURCES = scripts/cmapdump.c
cmapdump_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-1.1/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-1.1/fitz \
-I$(top_srcdir)/src/modules/pdf/mupdf-1.1/pdf \
-I$(top_builddir)/src/modules/pdf/mupdf-1.1/pdf

fontdump_SOURCES = scripts/fontdump.c

noinst_LTLIBRARIES = libfitz.la

libfitz_la_SOURCES = \
fitz/base_context.c \
fitz/base_error.c \
fitz/base_geometry.c \
fitz/base_getopt.c \
fitz/base_hash.c \
fitz/base_memory.c \
fitz/base_string.c \
fitz/base_time.c \
fitz/crypt_aes.c \
fitz/crypt_arc4.c \
fitz/crypt_md5.c \
fitz/crypt_sha2.c \
fitz/dev_bbox.c \
fitz/dev_list.c \
fitz/dev_null.c \
fitz/dev_text.c \
fitz/dev_trace.c \
fitz/doc_document.c \
fitz/doc_link.c \
fitz/doc_outline.c \
fitz/filt_basic.c \
fitz/filt_dctd.c \
fitz/filt_faxd.c \
fitz/filt_flate.c \
fitz/filt_jbig2d.c \
fitz/filt_lzwd.c \
fitz/filt_predict.c \
fitz/fitz-internal.h \
fitz/fitz.h \
fitz/image_jpeg.c \
fitz/image_jpx.c \
fitz/image_md5.c \
fitz/image_png.c \
fitz/image_save.c \
fitz/image_tiff.c \
fitz/memento.c \
fitz/memento.h \
fitz/res_bitmap.c \
fitz/res_colorspace.c \
fitz/res_font.c \
fitz/res_halftone.c \
fitz/res_path.c \
fitz/res_pixmap.c \
fitz/res_shade.c \
fitz/res_store.c \
fitz/res_text.c \
fitz/stm_buffer.c \
fitz/stm_open.c \
fitz/stm_read.c \
draw/draw_affine.c \
draw/draw_blend.c \
draw/draw_device.c \
draw/draw_edge.c \
draw/draw_glyph.c \
draw/draw_mesh.c \
draw/draw_paint.c \
draw/draw_path.c \
draw/draw_scale.c \
draw/draw_unpack.c \
pdf/data_encodings.h \
pdf/data_glyphlist.h \
pdf/mupdf-internal.h \
pdf/mupdf.h \
pdf/pdf_annot.c \
pdf/pdf_cmap.c \
pdf/pdf_cmap_load.c \
pdf/pdf_cmap_parse.c \
pdf/pdf_cmap_table.c \
pdf/pdf_colorspace.c \
pdf/pdf_crypt.c \
pdf/pdf_encoding.c \
pdf/pdf_font.c \
pdf/pdf_fontfile.c \
pdf/pdf_function.c \
pdf/pdf_image.c \
pdf/pdf_interpret.c \
pdf/pdf_lex.c \
pdf/pdf_metrics.c \
pdf/pdf_nametree.c \
pdf/pdf_object.c \
pdf/pdf_outline.c \
pdf/pdf_page.c \
pdf/pdf_parse.c \
pdf/pdf_pattern.c \
pdf/pdf_repair.c \
pdf/pdf_shade.c \
pdf/pdf_store.c \
pdf/pdf_stream.c \
pdf/pdf_type3.c \
pdf/pdf_unicode.c \
pdf/pdf_write.c \
pdf/pdf_xobject.c \
pdf/pdf_xref.c \
pdf/pdf_xref_aux.c \
xps/muxps-internal.h \
xps/muxps.h \
xps/xps_common.c \
xps/xps_doc.c \
xps/xps_glyphs.c \
xps/xps_gradient.c \
xps/xps_image.c \
xps/xps_outline.c \
xps/xps_path.c \
xps/xps_resource.c \
xps/xps_tile.c \
xps/xps_util.c \
xps/xps_xml.c \
xps/xps_zip.c \
cbz/mucbz.c \
cbz/mucbz.h

libfitz_la_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-1.1/cbz \
-I$(top_builddir)/src/modules/pdf/mupdf-1.1/cbz \
-I$(top_srcdir)/src/modules/pdf/mupdf-1.1/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-1.1/fitz \
-I$(top_srcdir)/src/modules/pdf/mupdf-1.1/pdf \
-I$(top_builddir)/src/modules/pdf/mupdf-1.1/pdf \
-I$(top_srcdir)/src/modules/pdf/mupdf-1.1/xps \
-I$(top_builddir)/src/modules/pdf/mupdf-1.1/xps \
@MUPDF_CFLAGS@
libfitz_la_LIBADD = @MUPDF_LIBS@

cmap_cns_files = \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/Adobe-CNS1-0 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/Adobe-CNS1-1 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/Adobe-CNS1-2 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/Adobe-CNS1-3 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/Adobe-CNS1-4 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/Adobe-CNS1-5 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/Adobe-CNS1-6 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/Adobe-CNS1-UCS2 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/B5pc-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/B5pc-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/CNS1-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/CNS1-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/CNS2-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/CNS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/CNS-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/CNS-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/ETen-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/ETen-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/ETenms-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/ETenms-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/ETHK-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/ETHK-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/HKdla-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/HKdla-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/HKdlb-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/HKdlb-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/HKgccs-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/HKgccs-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/HKm314-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/HKm314-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/HKm471-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/HKm471-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/HKscs-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/HKscs-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/UniCNS-UCS2-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/UniCNS-UCS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/UniCNS-UTF16-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/cns/UniCNS-UTF16-V

cmap_gb_files = \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/Adobe-GB1-0 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/Adobe-GB1-1 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/Adobe-GB1-2 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/Adobe-GB1-3 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/Adobe-GB1-4 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/Adobe-GB1-5 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/Adobe-GB1-UCS2 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GB-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GB-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GB-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBK2K-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBK2K-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBK-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBK-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBKp-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBKp-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBpc-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBpc-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBT-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBT-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBT-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBTpc-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBTpc-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GBT-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/GB-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/UniGB-UCS2-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/UniGB-UCS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/UniGB-UTF16-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/gb/UniGB-UTF16-V

cmap_japan_files = \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/78-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/78-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/78-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/78ms-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/78ms-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/78-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/78-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/78-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/83pv-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/90msp-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/90msp-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/90ms-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/90ms-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/90pv-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/90pv-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Add-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Add-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Add-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Add-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Adobe-Japan1-0 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Adobe-Japan1-1 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Adobe-Japan1-2 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Adobe-Japan1-3 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Adobe-Japan1-4 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Adobe-Japan1-5 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Adobe-Japan1-6 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Adobe-Japan1-UCS2 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Adobe-Japan2-0 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Ext-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Ext-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Ext-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Ext-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Hankaku \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Hiragana \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Hojo-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Hojo-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Hojo-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Hojo-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Katakana \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/NWP-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/NWP-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/Roman \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/UniHojo-UCS2-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/UniHojo-UCS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/UniHojo-UTF16-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/UniHojo-UTF16-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/UniJISPro-UCS2-HW-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/UniJISPro-UCS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/UniJIS-UCS2-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/UniJIS-UCS2-HW-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/UniJIS-UCS2-HW-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/UniJIS-UCS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/UniJIS-UTF16-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/UniJIS-UTF16-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/japan/WP-Symbol

cmap_korea_files = \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/Adobe-Korea1-0 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/Adobe-Korea1-1 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/Adobe-Korea1-2 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/Adobe-Korea1-UCS2 \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/KSC-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/KSC-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/KSC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/KSC-Johab-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/KSC-Johab-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/KSCms-UHC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/KSCms-UHC-HW-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/KSCms-UHC-HW-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/KSCms-UHC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/KSCpc-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/KSCpc-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/KSC-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/UniKS-UCS2-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/UniKS-UCS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/UniKS-UTF16-H \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/cmaps/korea/UniKS-UTF16-V

generated:
	@echo "  MKDIR  generated/"
	@$(MKDIR_P) generated/

cmap_cns.h: cmapdump$(EXEEXT) generated $(cmap_cns_files)
	@echo "  GEN    $@"
	@./cmapdump$(EXEEXT) generated/$@ $(cmap_cns_files) > /dev/null 2>&1

cmap_gb.h: cmapdump$(EXEEXT) generated $(cmap_gb_files)
	@echo "  GEN    $@"
	@./cmapdump$(EXEEXT) generated/$@ $(cmap_gb_files) > /dev/null 2>&1

cmap_japan.h: cmapdump$(EXEEXT) generated $(cmap_japan_files)
	@echo "  GEN    $@"
	@./cmapdump$(EXEEXT) generated/$@ $(cmap_japan_files) > /dev/null 2>&1

cmap_korea.h: cmapdump$(EXEEXT) generated $(cmap_korea_files)
	@echo "  GEN    $@"
	@./cmapdump$(EXEEXT) generated/$@ $(cmap_korea_files) > /dev/null 2>&1

font_base_files = \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/Dingbats.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/NimbusMonL-Bold.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/NimbusMonL-BoldObli.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/NimbusMonL-Regu.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/NimbusMonL-ReguObli.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/NimbusRomNo9L-Medi.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/NimbusRomNo9L-MediItal.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/NimbusRomNo9L-Regu.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/NimbusRomNo9L-ReguItal.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/NimbusSanL-Bold.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/NimbusSanL-BoldItal.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/NimbusSanL-Regu.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/NimbusSanL-ReguItal.cff \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/StandardSymL.cff

font_droid_files = \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/droid/DroidSans.ttf \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/droid/DroidSansMono.ttf

font_cjk_files = \
$(top_srcdir)/src/modules/pdf/mupdf-1.1/fonts/droid/DroidSansFallback.ttf

font_base14.h: fontdump$(EXEEXT) generated $(font_base_files)
	@echo "  GEN    $@"
	@./fontdump$(EXEEXT) generated/$@ $(font_base_files) > /dev/null 2>&1

font_droid.h: fontdump$(EXEEXT) generated $(font_droid_files)
	@echo "  GEN    $@"
	@./fontdump$(EXEEXT) generated/$@ $(font_droid_files) > /dev/null 2>&1

font_cjk.h: fontdump$(EXEEXT) generated $(font_cjk_files)
	@echo "  GEN    $@"
	@./fontdump$(EXEEXT) generated/$@ $(font_cjk_files) > /dev/null 2>&1
