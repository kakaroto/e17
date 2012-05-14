MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = clouseaud client

clouseaud_SOURCES = \
		    clouseaud.c \
		    ../lib/eet_dump.c \
		    ../lib/helper.c \
		    ../lib/ui/obj_information.c

client_SOURCES = \
		 client.c \
		 ../lib/helper.c \
		 ../lib/ui/obj_information.c \
		 ../lib/eet_dump.c

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/include \
-I$(top_builddir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EFL_CFLAGS@

clouseaud_LDADD = @EFL_LIBS@
client_LDADD = @EFL_LIBS@

