#include <stdlib.h>
#include <stdio.h>
#include "box.h"
#include "window.h"
#include "api.h"

void  mousedown_cb(void *object, EwlEvent *ev, void *data);

int main(int argc, char *argv[])
{
	EwlWidget *win = ewl_window_new("toplevel"),
	          *box = ewl_hbox_new(FALSE),
	          *box2 = ewl_vbox_new(FALSE),
	          *btn;
	UNUSED(btn);
	ewl_init(&argc, &argv);

	ewl_callback_add(win, "mousedown", mousedown_cb, NULL);

	ewl_container_insert(win,box);
	ewl_container_insert(box,box2);
	ewl_widget_show(box2);
	ewl_widget_show(box);

	ewl_hash_dump(EWL_OBJECT(box)->data);
	ewl_widget_show(win);
	
	ewl_main();
	return 0;
}


void  mousedown_cb(void *object, EwlEvent *ev, void *data)
{
	int *x = ewl_event_get_data(ev, "x"),
	    *y = ewl_event_get_data(ev, "y"),
		*b = ewl_event_get_data(ev, "button");
	UNUSED(object);
	UNUSED(data);
	fprintf(stderr,"mousedown example:  button %d at (%d, %d).\n",
	        *b, *x, *y);
	return;
}
