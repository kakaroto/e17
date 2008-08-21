ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing config.rpath mkinstalldirs 

SUBDIRS = src config

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
-id $(top_srcdir)/images \
-fd $(top_srcdir)/fonts

filesdir = $(datadir)
files_DATA = module.desktop \
	     e-module-illume.edj \
	     illume.edj

kbdsdir = $(datadir)/keyboards
kbds_DATA = \
            keyboards/ignore_built_in_keyboards \
            keyboards/Default.kbd \
            keyboards/alpha.png \
            keyboards/Numbers.kbd \
            keyboards/numeric.png \
            keyboards/Terminal.kbd \
            keyboards/qwerty.png \
	    keyboards/up.png \
	    keyboards/down.png \
	    keyboards/left.png \
	    keyboards/right.png \
	    keyboards/shift.png \
	    keyboards/tab.png \
	    keyboards/enter.png \
	    keyboards/backspace.png

dictsdir = $(datadir)/dicts
dicts_DATA = \
            dicts/English_(US).dic \
            dicts/English_(US)_Small.dic

EXTRA_DIST = $(files_DATA) $(kbds_DATA) $(dicts_DATA) \
	     illume.edc \
	     e-module-illume.edc \
	     fonts/Vera.ttf \
	     fonts/VeraBd.ttf \
	     images/module_icon.png \
	     images/logo.png \
	     images/a1.png \
	     images/a2.png \
	     images/a3.png \
	     images/a4.png \
	     images/a5.png \
	     images/a6.png \
	     images/a7.png  \
	     images/apps.png  \
	     images/bt1.png  \
	     images/bt2.png  \
	     images/close.png  \
	     images/dark_vgrad.png  \
	     images/launcher.png

%.edj: %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@
	
clean-local:
	rm -rf *.edj *~
	
uninstall-hook:
	rm -rf $(datadir)
