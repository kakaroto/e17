#include <stdlib.h>
#include <stdio.h>
#include "box.h"
#include "window.h"
#include "api.h"

int main(int argc, char *argv[])
{
	EwlWidget *win = ewl_window_new("toplevel"),
	          *box = ewl_hbox_new(FALSE),
	          *btn;
	UNUSED(btn);
	ewl_init(&argc, &argv);

	ewl_container_insert(win,box);
	ewl_widget_show(box);
	ewl_hash_dump(EWL_OBJECT(box)->data);
	ewl_widget_show(win);

	
	
	ewl_main();
	return 0;
}

