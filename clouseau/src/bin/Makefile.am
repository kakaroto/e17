MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = clouseaud clouseau

clouseaud_SOURCES = \
		    clouseaud.c \
		    ../lib/eet_data.c \
		    ../lib/obj_information.c

clouseau_SOURCES = \
		 clouseau.c \
		 ../lib/eet_data.c \
		 ../lib/obj_information.c

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/include \
-I$(top_builddir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EFL_CFLAGS@

clouseaud_LDADD = @EFL_LIBS@
clouseau_LDADD = @EFL_LIBS@

