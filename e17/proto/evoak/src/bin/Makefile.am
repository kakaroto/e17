## Process this file with automake to produce Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
@my_cflags@

bin_PROGRAMS = \
evoak \
evoak_client_test \
evoak_client_test_rect \
evoak_client_test_text \
evoak_client_test_grad \
evoak_client_test_poly \
evoak_client_test_line \
evoak_client_test_edje \
evoak_bg_set_test

evoak_SOURCES = \
evoak_main.h \
evoak_main.c \
evoak_main_protocol.c

evoak_LDADD = \
@my_libs@

evoak_DEPENDENCIES = 

evoak_client_test_SOURCES = \
evoak_client_test_main.c

evoak_client_test_LDADD = \
$(top_builddir)/src/lib/libevoak.la

###### RECT
evoak_client_test_rect_DEPENDENCIES = $(top_builddir)/src/lib/libevoak.la

evoak_client_test_rect_SOURCES = \
evoak_client_test_rect.c

evoak_client_test_rect_LDADD = \
$(top_builddir)/src/lib/libevoak.la

###### TEXT
evoak_client_test_text_DEPENDENCIES = $(top_builddir)/src/lib/libevoak.la

evoak_client_test_text_SOURCES = \
evoak_client_test_text.c

evoak_client_test_text_LDADD = \
$(top_builddir)/src/lib/libevoak.la

###### GRAD
evoak_client_test_grad_DEPENDENCIES = $(top_builddir)/src/lib/libevoak.la

evoak_client_test_grad_SOURCES = \
evoak_client_test_grad.c

evoak_client_test_grad_LDADD = \
$(top_builddir)/src/lib/libevoak.la

###### POLY
evoak_client_test_poly_DEPENDENCIES = $(top_builddir)/src/lib/libevoak.la

evoak_client_test_poly_SOURCES = \
evoak_client_test_poly.c

evoak_client_test_poly_LDADD = \
$(top_builddir)/src/lib/libevoak.la

###### LINE
evoak_client_test_line_DEPENDENCIES = $(top_builddir)/src/lib/libevoak.la

evoak_client_test_line_SOURCES = \
evoak_client_test_line.c

evoak_client_test_line_LDADD = \
$(top_builddir)/src/lib/libevoak.la

###### EDJE
evoak_client_test_edje_DEPENDENCIES = $(top_builddir)/src/lib/libevoak.la

evoak_client_test_edje_SOURCES = \
evoak_client_test_edje.c

evoak_client_test_edje_LDADD = \
$(top_builddir)/src/lib/libevoak.la

###

evoak_bg_set_test_SOURCES = \
evoak_bg_set_test_main.c

evoak_bg_set_test_LDADD = \
$(top_builddir)/src/lib/libevoak.la

evoak_client_test_DEPENDENCIES = $(top_builddir)/src/lib/libevoak.la
