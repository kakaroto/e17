MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EINA_CFLAGS@ \
@EVAS_CFLAGS@ \
@x_cflags@ \
@SDL_CFLAGS@ \
@DIRECTFB_CFLAGS@ \
@XCB_CFLAGS@ \
@EVIL_CFLAGS@

bin_PROGRAMS = expedite
bin_SCRIPTS = expedite-cmp

expedite_SOURCES = \
main.c main.h \
ui.c ui.h \
about.c about.h \
tests.h \
widgets_file_icons.c \
widgets_file_icons_2.c \
widgets_file_icons_2_grouped.c \
widgets_file_icons_2_same.c \
widgets_file_icons_2_same_grouped.c \
widgets_file_icons_3.c \
widgets_file_icons_4.c \
widgets_list_1.c \
widgets_list_1_grouped.c \
widgets_list_2.c \
widgets_list_2_grouped.c \
widgets_list_3.c \
widgets_list_3_grouped.c \
widgets_list_4.c \
widgets_list_4_grouped.c \
image_blend_unscaled.c \
image_blend_solid_middle_unscaled.c \
image_blend_fade_unscaled.c \
image_blend_fade_pow2_unscaled.c \
image_blend_solid_unscaled.c \
image_blend_solid_fade_unscaled.c \
image_blend_solid_fade_pow2_unscaled.c \
image_blend_nearest_scaled.c \
image_blend_nearest_solid_scaled.c \
image_blend_smooth_scaled.c \
image_blend_smooth_solid_scaled.c \
image_blend_nearest_same_scaled.c \
image_blend_nearest_solid_same_scaled.c \
image_blend_smooth_same_scaled.c \
image_blend_smooth_solid_same_scaled.c \
image_blend_border.c \
image_blend_solid_middle_border.c \
image_blend_solid_border.c \
image_blend_border_recolor.c \
image_map_rotate.c \
image_map_solid_rotate.c \
image_map_nearest_rotate.c \
image_map_nearest_solid_rotate.c \
image_map_color_rotate.c \
image_map_color_solid_rotate.c \
image_map_color_nearest_rotate.c \
image_map_color_nearest_solid_rotate.c \
image_map_color_alpha_rotate.c \
image_map_color_alpha_solid_rotate.c \
image_map_color_alpha_nearest_rotate.c \
image_map_color_alpha_nearest_solid_rotate.c \
image_map_3d_1.c \
image_map_3d_2.c \
image_map_3d_3.c \
image_map_3d_4.c \
image_map_3d_5.c \
image_map_3d_6.c \
image_map_3d_flow.c \
image_quality_scale.c \
image_data_argb.c \
image_data_argb_alpha.c \
image_data_ycbcr601pl.c \
image_data_ycbcr601pl_wide_stride.c \
image_data_ycbcr601pl_map_solid_rotate.c \
image_data_ycbcr601pl_map_nearest_solid_rotate.c \
image_crossfade.c \
text_basic.c \
text_styles.c \
text_styles_different_strings.c \
text_change.c \
textblock_basic.c \
textblock_intl.c \
textblock_auto_align.c \
textblock_text_append.c \
rect_blend.c \
rect_blend_pow2.c \
rect_solid.c \
rect_blend_few.c \
rect_blend_pow2_few.c \
rect_solid_few.c \
image_blend_occlude1_few.c \
image_blend_occlude2_few.c \
image_blend_occlude3_few.c \
image_blend_occlude1.c \
image_blend_occlude2.c \
image_blend_occlude3.c \
image_blend_occlude1_many.c \
image_blend_occlude2_many.c \
image_blend_occlude3_many.c \
image_blend_occlude1_very_many.c \
image_blend_occlude2_very_many.c \
image_blend_occlude3_very_many.c \
poly_blend.c \
proxy_image.c \
proxy_text_fixed.c \
proxy_text_random.c

#image_mask.c \
#image_mask_2.c \
#image_mask_3.c \
#image_mask_4.c \
#image_mask_5.c \
#image_mask_6.c \
#image_mask_7.c \
#image_mask_8.c \
#image_mask_9.c \
#image_mask_10.c \
#image_mask_11.c \
#image_mask_12.c \
#image_mask_13.c \
#image_mask_14.c \
#image_mask_15.c \
#filter_object_colors.c \
#filter_object_colors_solid.c \
#filter_object_sepia.c \
#filter_object_sepia_solid.c \
#filter_object_greyscale.c \
#filter_object_greyscale_solid.c \
#filter_object_invert.c \
#filter_object_invert_solid.c \
#filter_object_brightness.c \
#filter_object_brightness_solid.c \
#filter_object_blur.c \
#filter_object_blur_solid.c

expedite_CFLAGS = @WIN32_CFLAGS@
expedite_CXXFLAGS = @EXPEDITE_CXXFLAGS@
expedite_LDADD = @SDL_LIBS@ @x_libs@ @gdi_libs@ @ddraw_libs@ @ddraw_16_libs@ @d3d_libs@ @wince_16_libs@ @DIRECTFB_LIBS@ @XCB_LIBS@ @EVAS_LIBS@ @EINA_LIBS@ @EVIL_LIBS@ @gl_cocoa_libs@ -lm
expedite_LDFLAGS = @lt_enable_auto_import@

if BUILD_SOFTWARE_XLIB
expedite_SOURCES += \
engine_software_xlib.c engine_software_xlib.h
endif

if BUILD_XRENDER_X11
expedite_SOURCES += \
engine_xrender_x11.c engine_xrender_x11.h
endif

if BUILD_OPENGL_X11
expedite_SOURCES += \
engine_gl_x11.c engine_gl_x11.h
endif

if BUILD_SOFTWARE_XCB
expedite_SOURCES += \
engine_software_xcb.c engine_software_xcb.h
endif

if BUILD_XRENDER_XCB
expedite_SOURCES += \
engine_xrender_xcb.c engine_xrender_xcb.h
endif

if BUILD_SOFTWARE_GDI
expedite_SOURCES += \
engine_software_gdi.c engine_software_gdi.h
endif

if BUILD_SOFTWARE_DDRAW
expedite_SOURCES += \
engine_software_ddraw.cpp engine_software_ddraw.h
endif

if BUILD_DIRECT3D
expedite_SOURCES += \
engine_direct3d.cpp engine_direct3d.h
endif

if BUILD_SOFTWARE_SDL
expedite_SOURCES += \
engine_software_sdl.c engine_software_sdl.h
endif

if BUILD_OPENGL_SDL
expedite_SOURCES += \
engine_gl_sdl.c engine_gl_sdl.h
endif

if BUILD_FB
expedite_SOURCES += \
engine_fb.c engine_fb.h
endif

if BUILD_DIRECTFB
expedite_SOURCES += \
engine_directfb.c engine_directfb.h
endif

if BUILD_OPENGL_COCOA
expedite_SOURCES += \
engine_gl_cocoa.m engine_gl_cocoa.h
endif

if BUILD_SOFTWARE_16_X11
expedite_SOURCES += \
engine_software_16_x11.c engine_software_16_x11.h
endif

if BUILD_SOFTWARE_16_DDRAW
expedite_SOURCES += \
engine_software_16_ddraw.cpp engine_software_16_ddraw.h
endif

if BUILD_SOFTWARE_16_WINCE
expedite_SOURCES += \
engine_software_16_wince.c engine_software_16_wince.h  engine_software_16_wince.rc
endif

.rc.lo:
	$(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --tag=RC --mode=compile $(RC) $(RCFLAGS) $< -o $@

EXTRA_DIST = $(bin_SCRIPTS)
