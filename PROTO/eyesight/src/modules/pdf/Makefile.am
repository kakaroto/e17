MAINTAINERCLEANFILES = Makefile.in


if EYESIGHT_BUILD_PDF
if !EYESIGHT_STATIC_BUILD_PDF

includesdir = $(includedir)/eyesight
includes_HEADERS = Eyesight_Module_Pdf.h

pkgdir = $(libdir)/eyesight


pkg_LTLIBRARIES = pdf.la

if HAVE_PDF_BACKEND_MUPDF

noinst_PROGRAMS = cmapdump fontdump
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

cmapdump_SOURCES = mupdf-0.9/scripts/cmapdump.c
cmapdump_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.9/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.9/fitz \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.9/pdf \
-I$(top_builddir)/src/modules/pdf/mupdf-0.9/pdf

fontdump_SOURCES = mupdf-0.9/scripts/fontdump.c

noinst_LTLIBRARIES = libfitz.la libmupdf.la libmuxps.la

libfitz_la_SOURCES = \
mupdf-0.9/fitz/base_error.c \
mupdf-0.9/fitz/base_geometry.c \
mupdf-0.9/fitz/base_getopt.c \
mupdf-0.9/fitz/base_hash.c \
mupdf-0.9/fitz/base_memory.c \
mupdf-0.9/fitz/base_object.c \
mupdf-0.9/fitz/base_string.c \
mupdf-0.9/fitz/base_time.c \
mupdf-0.9/fitz/crypt_aes.c \
mupdf-0.9/fitz/crypt_arc4.c \
mupdf-0.9/fitz/crypt_md5.c \
mupdf-0.9/fitz/crypt_sha2.c \
mupdf-0.9/fitz/dev_bbox.c \
mupdf-0.9/fitz/dev_list.c \
mupdf-0.9/fitz/dev_null.c \
mupdf-0.9/fitz/dev_text.c \
mupdf-0.9/fitz/dev_trace.c \
mupdf-0.9/fitz/filt_basic.c \
mupdf-0.9/fitz/filt_dctd.c \
mupdf-0.9/fitz/filt_faxd.c \
mupdf-0.9/fitz/filt_flate.c \
mupdf-0.9/fitz/filt_jbig2d.c \
mupdf-0.9/fitz/filt_jpxd.c \
mupdf-0.9/fitz/filt_lzwd.c \
mupdf-0.9/fitz/filt_predict.c \
mupdf-0.9/fitz/obj_print.c \
mupdf-0.9/fitz/res_bitmap.c \
mupdf-0.9/fitz/res_colorspace.c \
mupdf-0.9/fitz/res_font.c \
mupdf-0.9/fitz/res_halftone.c \
mupdf-0.9/fitz/res_path.c \
mupdf-0.9/fitz/res_pixmap.c \
mupdf-0.9/fitz/res_shade.c \
mupdf-0.9/fitz/res_text.c \
mupdf-0.9/fitz/stm_buffer.c \
mupdf-0.9/fitz/stm_open.c \
mupdf-0.9/fitz/stm_read.c \
mupdf-0.9/draw/arch_arm.c \
mupdf-0.9/draw/arch_port.c \
mupdf-0.9/draw/draw_affine.c \
mupdf-0.9/draw/draw_blend.c \
mupdf-0.9/draw/draw_device.c \
mupdf-0.9/draw/draw_edge.c \
mupdf-0.9/draw/draw_glyph.c \
mupdf-0.9/draw/draw_mesh.c \
mupdf-0.9/draw/draw_paint.c \
mupdf-0.9/draw/draw_path.c \
mupdf-0.9/draw/draw_scale.c \
mupdf-0.9/draw/draw_unpack.c


libfitz_la_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.9/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.9/fitz \
@MUPDF_CFLAGS@
libfitz_la_LIBADD = @MUPDF_LIBS@

cmap_cns_files = \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/Adobe-CNS1-0 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/Adobe-CNS1-1 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/Adobe-CNS1-2 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/Adobe-CNS1-3 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/Adobe-CNS1-4 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/Adobe-CNS1-5 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/Adobe-CNS1-6 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/Adobe-CNS1-UCS2 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/B5pc-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/B5pc-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/CNS1-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/CNS1-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/CNS2-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/CNS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/CNS-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/CNS-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/ETen-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/ETen-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/ETenms-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/ETenms-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/ETHK-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/ETHK-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/HKdla-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/HKdla-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/HKdlb-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/HKdlb-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/HKgccs-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/HKgccs-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/HKm314-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/HKm314-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/HKm471-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/HKm471-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/HKscs-B5-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/HKscs-B5-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/UniCNS-UCS2-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/UniCNS-UCS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/UniCNS-UTF16-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/cns/UniCNS-UTF16-V

cmap_gb_files = \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/Adobe-GB1-0 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/Adobe-GB1-1 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/Adobe-GB1-2 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/Adobe-GB1-3 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/Adobe-GB1-4 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/Adobe-GB1-5 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/Adobe-GB1-UCS2 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GB-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GB-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GB-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBK2K-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBK2K-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBK-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBK-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBKp-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBKp-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBpc-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBpc-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBT-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBT-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBT-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBTpc-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBTpc-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GBT-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/GB-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/UniGB-UCS2-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/UniGB-UCS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/UniGB-UTF16-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/gb/UniGB-UTF16-V

cmap_japan_files = \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/78-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/78-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/78-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/78ms-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/78ms-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/78-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/78-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/78-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/83pv-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/90msp-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/90msp-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/90ms-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/90ms-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/90pv-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/90pv-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Add-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Add-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Add-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Add-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Adobe-Japan1-0 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Adobe-Japan1-1 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Adobe-Japan1-2 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Adobe-Japan1-3 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Adobe-Japan1-4 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Adobe-Japan1-5 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Adobe-Japan1-6 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Adobe-Japan1-UCS2 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Adobe-Japan2-0 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Ext-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Ext-RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Ext-RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Ext-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Hankaku \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Hiragana \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Hojo-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Hojo-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Hojo-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Hojo-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Katakana \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/NWP-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/NWP-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/RKSJ-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/RKSJ-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/Roman \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/UniHojo-UCS2-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/UniHojo-UCS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/UniHojo-UTF16-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/UniHojo-UTF16-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/UniJISPro-UCS2-HW-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/UniJISPro-UCS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/UniJIS-UCS2-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/UniJIS-UCS2-HW-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/UniJIS-UCS2-HW-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/UniJIS-UCS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/UniJIS-UTF16-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/UniJIS-UTF16-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/japan/WP-Symbol

cmap_korea_files = \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/Adobe-Korea1-0 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/Adobe-Korea1-1 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/Adobe-Korea1-2 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/Adobe-Korea1-UCS2 \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/KSC-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/KSC-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/KSC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/KSC-Johab-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/KSC-Johab-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/KSCms-UHC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/KSCms-UHC-HW-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/KSCms-UHC-HW-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/KSCms-UHC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/KSCpc-EUC-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/KSCpc-EUC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/KSC-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/UniKS-UCS2-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/UniKS-UCS2-V \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/UniKS-UTF16-H \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/cmaps/korea/UniKS-UTF16-V

generated:
	@echo "  MKDIR  mupdf-0.9/generated/"
	@$(MKDIR_P) $(top_builddir)/src/modules/pdf/mupdf-0.9/generated/

cmap_cns.h: cmapdump generated $(cmap_cns_files)
	@echo "  GEN    $@"
	@./cmapdump $(top_builddir)/src/modules/pdf/mupdf-0.9/generated/$@ $(cmap_cns_files) > /dev/null 2>&1

cmap_gb.h: cmapdump generated $(cmap_gb_files)
	@echo "  GEN    $@"
	@./cmapdump $(top_builddir)/src/modules/pdf/mupdf-0.9/generated/$@ $(cmap_gb_files) > /dev/null 2>&1

cmap_japan.h: cmapdump generated $(cmap_japan_files)
	@echo "  GEN    $@"
	@./cmapdump $(top_builddir)/src/modules/pdf/mupdf-0.9/generated/$@ $(cmap_japan_files) > /dev/null 2>&1

cmap_korea.h: cmapdump generated $(cmap_korea_files)
	@echo "  GEN    $@"
	@./cmapdump $(top_builddir)/src/modules/pdf/mupdf-0.9/generated/$@ $(cmap_korea_files) > /dev/null 2>&1

font_base_files = \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/Dingbats.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/NimbusMonL-Bold.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/NimbusMonL-BoldObli.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/NimbusMonL-Regu.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/NimbusMonL-ReguObli.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/NimbusRomNo9L-Medi.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/NimbusRomNo9L-MediItal.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/NimbusRomNo9L-Regu.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/NimbusRomNo9L-ReguItal.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/NimbusSanL-Bold.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/NimbusSanL-BoldItal.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/NimbusSanL-Regu.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/NimbusSanL-ReguItal.cff \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/StandardSymL.cff

font_droid_files = \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/droid/DroidSans.ttf \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/droid/DroidSansMono.ttf

font_cjk_files = \
$(top_srcdir)/src/modules/pdf/mupdf-0.9/fonts/droid/DroidSansFallback.ttf

font_base14.h: fontdump generated $(font_base_files)
	@echo "  GEN    $@"
	@./fontdump $(top_builddir)/src/modules/pdf/mupdf-0.9/generated/$@ $(font_base_files) > /dev/null 2>&1

font_droid.h: fontdump generated $(font_droid_files)
	@echo "  GEN    $@"
	@./fontdump $(top_builddir)/src/modules/pdf/mupdf-0.9/generated/$@ $(font_droid_files) > /dev/null 2>&1

font_cjk.h: fontdump generated $(font_cjk_files)
	@echo "  GEN    $@"
	@./fontdump $(top_builddir)/src/modules/pdf/mupdf-0.9/generated/$@ $(font_cjk_files) > /dev/null 2>&1

libmupdf_la_SOURCES = \
mupdf-0.9/pdf/pdf_annot.c \
mupdf-0.9/pdf/pdf_cmap.c \
mupdf-0.9/pdf/pdf_cmap_load.c \
mupdf-0.9/pdf/pdf_cmap_parse.c \
mupdf-0.9/pdf/pdf_cmap_table.c \
mupdf-0.9/pdf/pdf_colorspace.c \
mupdf-0.9/pdf/pdf_crypt.c \
mupdf-0.9/pdf/pdf_encoding.c \
mupdf-0.9/pdf/pdf_font.c \
mupdf-0.9/pdf/pdf_fontfile.c \
mupdf-0.9/pdf/pdf_function.c \
mupdf-0.9/pdf/pdf_image.c \
mupdf-0.9/pdf/pdf_interpret.c \
mupdf-0.9/pdf/pdf_lex.c \
mupdf-0.9/pdf/pdf_metrics.c \
mupdf-0.9/pdf/pdf_nametree.c \
mupdf-0.9/pdf/pdf_outline.c \
mupdf-0.9/pdf/pdf_page.c \
mupdf-0.9/pdf/pdf_parse.c \
mupdf-0.9/pdf/pdf_pattern.c \
mupdf-0.9/pdf/pdf_repair.c \
mupdf-0.9/pdf/pdf_shade.c \
mupdf-0.9/pdf/pdf_store.c \
mupdf-0.9/pdf/pdf_stream.c \
mupdf-0.9/pdf/pdf_type3.c \
mupdf-0.9/pdf/pdf_unicode.c \
mupdf-0.9/pdf/pdf_xobject.c \
mupdf-0.9/pdf/pdf_xref.c

libmupdf_la_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.9/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.9/fitz \
@MUPDF_CFLAGS@
libmupdf_la_LIBADD = @MUPDF_LIBS@

libmuxps_la_SOURCES = \
mupdf-0.9/xps/xps_common.c \
mupdf-0.9/xps/xps_doc.c \
mupdf-0.9/xps/xps_glyphs.c \
mupdf-0.9/xps/xps_gradient.c \
mupdf-0.9/xps/xps_image.c \
mupdf-0.9/xps/xps_jpeg.c \
mupdf-0.9/xps/xps_path.c \
mupdf-0.9/xps/xps_png.c \
mupdf-0.9/xps/xps_resource.c \
mupdf-0.9/xps/xps_tiff.c \
mupdf-0.9/xps/xps_tile.c \
mupdf-0.9/xps/xps_util.c \
mupdf-0.9/xps/xps_xml.c \
mupdf-0.9/xps/xps_zip.c

libmuxps_la_CPPFLAGS = \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.9/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.9/fitz \
@MUPDF_CFLAGS@
libmuxps_la_LIBADD = @MUPDF_LIBS@

pdf_la_SOURCES = \
eyesight_pdf_mupdf.c \
eyesight_pdf_mupdf.h
pdf_la_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.9/fitz \
-I$(top_builddir)/src/modules/pdf/mupdf-0.9/fitz \
-I$(top_srcdir)/src/modules/pdf/mupdf-0.9/mupdf \
-I$(top_builddir)/src/modules/pdf/mupdf-0.9/mupdf \
@EFL_EYESIGHT_BUILD@ \
@EYESIGHT_CFLAGS@
pdf_la_LIBADD  = \
@EYESIGHT_LIBS@ \
$(top_builddir)/src/lib/libeyesight.la \
libfitz.la \
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
	-rm -rf mupdf-0.9/generated

EXTRA_DIST = mupdf-0.9
