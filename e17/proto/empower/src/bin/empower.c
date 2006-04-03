#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "Ewl.h"

char buf[1024];
Ewl_Widget *win = NULL;
int time_to_pipe = 0;
char password[1024];

void destroy_cb(Ewl_Widget *w, void *event, void *data)
{
	ewl_widget_destroy(win);
	ewl_main_quit();
}

void pipe_to_sudo(Ewl_Widget *w, void *event, void *data)
{	
	time_to_pipe = 1;
	
	snprintf(password, 1024, "%s", (char*)(ewl_password_text_get(EWL_PASSWORD(data))));

	ewl_widget_destroy(win);
	ewl_main_quit();
}

int main(int argc, char** argv)
{
	--argc; ++argv;
	
	Ewl_Widget *entry = NULL;
	Ewl_Widget *ok_button = NULL;
	Ewl_Widget *cancel_button = NULL;
	Ewl_Widget *progtext = NULL;
	Ewl_Widget *vbox=NULL, *hbox = NULL;
	
	if(argc)			//commands
	{
		snprintf(buf, 1024, "sudo -S %s ", *argv);
		
		--argc; ++argv;
		while(argc)
		{
			strncat(buf, " ", 1024);
			strncat(buf, *argv, 1024);
			--argc; ++argv;
		}
	}
	else
	{
		printf("Usage: ./empower <program name>\n");
		return 1;
	}
	

	
	if(!ewl_init(&argc, argv))
	{
		printf("Unable to init ewl\n");
		return 1;
	}
	
	win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(win), "Empower!");
	ewl_window_name_set(EWL_WINDOW(win), "Empower!");
	ewl_window_class_set(EWL_WINDOW(win), "Empower!");
	ewl_object_size_request(EWL_OBJECT(win), 250, 75);
	ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, destroy_cb, NULL);
	ewl_widget_show(win);
	
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(win), vbox);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
	ewl_widget_show(vbox);
	
	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
	ewl_widget_show(hbox);
	
	progtext = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(progtext), "Password: ");
	ewl_container_child_append(EWL_CONTAINER(hbox), progtext);
	ewl_object_maximum_size_set(EWL_OBJECT(progtext), 75, 25);
	ewl_widget_show(progtext);
	
	entry = ewl_password_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), entry);
	ewl_object_size_request(EWL_OBJECT(entry), 50, 50);
	ewl_callback_append(entry, EWL_CALLBACK_VALUE_CHANGED, pipe_to_sudo, entry);
	ewl_widget_show(entry);
		
	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_RIGHT);
	ewl_widget_show(hbox);
	
	ok_button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(ok_button), "Ok");
	ewl_container_child_append(EWL_CONTAINER(hbox), ok_button);
	ewl_callback_append(ok_button, EWL_CALLBACK_CLICKED, pipe_to_sudo, entry);
	ewl_widget_show(ok_button);
	
	cancel_button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(cancel_button), "Cancel");
	ewl_container_child_append(EWL_CONTAINER(hbox), cancel_button);
	ewl_callback_append(cancel_button, EWL_CALLBACK_CLICKED, destroy_cb, NULL);
	ewl_widget_show(cancel_button);

	ewl_main();
	
	if(time_to_pipe)
	{
		FILE *sudo_pipe;
	
		sudo_pipe = popen(buf, "w");
		fprintf(sudo_pipe, "%s\n", password);
		pclose(sudo_pipe);
	}
	
	return 0;
}

