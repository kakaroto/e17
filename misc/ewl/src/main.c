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
	          *btn;
	UNUSED(btn);
	ewl_init(&argc, &argv);

	/*ewl_container_insert(win,box);*/
	/*ewl_widget_show(box);*/
	ewl_callback_add(win, "mousedown", mousedown_cb, NULL);
	ewl_hash_dump(EWL_OBJECT(box)->data);
	ewl_widget_show(win);

	
	
	ewl_main();
	return 0;
}


void  mousedown_cb(void *object, EwlEvent *ev, void *data)
{
	Evas_Object obj = evas_add_image_from_file(ewl_widget_get_evas(EWL_WIDGET(object)), "/home/pabs/dl/images/anusmcgee.jpg");
	evas_show(ewl_widget_get_evas(EWL_WIDGET(object)), obj);
	return;
}
