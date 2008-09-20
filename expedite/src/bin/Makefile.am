MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EVAS_CFLAGS@ \
@x_cflags@ \
@SDL_CFLAGS@ \
@DIRECTFB_CFLAGS@ \
@XCB_CFLAGS@

bin_PROGRAMS = expedite

expedite_SOURCES = \
main.c main.h \
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
image_data_ycbcr601pl_wide_stride.c \
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
poly_blend.c

expedite_CFLAGS = @WIN32_CFLAGS@
expedite_LDADD = @EVAS_LIBS@ @SDL_LIBS@ @x_libs@ @ddraw_libs@ @ddraw_16_libs@ @d3d_libs@ @wince_16_libs@ @glew_libs@ @DIRECTFB_LIBS@ @XCB_LIBS@ @quartz_libs@ -lm
expedite_LDFLAGS = @lt_enable_auto_import@

if BUILD_SOFTWARE_X11
expedite_SOURCES += \
engine_software_x11.c engine_software_x11.h
endif

if BUILD_XRENDER_X11
expedite_SOURCES += \
engine_xrender_x11.c engine_xrender_x11.h
endif

if BUILD_OPENGL_X11
expedite_SOURCES += \
engine_gl_x11.c engine_gl_x11.h
endif

if BUILD_XCB
expedite_SOURCES += \
engine_software_xcb.c engine_software_xcb.h
endif

if BUILD_SOFTWARE_DDRAW
expedite_SOURCES += \
engine_software_ddraw.cpp engine_software_ddraw.h
endif

if BUILD_DIRECT3D
expedite_SOURCES += \
engine_direct3d.cpp engine_direct3d.h
endif

if BUILD_OPENGL_GLEW
expedite_SOURCES += \
engine_gl_glew.c engine_gl_glew.h
endif

if BUILD_SOFTWARE_SDL
expedite_SOURCES += \
engine_software_sdl.c engine_software_sdl.h
endif

if BUILD_FB
expedite_SOURCES += \
engine_fb.c engine_fb.h
endif

if BUILD_DIRECTFB
expedite_SOURCES += \
engine_directfb.c engine_directfb.h
endif

if BUILD_QUARTZ
expedite_SOURCES += \
engine_quartz.h

### add to SOURCES one we know how to fix:
# /usr/share/automake-1.9/am/depend2.am: am__fastdepOBJC does not appear in
# AM_CONDITIONAL
# engine_quartz.m 
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
engine_software_16_wince.c engine_software_16_wince.h
endif
