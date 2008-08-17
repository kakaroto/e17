## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

if HAVE_EMOTION
  etk_extra_video_pc = etk_extra_video.pc
endif
  
if HAVE_CAIRO
  etk_extra_cairo_pc = etk_extra_cairo.pc
endif

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess etk_extra-config.h.in \
			config.sub configure install-sh ltconfig ltmain.sh \
			missing mkinstalldirs stamp-h.in config.h.in depcomp \
			etk_extra_tree_model_wobbly.pc etk_extra_video.pc etk_extra_cairo.pc

EXTRA_DIST = README AUTHORS COPYING etk_extra_wobbly.pc.in etk_extra_video.pc.in etk_extra_cairo.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = etk_extra_tree_model_wobbly.pc $(etk_extra_video_pc) $(etk_extra_cairo_pc)
