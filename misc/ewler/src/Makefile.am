## Process this file with automake to produce Makefile.in

INCLUDES = @ewl_cflags@ @xml2_cflags@

bin_PROGRAMS = ewler

ewler_SOURCES = \
	ewl_button_stock.c ewl_button_stock.h \
	ewl_dialog.c ewl_dialog.h \
	ewl_filedialog_stock.c ewl_filedialog_stock.h \
	ewl_stock.c ewl_stock.h \
	ewler.c ewler.h \
	form.c form.h \
	form_file.c form_file.h \
	inspector.c inspector.h \
	main.c \
	project.c project.h \
	widgets.c widgets.h

ewler_LDADD = @ewl_libs@ @xml2_libs@
