AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in
INCLUDES	     = @my_includes@ -I. -I$(top_srcdir)  -I$(top_builddir)

lib_LTLIBRARIES      = libImlib2.la
include_HEADERS      = Imlib2.h
libImlib2_la_SOURCES = \
api.c \
blend.c \
blend.h \
color.c \
color.h \
color_helpers.c \
color_helpers.h \
colormod.c \
colormod.h \
common.h \
context.c \
context.h \
draw.c \
draw.h \
dynamic_filters.c \
dynamic_filters.h \
ellipse.c \
file.c \
file.h \
filter.c \
filter.h \
font.h \
font_draw.c \
font_load.c \
font_main.c \
font_query.c \
format.c \
format.h \
grab.c \
grab.h \
grad.c \
grad.h \
image.c \
image.h \
line.c \
loaderpath.h \
polygon.c \
rectangle.c \
rend.c \
rend.h \
rgba.c \
rgba.h \
rgbadraw.c \
rgbadraw.h \
rotate.c \
rotate.h \
scale.c \
scale.h \
script.c \
script.h \
span.c \
span.h \
updates.c \
updates.h \
ximage.c \
ximage.h

MMX_SRCS = \
asm_blend.S \
asm_blend_cmod.S \
asm_rgba.S \
asm_rotate.S \
asm_scale.S

EXTRA_libImlib2_la_SOURCES = $(MMX_SRCS)

MMX_OBJS = $(MMX_SRCS:.S=.lo)

if BUILD_MMX
libImlib2_la_LIBADD       = $(MMX_OBJS) @my_libs@
libImlib2_la_DEPENDENCIES = $(top_builddir)/config.h $(MMX_OBJS)
else
libImlib2_la_LIBADD       = @my_libs@
libImlib2_la_DEPENDENCIES = $(top_builddir)/config.h
endif
libImlib2_la_LDFLAGS      = $(LDFLAGS) -version-info 3:0:2
