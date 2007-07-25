#include <Ewl.h>
#include "entropy.h"
#include "entropy_gui.h"
#include "entropy_config.h"
#include <dlfcn.h>
#include <time.h>

entropy_generic_file* local_file;


void
 __destroy_properties_dialog(Ewl_Widget *dialog, void *ev_data, void *user_data)
 {
	/*Remove a reference to this file*/
	if (local_file) 
		entropy_core_file_cache_remove_reference(local_file->md5);
	 
	 ewl_widget_destroy(EWL_WIDGET(user_data));
}


void open_with_cb(Ewl_Widget *w , void *ev_data , void *user_data )  {
	entropy_mime_action* action = NULL;
	Ewl_Dialog_Event* e = ev_data;	
	char* file = NULL;
	
	if (e->response == EWL_STOCK_OPEN) {
		Ecore_List* l;
		l = ewl_filedialog_selected_files_get(EWL_FILEDIALOG(w));
		ecore_list_first_goto(l);
		file = ecore_list_current(l);
		
		if (action) {
			action->executable = strdup(file);
		} else {
			entropy_core_mime_action_add(local_file->mime_type, file );
		}
	}

	ewl_widget_destroy(EWL_WIDGET(w));
}


void ewl_properties_dialog_openwith_cb(Ewl_Widget *w , void *ev_data , void *user_data ) {
	Ewl_Widget* file_dialog = ewl_filedialog_new();

        ewl_callback_append (file_dialog, EWL_CALLBACK_VALUE_CHANGED, open_with_cb, NULL);
	ewl_widget_show(file_dialog);
}

void ewl_icon_local_viewer_show_stat(entropy_file_stat* file_stat) {
	Ewl_Widget* window;
	Ewl_Widget* vbox;
	Ewl_Widget* image;
	Ewl_Widget* hbox;
	Ewl_Widget* text;
	Ewl_Widget* button;
	char itext[100];
	time_t stime;

	/*Save a local copy of this stat for affiliated functions*/
	local_file = file_stat->file;

	/*Add a reference to this file while the dialog is open*/
	if (local_file) 
		entropy_core_file_cache_add_reference(local_file->md5);

	
	window = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(window), "File Properties");
	ewl_object_custom_size_set(EWL_OBJECT(window), 300, 400);
	
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(window), vbox);
	ewl_widget_show(vbox);


	/*----------------------------*/
	/*The icon*/
	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	image = ewl_image_new();
	ewl_image_constrain_set(EWL_IMAGE(image), 64);
	if (file_stat->file->thumbnail) {
		ewl_image_file_set(EWL_IMAGE(image), file_stat->file->thumbnail->thumbnail_filename, NULL);
	} else {
		ewl_image_file_set(EWL_IMAGE(image), PACKAGE_DATA_DIR "/icons/default.png", NULL);
	}
	ewl_container_child_append(EWL_CONTAINER(hbox), image);
	ewl_widget_show(image);

	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), file_stat->file->filename);
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);


	/*---------------------------*/
	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);



	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), "Location");
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);

	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), file_stat->file->path);
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);

	/*--------------------------*/



	/*----------------------------------------*/
	/*hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), "Filename");
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);*/




	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), "Type: ");
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);






	text = ewl_text_new();
	if (strlen(file_stat->file->mime_type)) {
		ewl_text_text_set(EWL_TEXT(text), file_stat->file->mime_type);
	} else {
		ewl_text_text_set(EWL_TEXT(text), "object/unknown");
	}
	
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);

	if (strlen(file_stat->file->mime_type)) {
		button = ewl_button_new();
		ewl_callback_append(button, EWL_CALLBACK_CLICKED, ewl_properties_dialog_openwith_cb, NULL);
		ewl_button_label_set(EWL_BUTTON(button), "Open with..");
		ewl_object_custom_size_set(EWL_OBJECT(button), 70, 10);
		ewl_container_child_append(EWL_CONTAINER(hbox), button);
		ewl_widget_show(button);
	}
	/*--------------------------------*/



	/*----------------------------------*/
	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), "Plugin URI");
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);



	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), file_stat->file->uri_base);
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);




	/*---------------------------------*/
	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), "Size: ");
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);

	text = ewl_text_new();
	snprintf(itext, 100, "%lld kb", (long long)(file_stat->stat_obj->st_size / 1024));
	ewl_text_text_set(EWL_TEXT(text), itext);
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);
	/*-------------------------------------*/

	/*---------------------------------*/
	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), "Modified Time");
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);

	text = ewl_text_new();
	stime = file_stat->stat_obj->st_mtime;
	ewl_text_text_set(EWL_TEXT(text), ctime(&stime));
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);
	/*-------------------------------------*/


	

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "OK");
	ewl_container_child_append(EWL_CONTAINER(vbox), button);
	ewl_object_maximum_h_set(EWL_OBJECT(button), 15);
	ewl_widget_show(button);
	ewl_callback_append(EWL_WIDGET(button), EWL_CALLBACK_CLICKED, __destroy_properties_dialog, window);

	
	
	
	/*printf("Got a 'stat available' object\n");
	printf("File size: %d\n", file_stat->stat_obj->st_size);
	printf("File inode: %d\n", file_stat->stat_obj->st_ino);
	printf("File uid: %d\n", file_stat->stat_obj->st_uid);
	printf("File gid: %d\n", file_stat->stat_obj->st_gid);
	printf("Last access: %d\n", file_stat->stat_obj->st_atime);
	printf("Last modify : %d\n", file_stat->stat_obj->st_mtime);*/


	ewl_widget_show(window);
}

