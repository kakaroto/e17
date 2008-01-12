MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs \
                       ABOUT-NLS

SUBDIRS = src

EDJE_FLAGS = -v \
	   -id $(top_srcdir)/images \
	   -fd $(top_srcdir)/fonts

# data files for the module
filesdir =  $(datadir)

files_DATA = module.desktop \
	     e-module-echo.edj

EXTRA_DIST = $(files_DATA) \
	     e_modules-echo.spec \
	     images/module_icon.png \
             images/base.png \
             images/left_high.png \
             images/left_medium.png \
             images/left_low.png \
             images/right_high.png \
             images/right_medium.png \
             images/right_low.png \
             images/mute.png

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf e-module-echo.edj module.desktop *~
uninstall:
	 rm -rf $(DESTDIR)$(datadir)
	 
