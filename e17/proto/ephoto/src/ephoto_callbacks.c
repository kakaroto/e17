#include "ephoto.h"

void destroy_cb(Ewl_Widget *w, void *event, void *data)
{
 if (!ecore_dlist_is_empty(current_thumbs))
	 ecore_dlist_destroy(current_thumbs);
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
 char full_path[PATH_MAX];
 Ecore_List *albums;
 Ewl_Widget *hbox;
 Ewl_Widget *image;
 Ewl_Widget *text;
 Ewl_Widget *children[2];
 Ewl_Widget *row;
 
 home = getenv("HOME");
 snprintf(path, PATH_MAX, "%s/.ephoto", home);

 if (!ecore_file_exists(path)) ecore_file_mkdir(path);

 albums = ecore_list_new(); 
 albums = ecore_file_ls(path);

 while(!ecore_list_is_empty(albums))
 {
  album = ecore_list_remove_first(albums);
  snprintf(full_path, PATH_MAX, "%s/%s", path, album);
  
  hbox = ewl_hbox_new();
  ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
  ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
  ewl_box_spacing_set(EWL_BOX(hbox), 5);
  ewl_widget_show(hbox);

  image = ewl_image_new();
  ewl_image_file_set(EWL_IMAGE(image), 
		     PACKAGE_DATA_DIR "/images/image.png", NULL);
  ewl_container_child_append(EWL_CONTAINER(hbox), image);
  ewl_widget_show(image);
  
  text = ewl_text_new();
  ewl_text_text_set(EWL_TEXT(text), album);
  ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
  ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_SHRINK);
  ewl_container_child_append(EWL_CONTAINER(hbox), text);
  ewl_widget_show(text);
 
  children[0] = hbox;
  children[1] = NULL;
  
  row = ewl_tree_row_add(EWL_TREE(m->albums), NULL, children);
  ewl_callback_append(row, EWL_CALLBACK_CLICKED, populate_images, NULL);
  ewl_widget_name_set(row, full_path);
 }
}

void populate_browser(Ewl_Widget *w, void *event, void *data)
{
 char *directory;
 char *file;
 char full_path[PATH_MAX];
 Ecore_List *ls;
 Ecore_List *directories;
 Ewl_Widget *hbox;
 Ewl_Widget *image;
 Ewl_Widget *text;
 Ewl_Widget *children[2];
 Ewl_Widget *row;
 
 directory = data;
 current_directory = strdup(directory);
 ewl_text_text_set(EWL_TEXT(m->entry), current_directory);
 
 ls = ecore_list_new();
 directories = ecore_list_new();
 
 ls = ecore_file_ls(directory);
 
 while (!ecore_list_is_empty(ls))
 {
  file = ecore_list_remove_first(ls);
  if (strcmp(directory, "/"))
  	snprintf(full_path, PATH_MAX, "%s/%s", directory, file);
  else
	snprintf(full_path, PATH_MAX, "%s%s", directory, file);
  if (ecore_file_is_dir(full_path) && strncmp(file, ".", 1)) 
	  ecore_list_append(directories, strdup(full_path));
 }

 while (!ecore_list_is_empty(directories))
 {
  file = ecore_list_remove_first(directories);
  
  hbox = ewl_hbox_new();
  ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
  ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
  ewl_box_spacing_set(EWL_BOX(hbox), 5);
  ewl_widget_show(hbox);

  image = ewl_image_new();
  ewl_image_file_set(EWL_IMAGE(image), 
		     PACKAGE_DATA_DIR "/images/folder.png", NULL);
  ewl_container_child_append(EWL_CONTAINER(hbox), image);
  ewl_widget_show(image);

  text = ewl_text_new();
  ewl_text_text_set(EWL_TEXT(text), basename(file));
  ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_ALL);
  ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
  ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_SHRINK);
  ewl_container_child_append(EWL_CONTAINER(hbox), text);
  ewl_widget_show(text);

  children[0] = hbox;
  children[1] = NULL;

  row = ewl_tree_row_add(EWL_TREE(m->browser), NULL, children);
  ewl_callback_append(row, EWL_CALLBACK_CLICKED, populate_images, NULL);
  ewl_widget_name_set(row, file);
 }
}

void populate_images(Ewl_Widget *w, void *event, void *data)
{
 FILE *file_ptr;
 const char *dir;
 char *image;
 char full_path[PATH_MAX];
 char text[PATH_MAX];
 Ecore_List *ls;
 Ecore_List *images;
 Ewl_Widget *shadow;
 Ewl_Widget *icon;
 
 ls = ecore_list_new();
 images = ecore_list_new();
 dir = ewl_widget_name_get(w);
 
 if (ecore_file_is_dir(dir))
 {
  ewl_container_reset(EWL_CONTAINER(m->browser));
  ewl_container_reset(EWL_CONTAINER(m->viewer_freebox));
  populate_browser(NULL, NULL, strdup((char *)dir));
  
  ls = ecore_file_ls(dir);
  while(!ecore_list_is_empty(ls))
  { 
   snprintf(full_path, PATH_MAX, "%s/%s", dir, ecore_list_remove_first(ls));
   if (fnmatch("*.[Jj][Pp][Ee][Gg]", full_path, 0) == 0)
   {
    ecore_list_append(images, strdup(full_path));
   }
   if (fnmatch("*.[Jj][Pp][Gg]", full_path, 0) == 0)
   {
    ecore_list_append(images, strdup(full_path));
   }
   if (fnmatch("*.[Pp][Nn][Gg]", full_path, 0) == 0)
   {
    ecore_list_append(images, strdup(full_path));
   }
   if (fnmatch("*.[Gg][Ii][Ff]", full_path, 0) == 0)
   {
    ecore_list_append(images, strdup(full_path));
   }
   if (fnmatch("*.[Ss][Vv][Gg]", full_path, 0) == 0)
   {
    ecore_list_append(images, strdup(full_path));
   }
  }
 }
 else
 {
  ewl_container_reset(EWL_CONTAINER(m->viewer_freebox));
  file_ptr = fopen(dir, "r");
  if (file_ptr != NULL)
  {
   while(fgets(text,PATH_MAX,file_ptr) != NULL)
   {
    if(strncmp(text, "#", 1)) 
    {
     text[strlen(text)-1] = '\0';
     ecore_list_append(images, strdup(text));
    }
   }
   fclose(file_ptr);
  }
 }
 if(!ecore_dlist_is_empty(current_thumbs)) 
 { 
  ecore_dlist_destroy(current_thumbs);
  current_thumbs = ecore_dlist_new();
 }
 while(!ecore_list_is_empty(images))
 {
  image = ecore_list_remove_first(images);
  ecore_dlist_append(current_thumbs, image);
  
  shadow = ewl_shadow_new();
  ewl_container_child_append(EWL_CONTAINER(m->viewer_freebox), shadow);
  ewl_object_minimum_size_set(EWL_OBJECT(shadow), 124, 124);
  ewl_object_maximum_size_set(EWL_OBJECT(shadow), 124, 124);
  ewl_widget_show(shadow);

  icon = ewl_image_thumbnail_new();
  ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(icon), image);
  ewl_container_child_append(EWL_CONTAINER(shadow), icon);
  ewl_image_size_set(EWL_IMAGE(icon), 100, 100);
  ewl_theme_data_str_set(icon, "/image_thumbnail/group",
		         ewl_theme_data_str_get(m->entry, "group"));
  ewl_callback_append(icon, EWL_CALLBACK_CLICKED, view_images, image);
  ewl_widget_show(icon);
 }
 ecore_list_destroy(ls);
 ecore_list_destroy(images);
}

void go_up(Ewl_Widget *w, void *event, void *data)
{
 char *new_dir;
 if (strcmp(current_directory, "/"))
 {
  new_dir = dirname(current_directory);
  ewl_widget_name_set(m->viewer, new_dir);
  populate_images(m->viewer, NULL, NULL);
 }
}

void go_home(Ewl_Widget *w, void *event, void *data)
{
 char *new_dir;
 new_dir = getenv("HOME");
 ewl_widget_name_set(m->viewer, new_dir);
 populate_images(m->viewer, NULL, NULL);
}

void entry_change(Ewl_Widget *w, void *event, void *data)
{
 char *new_dir;
 new_dir = ewl_text_text_get(EWL_TEXT(w));
 if (ecore_file_is_dir(new_dir))
 {
  ewl_widget_name_set(m->viewer, new_dir);
  populate_images(m->viewer, NULL, NULL);
 }
 else
 {
  ewl_text_text_set(EWL_TEXT(m->entry), current_directory);
 }
}
