AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/bin @ELATION_CFLAGS@

bin_PROGRAMS = elation

elation_SOURCES = elation_main.c elation_module.c Elation.h
elation_LDADD = @ELATION_LIBS@ -ldl


