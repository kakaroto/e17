MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/keyboards -fd $(top_srcdir)/keyboards

filesdir = $(datadir)/ekbd/keyboards
files_DATA = \
	    Original.kbd \
	    Original_v.kbd \
	    qwerty.png \
	    shift.png \
	    tab.png \
	    enter.png \
	    backspace.png \
	    Math.tie \
	    Symbol.tie \
	    Extra.tie \
            Punct.tie \
	    math.edj \
	    quote.edj


.edc.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/keyboards/$< \
	 $(top_builddir)/keyboards/$@

clean-local:
	rm -f *.edj

EXTRA_DIST = $(files_DATA) \
             math.edc \
	     quote.edc \
	     macro.edc

