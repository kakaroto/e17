#include "Empower.h"

void key_down_cb(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Event_Key_Down *ev;
	
	ev = event;
	
	if(!ev->base.modifiers)
	{
		if(strcmp(ev->base.keyname, "Escape") == 0)
		{
			ewl_widget_destroy(EWL_WIDGET(win));
			ewl_main_quit();
			exit(-1);
		}
	}
}

void destroy_cb(Ewl_Widget *w, void *event, void *data)
{
	ewl_widget_destroy(EWL_WIDGET(win));
	ewl_main_quit();
	
	exit(-1);
}

void reveal_cb(Ewl_Widget *w, void *event, void *data)
{
	ewl_window_raise(EWL_WINDOW(win));
}

void check_pass_cb(Ewl_Widget *w, void *event, void *data)
{	
	char *pass = ewl_password_text_get(EWL_PASSWORD(data));

	if(pass && strlen(pass))
	{
		ewl_widget_disable(win);
		
		if(mode == SUDO)
			authorize(pass);
		else
		{
			printf("%s",pass);
			ewl_main_quit();
		}
	}
}

int sudo_done_cb(void *data, int type, void *event)
{
	Ecore_Exe_Event_Del *ev = event;
	int* code = data;
	
	sudo = NULL;
	ewl_main_quit();
	
	if((ev->exit_code))
		exit(-1);
	
	return 0;
}

int sudo_data_cb(void *data, int type, void *event)
{
	Ecore_Exe_Event_Data *ev = event;
	
	if(ev->size > 1)
		display_window();
	
	return 0;
}

//EXIT SIGNAL HANDLER
int exit_cb(void *data, int type, void *event)
{	
	ewl_main_quit();
	
	exit(-1);
}
