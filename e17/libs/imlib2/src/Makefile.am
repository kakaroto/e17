## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in loaderpath.h

LDFLAGS		     = -L/usr/X11R6/lib
INCLUDES	     = -I/usr/X11R6/include -I$(top_srcdir)/libltdl \
                       $(X_CFLAGS) -I$(prefix)/include -I$(includedir) \
		       -I. -I$(top_srcdir) -I$(top_srcdir)/src \
		       -I$(top_srcdir)/loaders
MMX_SRCS             = asm_blend.S asm_rgba.S asm_scale.S asm_rotate.S \
		       asm_blend_cmod.S
MMX_OBJS             = $(MMX_SRCS:.S=.lo)

lib_LTLIBRARIES      = libImlib2.la
include_HEADERS      = Imlib2.h
libImlib2_la_SOURCES = scale.c image.c blend.c file.c rgbadraw.c api.c \
		       updates.c colormod.c font.c format.c grad.c rotate.c \
		       filter.c script.c dynamic_filters.c color_helpers.c \
                       Imlib2.h image.h scale.h blend.h updates.h colormod.h \
		       file.h common.h rgbadraw.h font.h format.h \
		       rotate.h grad.h filter.h script.h dynamic_filters.h \
		       color_helpers.h

EXTRA_libImlib2_la_SOURCES = rend.c ximage.c rgba.c color.c grab.c draw.c \
			     context.c color.h context.h draw.h grab.h rend.h \
			     rgba.h ximage.h $(MMX_SRCS)
libImlib2_la_LDFLAGS      = -version-info 1:3:0

if HAVE_MMX
libImlib2_la_LIBADD   = $(X_OBJS) @DLLDFLAGS@ $(MMX_OBJS) \
                       $(top_builddir)/libltdl/libltdlc.la \
                       $(x_libs) -lttf $(LDFLAGS)
libImlib2_la_DEPENDENCIES = $(top_builddir)/config.h $(X_OBJS) $(MMX_OBJS)
else
libImlib2_la_LIBADD   = $(X_OBJS) @DLLDFLAGS@ \
                       $(top_builddir)/libltdl/libltdlc.la \
                       $(x_libs) -lttf $(LDFLAGS)
libImlib2_la_DEPENDENCIES = $(top_builddir)/config.h $(X_OBJS)
endif

SYS_LOADERS_PATH = @pkglibdir@
image.lo: loaderpath.h
loaderpath.h: Makefile
	@echo "#define SYS_LOADERS_PATH \"${SYS_LOADERS_PATH}\"" > $@
	@echo "#define USER_LOADERS_PATH \".loaders\"" >> $@
