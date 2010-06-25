AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS =	@ELEMENTARY_CFLAGS@ -fPIC 
AM_LDFLAGS = -module -shared -avoid-version 
libdir = $(prefix)/lib/ensure/assurances

lib_LTLIBRARIES = 	\
                badclipsmart.la     \
                clippedsmart.la     \
                edjeerr.la          \
                emptytext.la        \
                imageerr.la         \
                largetext.la        \
                negativesize.la     \
                offscreen.la        \
                outofclip.la        \
                outofsmart.la       \
                premul.la           \
                smalltext.la        \
                trailingspace.la    \
                zeroalpha.la        \
                zerosize.la 

