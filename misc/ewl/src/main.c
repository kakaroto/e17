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
	int        t = 0;

	/* declare command line options */
	ewl_option_add("t", "test", "This is a test option.",cb_test_option);

	/* initialize ewl */
	/*ewl_debug_enable();*/
	ewl_init(&argc, &argv);
	ewl_set_application_name("ewltest");

	/* initialize widgets  */
	win = ewl_window_new_with_values(EWL_WINDOW_TOPLEVEL, "EWL Text",
									320, 240);
	box = ewl_hbox_new(FALSE);
	btn = ewl_button_new_with_label("Test Button");

	/* connect callbacks, adjust widgtes */
	ewl_callback_add(win, EWL_EVENT_MOUSEDOWN, cb_mouse, NULL);
	ewl_callback_add(btn, EWL_EVENT_MOUSEDOWN, cb_mouse, NULL);
	ewl_callback_add(btn, EWL_EVENT_MOUSEUP, cb_mouse, NULL);
	ewl_callback_add(btn, EWL_EVENT_MOUSEUP, cb_resize, NULL);

	ewl_window_move(win,800,600);
	ewl_window_resize(win,640,480);
	ewl_window_set_class_hints(win,"blah", "blah");
	ewl_window_set_decoration_hint(win, TRUE);

	/* pack widget(s) into container */
	box = ewl_vbox_new(FALSE);
	btn = ewl_button_new_with_label("Test Button");
	ewl_callback_add(btn, EWL_EVENT_MOUSEDOWN, cb_mouse, NULL);
	ewl_callback_add(btn, EWL_EVENT_MOUSEUP, cb_mouse, NULL);
	ewl_callback_add(btn, EWL_EVENT_MOUSEUP, cb_resize, NULL);
	ewl_box_pack_end(box,btn);
	ewl_widget_show(btn);

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

	if (ev->type==EWL_EVENT_MOUSEDOWN)
		sprintf(evtype,"down");
	else if (ev->type==EWL_EVENT_MOUSEUP)
		sprintf(evtype,"up");
	else if (ev->type==EWL_EVENT_MOUSEMOVE)
		sprintf(evtype,"move");

	if (w->type==EWL_BUTTON&&ev->type==EWL_EVENT_MOUSEDOWN) {
		ewl_widget_imlayer_show(w, "clicked");
		fprintf(stderr,"showing imlayer\n");
	} else if (w->type==EWL_BUTTON&&ev->type==EWL_EVENT_MOUSEUP) {
		ewl_widget_imlayer_hide(w, "clicked");
		fprintf(stderr,"hiding imlayer\n");
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
		ewl_window_resize(win,50,50);
		break;
	case 2:
		ewl_window_move(win,20,20);
		break;
	case 3:
		ewl_quit();
		break;
	default:
		break;
	}
	return TRUE;
}
