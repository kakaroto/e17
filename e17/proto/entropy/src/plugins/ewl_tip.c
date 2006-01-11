#include <Ewl.h>
#include "entropy.h"
#include "entropy_gui.h"
#include "entropy_config.h"
#include <dlfcn.h>
#include <time.h>

Ecore_List* tool_tips;
Ewl_Widget *text_tip;
int tip_number;

void ewl_entropy_tip_window_destroy_cb(Ewl_Widget* w, void* ev_data, void* user_data);
void ewl_entropy_tip_window_create_tips();
void ewl_entropy_tip_window_tip_next_cb(Ewl_Widget* w, void* ev_data, void* user_data);
char* ewl_entropy_tip_window_tip_next();


void ewl_entropy_tip_window_destroy_cb(Ewl_Widget* w, void* ev_data, void* user_data) 
{
	ecore_list_destroy(tool_tips);
	ewl_widget_destroy(EWL_WIDGET(user_data));
}


void ewl_entropy_tip_window_tip_next_cb(Ewl_Widget* w, void* ev_data, void* user_data) {
	char* tip = ewl_entropy_tip_window_tip_next();
	ewl_text_text_set(EWL_TEXT(text_tip), tip);
}


void ewl_entropy_tip_window_create_tips()
{
	tool_tips = ecore_list_new();
	ecore_list_append(tool_tips, "You can add a new 'location' by clicking on \"Add Location\"\nin the Tools menu");
	ecore_list_append(tool_tips, "Entropy can browse .tar.gz and .tar.bz2 files.  Just click on the\n file, and it will be"
				     "treated as a regular folder");
}

char* ewl_entropy_tip_window_tip_next() {
	char* next = ecore_list_next(tool_tips);
	if (!next) {
		ecore_list_goto_first(tool_tips);
		next = ecore_list_next(tool_tips);
	}
	return next;
}

void ewl_entropy_tip_window_display() 
{
	Ewl_Widget* tip_window = ewl_window_new();
	Ewl_Widget* vbox = ewl_vbox_new();
	Ewl_Widget* hbox = ewl_hbox_new();
	Ewl_Widget* button;

	ewl_entropy_tip_window_create_tips();
	tip_number = 0;
	
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_BOTTOM);	
	ewl_object_alignment_set(EWL_OBJECT(vbox), EWL_FLAG_ALIGN_BOTTOM);
	ewl_object_alignment_set(EWL_OBJECT(tip_window), EWL_FLAG_ALIGN_BOTTOM);
	ewl_object_fill_policy_set(EWL_OBJECT(tip_window), EWL_FLAG_FILL_FILL);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_FILL);
	
	ewl_object_minimum_size_set(EWL_OBJECT(tip_window), 400, 150);
	ewl_container_child_append(EWL_CONTAINER(tip_window), vbox);
	
	text_tip = ewl_text_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), text_tip);
	

	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Prev Tip");
	ewl_object_custom_h_set(EWL_OBJECT(button), 15);
	ewl_widget_show(button);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Next Tip");
	ewl_object_custom_h_set(EWL_OBJECT(button), 15);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, ewl_entropy_tip_window_tip_next_cb, NULL);
	ewl_widget_show(button);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Close");
	ewl_object_custom_h_set(EWL_OBJECT(button), 15);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, ewl_entropy_tip_window_destroy_cb, tip_window);
	ewl_widget_show(button);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);

		
	ewl_callback_append(tip_window, EWL_CALLBACK_DELETE_WINDOW, ewl_entropy_tip_window_destroy_cb, tip_window);
	
	ewl_widget_show(tip_window);
	ewl_widget_show(vbox);
	ewl_widget_show(hbox);
	ewl_widget_show(text_tip);

	ewl_entropy_tip_window_tip_next_cb(NULL,NULL,NULL);
	
}
