## Process this file with automake to produce Makefile.in

SUBDIRS = src data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
			config.sub configure install-sh ltconfig ltmain.sh \
			missing mkinstalldirs stamp-h.in config.h.in depcomp \
			esmart.spec

bin_SCRIPTS = 

EXTRA_DIST = README AUTHORS COPYING esmart.spec esmart.spec.in \
	     esmart_container.pc.in \
	     esmart_draggies.pc.in \
	     esmart_file_dialog.pc.in \
	     esmart_resize.pc.in \
	     esmart_textarea.pc.in \
	     esmart_text_entry.pc.in \
	     esmart_thumb.pc.in \
	     esmart_trans_x11.pc.in

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = 

if BUILD_ESMART_CONTAINER
  pkgconfig_DATA += esmart_container.pc
endif

if BUILD_ESMART_DRAGGIES
  pkgconfig_DATA += esmart_draggies.pc
endif

if BUILD_ESMART_RESIZE
  pkgconfig_DATA += esmart_resize.pc
endif
  
if BUILD_ESMART_FILE_DIALOG
  pkgconfig_DATA += esmart_file_dialog.pc
endif

if BUILD_ESMART_TEXT_ENTRY
  pkgconfig_DATA += esmart_text_entry.pc
endif

if BUILD_ESMART_TEXTAREA
  pkgconfig_DATA += esmart_textarea.pc
endif

if BUILD_ESMART_THUMB
  pkgconfig_DATA += esmart_thumb.pc
endif

if BUILD_ESMART_TRANS_X11
  pkgconfig_DATA += esmart_trans_x11.pc
endif
