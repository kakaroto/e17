## Process this file with automake to produce Makefile.in

SUBDIRS = src data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
			config.sub configure install-sh ltconfig ltmain.sh \
			missing mkinstalldirs stamp-h.in config.h.in depcomp \
			debian/changelog esmart.spec

bin_SCRIPTS = 

EXTRA_DIST = README AUTHORS COPYING esmart.spec esmart.spec.in \
	     debian/control debian/copyright debian/rules \
	     debian/changelog \
	     debian/changelog.in \
	     debian/libesmart0-dev.files debian/esmart0-bin.files \
	     debian/libesmart0.files debian/esmart0-bin.files \
	     esmart_container.pc.in \
	     esmart_draggies.pc.in \
	     esmart_file_dialog.pc.in \
	     esmart_resize.pc.in \
	     esmart_textarea.pc.in \
	     esmart_text_entry.pc.in \
	     esmart_thumb.pc.in \
	     esmart_trans_x11.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = esmart_container.pc \
		 esmart_draggies.pc \
		 esmart_file_dialog.pc \
		 esmart_resize.pc \
		 esmart_textarea.pc \
		 esmart_text_entry.pc \
		 esmart_thumb.pc \
		 esmart_trans_x11.pc


