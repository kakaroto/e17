#include <Etk.h>
#include "entropy.h"

typedef struct _Entropy_Etk_Properties_Dialog Entropy_Etk_Properties_Dialog;
struct _Entropy_Etk_Properties_Dialog {
	Etk_Widget* window;
	Entropy_Generic_File* file;
};

Etk_Bool _etk_window_deleted_cb (Etk_Object * object, void *data) 
{
	Entropy_Etk_Properties_Dialog* dialog= data;

	/*Remove the file reference*/
	entropy_core_file_cache_remove_reference(dialog->file->md5);

	etk_object_destroy(ETK_OBJECT(dialog->window));
	entropy_free(dialog);

	return ETK_TRUE;
}

void etk_properties_dialog_new(Entropy_Generic_File* file) 
{
	Entropy_Etk_Properties_Dialog* dialog;
	Etk_Widget* notebook;
	Etk_Widget* vbox;
	Etk_Widget* button;
	Etk_Widget* hbox;
	Etk_Widget* ivbox;
	Etk_Widget* icon;
	Etk_Widget* label;
	Etk_Widget* pvbox;
	char* perms;

	/*First up, add a reference to this file*/
	printf("Adding reference to '%s/%s'- > %s\n", file->path, file->filename, file->md5);
	entropy_core_file_cache_add_reference(file->md5);

	/*Retrieve file permissions*/
	perms = entropy_generic_file_display_permissions_parse(file);

	dialog = entropy_malloc(sizeof(Entropy_Etk_Properties_Dialog));
	dialog->file = file;

	dialog->window = etk_window_new();
	etk_window_title_set(ETK_WINDOW(dialog->window), file->filename);

	etk_widget_size_request_set(dialog->window, 450, 520);


	vbox = etk_vbox_new(ETK_TRUE,0);
	etk_container_add(ETK_CONTAINER(dialog->window), vbox);

	notebook = etk_notebook_new();
	etk_box_pack_start(ETK_BOX(vbox), notebook, ETK_TRUE, ETK_TRUE, 0);

	/*Main page*/
	ivbox = etk_vbox_new(ETK_FALSE,0);	
	etk_notebook_page_append(ETK_NOTEBOOK(notebook), "General", ivbox);

	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_pack_start(ETK_BOX(ivbox), hbox, ETK_FALSE, ETK_FALSE, 0);

	if (file->thumbnail) {
		icon = etk_image_new_from_file(file->thumbnail->thumbnail_filename);
		etk_box_pack_start(ETK_BOX(hbox), icon, ETK_FALSE, ETK_FALSE, 0);
	}

	pvbox = etk_vbox_new(ETK_FALSE,0);
	etk_box_pack_start(ETK_BOX(hbox), pvbox , ETK_FALSE, ETK_FALSE, 0);

	label = etk_label_new(file->path);
	etk_box_pack_start(ETK_BOX(pvbox), label, ETK_FALSE, ETK_FALSE, 0);	
	
	label = etk_label_new(file->filename);
	etk_box_pack_start(ETK_BOX(pvbox), label, ETK_FALSE, ETK_FALSE, 0);
	/*------------------*/

	/*Permissions*/
	ivbox = etk_vbox_new(ETK_FALSE,0);	
	etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Permissions", ivbox);

	/*User*/
	label = etk_label_new("User");
	etk_box_pack_start(ETK_BOX(ivbox), label, ETK_FALSE, ETK_FALSE, 0);

	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_pack_start(ETK_BOX(ivbox), hbox, ETK_FALSE, ETK_FALSE, 0);

	button = etk_check_button_new_with_label("Read");
	if (perms && perms[1] == 'r') etk_button_click(ETK_BUTTON(button));
	etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);

	button = etk_check_button_new_with_label("Write");
	if (perms && perms[2] == 'w') etk_button_click(ETK_BUTTON(button));
	etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);

	button = etk_check_button_new_with_label("Execute");
	if (perms && perms[3] == 'x') etk_button_click(ETK_BUTTON(button));
	etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);

	/*Group*/
	label = etk_label_new("Group");
	etk_box_pack_start(ETK_BOX(ivbox), label, ETK_FALSE, ETK_FALSE, 0);

	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_pack_start(ETK_BOX(ivbox), hbox, ETK_FALSE, ETK_FALSE, 0);

	button = etk_check_button_new_with_label("Read");
	if (perms && perms[4] == 'r') etk_button_click(ETK_BUTTON(button));
	etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);

	button = etk_check_button_new_with_label("Write");
	if (perms && perms[5] == 'w') etk_button_click(ETK_BUTTON(button));
	etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);

	button = etk_check_button_new_with_label("Execute");
	if (perms && perms[6] == 'x') etk_button_click(ETK_BUTTON(button));
	etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);

	/*Other*/
	label = etk_label_new("Other");
	etk_box_pack_start(ETK_BOX(ivbox), label, ETK_FALSE, ETK_FALSE, 0);

	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_pack_start(ETK_BOX(ivbox), hbox, ETK_FALSE, ETK_FALSE, 0);

	button = etk_check_button_new_with_label("Read");
	if (perms && perms[7] == 'r') etk_button_click(ETK_BUTTON(button));
	etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);

	button = etk_check_button_new_with_label("Write");
	if (perms && perms[8] == 'w') etk_button_click(ETK_BUTTON(button));
	etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);

	button = etk_check_button_new_with_label("Execute");
	if (perms && perms[9] == 'x') etk_button_click(ETK_BUTTON(button));
	etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);
	
	/*---------------------*/

	hbox = etk_hbox_new(ETK_TRUE,0);
	etk_box_pack_end(ETK_BOX(vbox), hbox, ETK_FALSE, ETK_FALSE, 0);

	button = etk_button_new_with_label("OK");
	etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);

	button = etk_button_new_with_label("Apply");
	etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);

	button = etk_button_new_with_label("Cancel");
	etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);
	

	if (perms) entropy_free(perms);
	etk_widget_show_all(dialog->window);
}
