#include <stdio.h>
#include <stdlib.h>
#include "ewl.h"

EwlBool cb_test_option(int argc, char *argv[]);
EwlBool cb_mouse(EwlWidget *w, EwlEvent *ev, EwlData *d);
EwlBool cb_keydown(EwlWidget *w, EwlEvent *ev, EwlData *d);
EwlBool cb_resize(EwlWidget *w, EwlEvent *ev, EwlData *d);

EwlWidget *win;

int main(int argc, char *argv[])
{
	EwlWidget *box;
	EwlWidget *btn;
	EwlWidget *lbl;

	/* declare command line options */
	ewl_option_add("t", "test", "This is a test option.",cb_test_option);

	/* initialize ewl */
	/*ewl_debug_enable();*/
	ewl_init(&argc, &argv);
	ewl_set_application_name("ewltest");

	/* initialize widgets  */
	win = ewl_window_new_with_values(EWL_WINDOW_TOPLEVEL,
	                                 "EWL Test Application",
	                                 320, 240);
	box = ewl_hbox_new(FALSE);
	btn = ewl_button_new_with_label("Test Button");
	lbl = ewl_label_new("Test EwlLabel");

	/* set up window attributes */
	ewl_window_move(win,320,240);
	ewl_window_resize(win,640,480);
	ewl_window_set_class_hints(win,"blah", "blah");
	ewl_window_set_decoration_hint(win, TRUE);

	/* connect callbacks, adjust widgtes -- notice how you can chain
	   callbacks */
	ewl_callback_add(btn, EWL_EVENT_MOUSEDOWN, cb_mouse, NULL);
	ewl_callback_add(btn, EWL_EVENT_MOUSEUP, cb_resize, NULL);
	ewl_callback_add(btn, EWL_EVENT_MOUSEUP, cb_mouse, NULL);

	/* pack widget(s) into container */
	ewl_box_pack_end(box,btn);
	ewl_widget_show(btn);
	ewl_box_pack_end(box,lbl);
	ewl_widget_show(lbl);

	/* pack container into window */
	ewl_window_pack(win,box);
	ewl_widget_show(box);
	ewl_widget_show(win);

	/* DEBUGGING */
	fprintf(stderr, "win = 0x%08x\nbox = 0x%08x\nbtn = 0x%08x\n",
	        (unsigned int) win, (unsigned int) box, (unsigned int) btn);

	/*evas_show(ewl_widget_get_evas(btn),btn->bg);*/
	/* call the ewl_main() routine */
	ewl_main();

	return 0;
}


char cb_test_option(int argc, char *argv[])
{
	fprintf(stderr,"wahoo!\n");
	return 1;
}

EwlBool cb_mouse(EwlWidget *w, EwlEvent *ev, EwlData *d)
{
	char evtype[8] = "";
	double wid, hgt;

	evas_get_geometry(ewl_widget_get_evas(w),
	                  ewl_widget_get_background(w),
	                  0, 0, &wid, &hgt);
	switch (ev->type)	{
	case EWL_EVENT_MOUSEDOWN:
		evas_set_image_file(ewl_widget_get_evas(w),
			                ewl_widget_get_background(w),
	   		                ewl_theme_find_file(
		                      ewl_theme_get_string(
		                        "/EwlButton/images/clicked")));
		evas_set_image_fill(ewl_widget_get_evas(w),
		                    ewl_widget_get_background(w),
		                    0, 0, wid, hgt);
		evas_resize(ewl_widget_get_evas(w),
		            ewl_widget_get_background(w),
		            wid, hgt);
		sprintf(evtype,"down");
		break;
	case EWL_EVENT_MOUSEUP:
		evas_set_image_file(ewl_widget_get_evas(w),
			                ewl_widget_get_background(w),
	   		                ewl_theme_find_file(
		                      ewl_theme_get_string(
		                        "/EwlButton/images/normal")));
		evas_set_image_fill(ewl_widget_get_evas(w),
		                    ewl_widget_get_background(w),
		                    0, 0, wid, hgt);
		evas_resize(ewl_widget_get_evas(w),
		            ewl_widget_get_background(w),
		            wid, hgt);
		sprintf(evtype,"up");
		break;
	/*case EWL_EVENT_FOCUSIN:
		evas_set_image_file(ewl_widget_get_evas(w),
			                ewl_widget_get_background(w),
	   		                ewl_theme_find_file(
		                      ewl_theme_get_string(
		                        "/EwlButton/images/hilited")));
		evas_set_image_fill(ewl_widget_get_evas(w),
		                    ewl_widget_get_background(w),
		                    0, 0, wid, hgt);
		evas_resize(ewl_widget_get_evas(w),
		            ewl_widget_get_background(w),
		            wid, hgt);
		sprintf(evtype,"enter");
	case EWL_EVENT_FOCUSOUT:
		evas_set_image_file(ewl_widget_get_evas(w),
			                ewl_widget_get_background(w),
	   		                ewl_theme_find_file(
		                      ewl_theme_get_string(
		                        "/EwlButton/images/normal")));
		evas_set_image_fill(ewl_widget_get_evas(w),
		                    ewl_widget_get_background(w),
		                    0, 0, wid, hgt);
		evas_resize(ewl_widget_get_evas(w),
		            ewl_widget_get_background(w),
		            wid, hgt);
		sprintf(evtype,"leave");*/
	default:
		break;
	}

	fprintf(stderr,"mouse%s in widget 0x%08x\n", evtype, (unsigned int) w);
	return TRUE;
}

EwlBool cb_keydown(EwlWidget *w, EwlEvent *ev, EwlData *d)
{
	fprintf(stderr,"keydown in widget 0x%08x\n", (unsigned int) w);
	ewl_quit();
	return TRUE;
}

EwlBool cb_resize(EwlWidget *w, EwlEvent *ev, EwlData *d)
{
	EwlEventMouseup *mev = (EwlEventMouseup*) ev;
	switch (mev->button) {
	case 1:
		/*ewl_window_resize(win,160,120);*/
		break;
	case 2:
		/*ewl_window_move(win,320,240);*/
		break;
	case 3:
		ewl_quit();
		break;
	default:
		break;
	}
	return TRUE;
}
