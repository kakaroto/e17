#include <Etk.h>
#include "entropy.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct _About_Dialog About_Dialog;

About_Dialog *about_dialog_new();

struct _About_Dialog
{
   Entropy_Generic_File* file;
	
   /* Main window */
   struct{
      Etk_Widget *dialog;     /* holds everything */
      Etk_Widget *outer_vbox; /* has two rows */
      Etk_Widget *top_hbox;   /* has two cols */
      Etk_Widget *top_vbox;   /* has two rows */
   } main;
   
   /* General frame */
   struct {      
      Etk_Widget *frame;
      Etk_Widget *table;
      Etk_Widget *file_label;
      Etk_Widget *file_entry;
      Etk_Widget *size_label;
      Etk_Widget *size_entry;
      Etk_Widget *lastm_label;
      Etk_Widget *lastm_entry;
      Etk_Widget *ftype_label;
      Etk_Widget *ftype_entry;   
   } general;
   
   /* Permissions frame */
   struct {
      Etk_Widget *frame;
      Etk_Widget *table;   
      Etk_Widget *owner_label;
      Etk_Widget *owner_entry;
      Etk_Widget *others_read_cbox;
      Etk_Widget *others_write_cbox;
      Etk_Widget *owner_read_cbox;
      Etk_Widget *owner_write_cbox;
   } perm;
   
   /* Preview frame */
   struct {
      Etk_Widget *frame;
      Etk_Widget *image;
   } preview;
   
   /* Icon frame */
   struct {
      Etk_Widget *frame;
      Etk_Widget *vbox;
      Etk_Widget *table;
      Etk_Widget *image;
      Etk_Widget *thumb_radio;
      Etk_Widget *default_radio;
      Etk_Widget *edje_radio;
      Etk_Widget *image_radio;      
      Etk_Widget *use_for_all_cbox;
   } icon;
};

Etk_Bool _etk_about_dialog_window_deleted_cb (Etk_Object * object, void *data) 
{
	About_Dialog* dialog= data;

	/*Remove the file reference*/
	entropy_core_file_cache_remove_reference(dialog->file->md5);

	etk_object_destroy(ETK_OBJECT(dialog->main.dialog));
	entropy_free(dialog);

	return ETK_TRUE;
}

/**
 * Creates and about dialog and returns it
 * @return returns an about dialog
 */
About_Dialog *about_dialog_new()
{
   About_Dialog *ad;
   Etk_Widget *button;
   
   ad = calloc(1, sizeof(About_Dialog));
   
   /* create the main window, add the boxes */
   ad->main.dialog = etk_dialog_new();
   ad->main.outer_vbox = etk_vbox_new(ETK_FALSE, 0);
   ad->main.top_hbox = etk_hbox_new(ETK_FALSE, 0);
   ad->main.top_vbox = etk_vbox_new(ETK_FALSE, 0);
   
   /* position the boxes properly */
   etk_dialog_pack_in_main_area(ETK_DIALOG(ad->main.dialog), 
				ad->main.outer_vbox, 
				ETK_BOX_START,ETK_BOX_EXPAND_FILL,0);
   etk_box_append(ETK_BOX(ad->main.outer_vbox), ad->main.top_hbox, 
		  ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_box_append(ETK_BOX(ad->main.top_hbox), ad->main.top_vbox,
		  ETK_BOX_START, ETK_BOX_NONE, 0);
   
   /* create General frame, position its widgets internally, add it */
   ad->general.frame = etk_frame_new("General");
   ad->general.table = etk_table_new(2, 4, ETK_FALSE);
   ad->general.file_label = etk_label_new("File:");
   ad->general.file_entry = etk_entry_new();
   ad->general.size_label = etk_label_new("Size:");
   ad->general.size_entry = etk_entry_new();
   ad->general.lastm_label = etk_label_new("Last Modified:");
   ad->general.lastm_entry = etk_entry_new();
   ad->general.ftype_label = etk_label_new("File Type:");
   ad->general.ftype_entry = etk_entry_new();
   etk_table_attach_default(ETK_TABLE(ad->general.table), 
			    ad->general.file_label,
			    0, 0, 0, 0);
   etk_table_attach_default(ETK_TABLE(ad->general.table), 
			    ad->general.file_entry,
			    1, 1, 0, 0);
   etk_table_attach_default(ETK_TABLE(ad->general.table), 
			    ad->general.size_label,
			    0, 0, 1, 1);
   etk_table_attach_default(ETK_TABLE(ad->general.table), 
			    ad->general.size_entry,
			    1, 1, 1, 1);
   etk_table_attach_default(ETK_TABLE(ad->general.table), 
			    ad->general.lastm_label,
			    0, 0, 2, 2);
   etk_table_attach_default(ETK_TABLE(ad->general.table), 
			    ad->general.lastm_entry,
			    1, 1, 2, 2);
   etk_table_attach_default(ETK_TABLE(ad->general.table), 
			    ad->general.ftype_label,
			    0, 0, 3, 3);
   etk_table_attach_default(ETK_TABLE(ad->general.table), 
			    ad->general.ftype_entry,
			    1, 1, 3, 3);
   etk_container_add(ETK_CONTAINER(ad->general.frame), ad->general.table);
   etk_box_append(ETK_BOX(ad->main.top_vbox), ad->general.frame,
		  ETK_BOX_START, ETK_BOX_NONE, 0);
   
   /* create Permissions frame, position widgets internally, add it */
   ad->perm.frame = etk_frame_new("Permissions");
   ad->perm.table = etk_table_new(2, 3, ETK_FALSE);
   ad->perm.owner_label = etk_label_new("Owner:");
   ad->perm.owner_entry = etk_entry_new();
   ad->perm.others_read_cbox = etk_check_button_new_with_label("Other can read");
   ad->perm.others_write_cbox = etk_check_button_new_with_label("Other can write");
   ad->perm.owner_read_cbox = etk_check_button_new_with_label("Owner can read");
   ad->perm.owner_write_cbox = etk_check_button_new_with_label("Owner can write");
   etk_table_attach_default(ETK_TABLE(ad->perm.table), 
			    ad->perm.owner_label,
			    0, 0, 0, 0);
   etk_table_attach_default(ETK_TABLE(ad->perm.table), 
			    ad->perm.owner_entry,
			    1, 1, 0, 0);
   etk_table_attach_default(ETK_TABLE(ad->perm.table), 
			    ad->perm.others_read_cbox,
			    0, 0, 1, 1);
   etk_table_attach_default(ETK_TABLE(ad->perm.table), 
			    ad->perm.owner_read_cbox,
			    1, 1, 1, 1);
   etk_table_attach_default(ETK_TABLE(ad->perm.table), 
			    ad->perm.others_write_cbox,
			    0, 0, 2, 2);
   etk_table_attach_default(ETK_TABLE(ad->perm.table), 
			    ad->perm.owner_write_cbox,
			    1, 1, 2, 2);
   etk_container_add(ETK_CONTAINER(ad->perm.frame), ad->perm.table);
   etk_box_append(ETK_BOX(ad->main.top_vbox), ad->perm.frame,
		  ETK_BOX_START, ETK_BOX_NONE, 0);
   
   /* create Preview frame, position widgets internally, add it */
   ad->preview.frame = etk_frame_new("Preview");
   ad->preview.image = etk_image_new();
   etk_container_add(ETK_CONTAINER(ad->preview.frame), ad->preview.image);
   etk_box_append(ETK_BOX(ad->main.top_hbox), ad->preview.frame,
		  ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   /* create Icon frame, position widgets itnernally, add it */
   ad->icon.frame = etk_frame_new("Icon");
   ad->icon.table = etk_table_new(3, 3, ETK_FALSE);
   ad->icon.vbox = etk_vbox_new(ETK_FALSE, 0);
   ad->icon.image = etk_image_new();
   ad->icon.thumb_radio = etk_radio_button_new_with_label("Thumbnail", NULL);
   ad->icon.default_radio = etk_radio_button_new_with_label_from_widget("Default", ETK_RADIO_BUTTON(ad->icon.thumb_radio));
   ad->icon.edje_radio = etk_radio_button_new_with_label_from_widget("Edje", ETK_RADIO_BUTTON(ad->icon.thumb_radio));
   ad->icon.image_radio = etk_radio_button_new_with_label_from_widget("Image", ETK_RADIO_BUTTON(ad->icon.thumb_radio));
   ad->icon.use_for_all_cbox = etk_check_button_new_with_label("Use this icon for files of this type");
   etk_table_attach_default(ETK_TABLE(ad->icon.table),
			    ad->icon.image,
			    0, 0, 0, 2);
   etk_table_attach_default(ETK_TABLE(ad->icon.table),
			    ad->icon.thumb_radio,
			    1, 1, 0, 0);
   etk_table_attach_default(ETK_TABLE(ad->icon.table),
			    ad->icon.default_radio,
			    1, 1, 1, 1);
   etk_table_attach_default(ETK_TABLE(ad->icon.table),
			    ad->icon.image_radio,
			    2, 2, 0, 0);
   etk_table_attach_default(ETK_TABLE(ad->icon.table),
			    ad->icon.edje_radio,
			    2, 2, 1, 1);   
   etk_container_add(ETK_CONTAINER(ad->icon.frame), ad->icon.vbox);   
   etk_box_append(ETK_BOX(ad->icon.vbox), ad->icon.table,
		  ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_box_append(ETK_BOX(ad->icon.vbox), ad->icon.use_for_all_cbox,
		  ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_box_append(ETK_BOX(ad->main.outer_vbox), ad->icon.frame,
		  ETK_BOX_START, ETK_BOX_NONE, 0);

   /* add buttons */
   etk_dialog_has_separator_set(ETK_DIALOG(ad->main.dialog), ETK_TRUE);
   
   button = etk_dialog_button_add(ETK_DIALOG(ad->main.dialog), "Ok", ETK_RESPONSE_OK);
   etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_etk_about_dialog_window_deleted_cb), ad);
   etk_widget_size_request_set(button, 64, -1);
   
   button = etk_dialog_button_add(ETK_DIALOG(ad->main.dialog), "Apply", ETK_RESPONSE_APPLY);
   etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_etk_about_dialog_window_deleted_cb), ad);
   etk_widget_size_request_set(button, 64, -1);   
   
   button = etk_dialog_button_add(ETK_DIALOG(ad->main.dialog), "Cancel", ETK_RESPONSE_CANCEL);
   etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_etk_about_dialog_window_deleted_cb), ad);
   etk_widget_size_request_set(button, 64, -1);   

   
   etk_container_border_width_set(ETK_CONTAINER(ad->main.dialog), 10);
   etk_window_title_set(ETK_WINDOW(ad->main.dialog), "File Properties");
   return ad;
}

void etk_properties_dialog_new(Entropy_Generic_File* file)
{
   About_Dialog *ad;
   char buf[1024];
   time_t stime;

   ad = about_dialog_new();
   ad->file = file;
   
   
   if(ad && ad->main.dialog) {
     entropy_core_file_cache_add_reference(file->md5);
	   
     snprintf(buf,sizeof(buf),"%s/%s", ad->file->path,ad->file->filename);

     /*Setup the display*/
     etk_entry_text_set(ETK_ENTRY(ad->general.file_entry), buf);

     if (file->thumbnail) {
	     etk_image_set_from_file(ETK_IMAGE(ad->preview.image),
 			   file->thumbnail->thumbnail_filename,
			   NULL);
     } else {
	     etk_image_set_from_file(ETK_IMAGE(ad->preview.image),
 			   PACKAGE_DATA_DIR "/icons/default.png",
			   NULL);	     
     }
     snprintf(buf,50,"%lld kb", file->properties.st_size / 1024);
     etk_entry_text_set(ETK_ENTRY(ad->general.size_entry), buf);

     stime = file->properties.st_atime;
     etk_entry_text_set(ETK_ENTRY(ad->general.lastm_entry), ctime(&stime));
     etk_entry_text_set(ETK_ENTRY(ad->general.ftype_entry), file->mime_type); 
		     
	   
     etk_widget_show_all(ad->main.dialog);
   }
}

