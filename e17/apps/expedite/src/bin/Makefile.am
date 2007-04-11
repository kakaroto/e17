MAINTAINERCLEANFILES = Makefile.in
INCLUDES = -I$(includedir) \
	   -I$(top_srcdir) \
	   -I$(top_srcdir)/src/bin \
	   @EVAS_CFLAGS@ @x_cflags@ \
	   -DPACKAGE_BIN_DIR=\"@PACKAGE_BIN_DIR@\" \
	   -DPACKAGE_LIB_DIR=\"@PACKAGE_LIB_DIR@\" \
	   -DPACKAGE_DATA_DIR=\"@PACKAGE_DATA_DIR@\"

bin_PROGRAMS = expedite

expedite_SOURCES = \
main.c main.h \
engine_software_x11.c engine_software_x11.h \
engine_gl_x11.c engine_gl_x11.h \
engine_xrender_x11.c engine_xrender_x11.h \
ui.c ui.h \
about.c about.h \
tests.h \
image_blend_unscaled.c \
image_blend_solid_unscaled.c \
image_blend_nearest_scaled.c \
image_blend_nearest_solid_scaled.c \
image_blend_smooth_scaled.c \
image_blend_smooth_solid_scaled.c \
image_blend_border.c \
image_blend_solid_border.c \
image_blend_border_recolor.c \
image_quality_scale.c \
image_data_argb.c \
image_data_argb_alpha.c \
image_data_ycbcr601pl.c \
image_crossfade.c \
text_basic.c \
text_styles.c \
text_styles_different_strings.c \
text_change.c \
textblock_basic.c \
textblock_intl.c \
rect_blend.c \
rect_solid.c \
rect_blend_few.c \
rect_solid_few.c

expedite_LDFLAGS = @EVAS_LIBS@  @x_libs@ -lm
