## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LDFLAGS		     = -L/usr/X11R6/lib
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src \
		       -I$(top_builddir)/src \
                       -I$(top_srcdir)/loaders \
                       $(freetype_cflags) -I/usr/X11R6/include $(X_CFLAGS) \
                       -I$(prefix)/include -I$(includedir)
MMX_SRCS             = asm_blend.S asm_rgba.S asm_scale.S asm_rotate.S \
		       asm_blend_cmod.S
MMX_OBJS             = $(MMX_SRCS:.S=.lo)

lib_LTLIBRARIES      = libImlib2.la
include_HEADERS      = Imlib2.h
libImlib2_la_SOURCES = scale.c image.c blend.c file.c rgbadraw.c api.c \
		       updates.c colormod.c format.c grad.c rotate.c \
		       filter.c script.c dynamic_filters.c color_helpers.c \
		       font_main.c font_load.c font_query.c font_draw.c \
                       Imlib2.h image.h scale.h blend.h updates.h colormod.h \
		       file.h common.h rgbadraw.h font.h format.h \
		       rotate.h grad.h filter.h script.h dynamic_filters.h \
		       color_helpers.h loaderpath.h

EXTRA_libImlib2_la_SOURCES = rend.c ximage.c rgba.c color.c grab.c draw.c \
			     context.c color.h context.h draw.h grab.h rend.h \
			     rgba.h ximage.h $(MMX_SRCS)
libImlib2_la_LDFLAGS      = -version-info 2:1:1

if HAVE_MMX
libImlib2_la_LIBADD   = $(X_OBJS) @DLLDFLAGS@ $(MMX_OBJS) \
                       -lltdl \
                       $(x_libs) $(freetype_libs) $(LDFLAGS) -lm
libImlib2_la_DEPENDENCIES = $(top_builddir)/config.h $(X_OBJS) $(MMX_OBJS)
else
libImlib2_la_LIBADD   = $(X_OBJS) @DLLDFLAGS@ \
                       -lltdl \
                       $(x_libs) $(freetype_libs) $(LDFLAGS) -lm
libImlib2_la_DEPENDENCIES = $(top_builddir)/config.h $(X_OBJS)
endif
