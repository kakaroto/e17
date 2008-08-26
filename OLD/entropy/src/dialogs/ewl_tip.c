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
	ecore_list_append(tool_tips, "To copy a file, drag the icon from the icon view to a folder in the left hand pane");
	ecore_list_append(tool_tips, "The icon view supports keyboard navigation.  Arrow keys, and special keys\n like 'Delete'"
				     "will execute appropriate actions");
	ecore_list_append(tool_tips, "File properties can be viewed by selecting an icon, and clicking 'Properties'");
	ecore_list_append(tool_tips, "You can change the action that is executed for a particular file type\n"
				     "by opening the 'Properties' dialog for a file of that type, and clicking\n"
				     "'Open With..'");
	ecore_list_append(tool_tips, "The MIME-Type dialog allows fine grained control over the action executed\n"
				     "for a file type.");
	ecore_list_append(tool_tips, "Entropy is able to thumbnail images in any filesystem supported by EVFS.\n"
				     "For instance, this means that Entropy can thumbnail files in a .tar.bz2\n"
				     "file on a Samba share!\n");

	
}

char* ewl_entropy_tip_window_tip_next() {
	char* next = ecore_list_next(tool_tips);
	if (!next) {
		ecore_list_first_goto(tool_tips);
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
	Ewl_Widget* image = ewl_image_new();

	ewl_entropy_tip_window_create_tips();
	tip_number = 0;
	
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_BOTTOM);	
	ewl_object_alignment_set(EWL_OBJECT(vbox), EWL_FLAG_ALIGN_BOTTOM);
	ewl_object_alignment_set(EWL_OBJECT(tip_window), EWL_FLAG_ALIGN_BOTTOM);
	ewl_object_fill_policy_set(EWL_OBJECT(tip_window), EWL_FLAG_FILL_FILL);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_FILL);
	
	ewl_container_child_append(EWL_CONTAINER(tip_window), vbox);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	
	text_tip = ewl_text_new();
	ewl_object_minimum_h_set(EWL_OBJECT(text_tip), 80);
	ewl_container_child_append(EWL_CONTAINER(hbox), text_tip);

	ewl_image_file_set(EWL_IMAGE(image), PACKAGE_DATA_DIR "/icons/tip.png", NULL);
	ewl_container_child_append(EWL_CONTAINER(hbox), image);
	ewl_widget_show(hbox);
	
	hbox = ewl_hbox_new();
	ewl_widget_show(hbox);
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
	ewl_widget_show(text_tip);
	ewl_widget_show(image);

	ewl_entropy_tip_window_tip_next_cb(NULL,NULL,NULL);
	
}
