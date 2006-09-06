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
 char full_path[PATH_MAX];
 const char *image_path;
 Ecore_List *albums;
 Ewl_Widget *hbox;
 Ewl_Widget *image;
 Ewl_Widget *text;
 Ewl_Widget *children[1];
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

  image_path = ewl_icon_theme_icon_path_get(EWL_ICON_IMAGE_X_GENERIC,
                                            EWL_ICON_SIZE_LARGE);  
  image = ewl_image_new();
  ewl_image_file_set(EWL_IMAGE(image), image_path, NULL);
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
  ewl_callback_append(row, EWL_CALLBACK_CLICKED, populate_images, NULL);
  ewl_widget_name_set(row, full_path);
 }
}

void populate_browser(Ewl_Widget *w, void *event, void *data)
{
 char *directory;
 char *file;
 char full_path[PATH_MAX];
 const char *image_path;
 Ecore_List *ls;
 Ecore_List *directories;
 Ewl_Widget *hbox;
 Ewl_Widget *image;
 Ewl_Widget *text;
 Ewl_Widget *children[1];
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

  image_path = ewl_icon_theme_icon_path_get(EWL_ICON_FOLDER, 
	                                    EWL_ICON_SIZE_LARGE);
  image = ewl_image_new();
  ewl_image_file_set(EWL_IMAGE(image), image_path, NULL);
  ewl_container_child_append(EWL_CONTAINER(hbox), image);
  ewl_widget_show(image);

  text = ewl_text_new();
  ewl_text_text_set(EWL_TEXT(text), basename(file));
  ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_ALL);
  ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
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
 
 while(!ecore_list_is_empty(images))
 {
  image = ecore_list_remove_first(images);
  
  m->icon = ewl_image_thumbnail_new();
  ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(m->icon), image);
  ewl_container_child_append(EWL_CONTAINER(m->viewer_freebox), m->icon);
  ewl_widget_show(m->icon);
 }
}

void go_up(Ewl_Widget *w, void *event, void *data)
{
 char *new_dir;
 if (strcmp(current_directory, "/"))
 {
  new_dir = dirname(current_directory);
  ewl_widget_name_set(m->viewer_freebox, new_dir);
  populate_images(m->viewer_freebox, NULL, NULL);
 }
}

void go_home(Ewl_Widget *w, void *event, void *data)
{
 char *new_dir;
 new_dir = getenv("HOME");
 ewl_widget_name_set(m->viewer_freebox, new_dir);
 populate_images(m->viewer_freebox, NULL, NULL);
}
