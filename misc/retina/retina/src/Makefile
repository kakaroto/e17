all:
	gcc -c r_gtk.c
	gcc -c r_evas.c
	gcc -c r_evas_cb.c
	gcc -c r_db.c
	gcc -c retina.c
	gcc -o retina r_gtk.o r_evas.o r_evas_cb.o r_db.o retina.o \
	`gtk-config --cflags` `imlib2-config --cflags` `evas-config --cflags` `gtk-config --libs` `imlib2-config --libs` `evas-config --libs` -ledb

