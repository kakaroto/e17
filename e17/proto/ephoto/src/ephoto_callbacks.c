#include "ephoto.h"

void destroy_cb(Ewl_Widget *w, void *event, void *data)
{
 ewl_widget_destroy(w);
 ewl_main_quit();
 w = NULL;
 event = NULL;
 data = NULL;
 return;
}

void populate_albums(Ewl_Widget *w, void *event, void *data)
{
 char *album;
 char *home;
 char path[PATH_MAX];
 Ecore_List *albums;
 Ewl_Widget *hbox;
 Ewl_Widget *image;
 Ewl_Widget *text;
 Ewl_Widget *children[1];
 Ewl_Widget *row;
 
 home = getenv("HOME");
 snprintf(path, PATH_MAX, "%s/.ephoto/", home);

 if (!ecore_file_exists(path)) ecore_file_mkdir(path);

 albums = ecore_list_new(); 
 albums = ecore_file_ls(path);

 while(!ecore_list_is_empty(albums))
 {
  album = ecore_list_remove_first(albums);
  
  hbox = ewl_hbox_new();
  ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
  ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
  ewl_box_spacing_set(EWL_BOX(hbox), 5);
  ewl_widget_show(hbox);
								
  image = ewl_image_new();
  ewl_container_child_append(EWL_CONTAINER(hbox), image);
  ewl_widget_show(image);
										  text = ewl_text_new();
  ewl_text_text_set(EWL_TEXT(text), album);
  ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_ALL);
  ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
  ewl_container_child_append(EWL_CONTAINER(hbox), text);
  ewl_widget_show(text);
 
  children[0] = hbox;
  children[1] = NULL;
  
  row = ewl_tree_row_add(EWL_TREE(m->albums), NULL, children);
  ewl_widget_name_set(row, album);
  //ewl_callback_append(row, EWL_CALLBACK_CLICKED, NULL, NULL);
 }
}

void populate_browser(Ewl_Widget *w, void *event, void *data)
{
 char *directory;
 char *file;
 Ecore_List *files;
 Ewl_Widget *hbox;
 Ewl_Widget *image;
 Ewl_Widget *text;
 Ewl_Widget *children[1];
 Ewl_Widget *row;
 
 directory = data;
 
 files = ecore_list_new();
 files = ecore_file_ls(directory);
 
 while (!ecore_list_is_empty(files))
 {
  file = ecore_list_remove_first(files);

  hbox = ewl_hbox_new();
  ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
  ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
  ewl_box_spacing_set(EWL_BOX(hbox), 5);
  ewl_widget_show(hbox);

  image = ewl_image_new();
  ewl_container_child_append(EWL_CONTAINER(hbox), image);
  ewl_widget_show(image);

  text = ewl_text_new();
  ewl_text_text_set(EWL_TEXT(text), file);
  ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_ALL);
  ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
  ewl_container_child_append(EWL_CONTAINER(hbox), text);
  ewl_widget_show(text);

  children[0] = hbox;
  children[1] = NULL;

  row = ewl_tree_row_add(EWL_TREE(m->browser), NULL, children);
  ewl_widget_name_set(row, file);
 }
}
