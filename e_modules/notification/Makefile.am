MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs \
                       ABOUT-NLS

SUBDIRS = src po

EDJE_FLAGS = -v \
	   -id $(top_srcdir)/images \
	   -fd $(top_srcdir)/fonts

# data files for the module
filesdir =  $(datadir)

files_DATA = module.desktop \
	     e-module-notification.edj

EXTRA_DIST = $(files_DATA) \
	     images/e17_dialog_watermark.png \
	     images/e17_menu_bg_border.png \
	     images/module_icon.png \
	     images/close_btn.png

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf e-module-notification.edj module.desktop *~
uninstall:
	 rm -rf $(DESTDIR)$(datadir)
	 
