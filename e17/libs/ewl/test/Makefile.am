AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

CFLAGS			=	-g -W -Wall -Wno-implicit
INCLUDES		=	-I$(top_srcdir)/src @ebits_cflags@ @ecore_cflags@ @evas_cflags@ @ewd_cflags@

bin_PROGRAMS			=	ewl_test

ewl_test_DEPENDENCIES		=	$(top_builddir)/src/libewl.la
ewl_test_SOURCES		=	ewl_test.c \
					ewl_box_test.c \
					ewl_button_test.c
ewl_test_LDFLAGS		=	-static
ewl_test_LDADD			=	$(top_builddir)/src/libewl.la @ebits_libs@ @ecore_libs@ @evas_libs@ @ewd_libs@
