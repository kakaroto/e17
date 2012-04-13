MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = exquisite exquisite-write

exquisite_SOURCES = \
main.h \
main.c \
theme.c \
ipc.c \
ipc.h

exquisite_CPPFLAGS = \
-I. \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
@EXQUISITE_CFLAGS@ \
@ECORE_FB_CFLAGS@

exquisite_LDADD = @EXQUISITE_LIBS@ \
                  @ECORE_FB_LIBS@

exquisite_write_SOURCES = write.c
exquisite_write_CPPFLAGS = -I. @EXQUISITE_WRITE_CFLAGS@
exquisite_write_LDADD = @EXQUISITE_WRITE_LIBS@
