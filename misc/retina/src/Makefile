_CFLAGS=`gtk-config --cflags` `imlib2-config --cflags` `evas-config --cflags`

all:
	gcc -c r_gtk.c $(_CFLAGS)
	gcc -c r_evas.c $(_CFLAGS)
	gcc -c r_evas_cb.c $(_CFLAGS)
	gcc -c r_browser.c $(_CFLAGS)
	gcc -c r_db.c $(_CFLAGS)
	gcc -c r_error.c $(_CFLAGS)
	gcc -c r_file_load.c $(_CFLAGS)
	gcc -c retina.c $(_CFLAGS)
	gcc -o retina r_gtk.o r_evas_cb.o r_evas.o r_browser.o \
	r_db.o r_error.o r_file_load.c retina.o \
	`gtk-config --libs` `imlib2-config --libs` `evas-config --libs` -ledb

