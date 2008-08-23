## Process this file with automake to produce Makefile.in

INCLUDES = \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = e16menuedit2

e16menuedit2_SOURCES = \
        callbacks.c\
        callbacks.h\
        e16menuedit2.c\
        e16menuedit2.h\
        file.c\
        file.h\
        nls.h\
        icon_chooser.c\
        icon_chooser.h\
        e16menu.c\
        e16menu.h\
        treeview.c\
        treeview.h\
        libglade_support.c\
        libglade_support.h\
	regex_func.c\
	regex_func.h

e16menuedit2_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)
