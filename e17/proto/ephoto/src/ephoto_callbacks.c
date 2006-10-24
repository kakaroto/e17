#include "ephoto.h"
Ewl_Widget *entry_t, *entry_d;

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

void populate_browser(Ewl_Widget *w, void *event, void *data)
{
 char *directory;
 char *file;
 char full_path[PATH_MAX];
 Ecore_List *ls;
 Ecore_List *directories;
 Ewl_Widget *hbox, *image, *text, *children[2], *row;

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
 char *image_path;
 char full_path[PATH_MAX];
 char text[PATH_MAX];
 Ecore_List *ls;
 Ecore_List *images;
 Ewl_Widget *image;
 Ewl_Widget *shadow;
 
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
 if(!ecore_dlist_is_empty(current_thumbs)) 
 { 
  ecore_dlist_destroy(current_thumbs);
  current_thumbs = ecore_dlist_new();
 }
 while(!ecore_list_is_empty(images))
 {
  image_path = ecore_list_remove_first(images);
  ecore_dlist_append(current_thumbs, image_path);

  shadow = ewl_shadow_new();
  ewl_container_child_append(EWL_CONTAINER(m->viewer_freebox), shadow);
  ewl_object_minimum_size_set(EWL_OBJECT(shadow), 110, 90);
  ewl_object_maximum_size_set(EWL_OBJECT(shadow), 110, 90);
  ewl_callback_append(shadow, EWL_CALLBACK_CLICKED, view_images, image_path);
  ewl_widget_show(shadow);
  
  image = ewl_image_thumbnail_new();
  ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(image), image_path);
  ewl_container_child_append(EWL_CONTAINER(shadow), image);
  ewl_image_size_set(EWL_IMAGE(image), 97, 83);
  ewl_image_proportional_set(EWL_IMAGE(image), TRUE);
  ewl_theme_data_str_set(image, "/image_thumbnail/file",
                         PACKAGE_DATA_DIR "/images/image_bg.edj");
  ewl_theme_data_str_set(image, "/image_thumbnail/group", "image_bg");
  ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
  ewl_widget_show(image);
 }
 ewl_widget_configure(m->albums);
 ewl_widget_configure(m->browser);
 ewl_widget_configure(m->viewer_freebox);
 ewl_widget_configure(m->viewer);
 ecore_list_destroy(ls);
 ecore_list_destroy(images);
}

void destroy_album_win(Ewl_Widget *w, void *event, void *data)
{
 Ewl_Widget *win;
 win = data;
 ewl_widget_destroy(win);
}

void save_album(Ewl_Widget *w, void *event, void *data)
{
 char *title, *description;
 Ewl_Widget *win;
 sqlite3 *db;

 title = ewl_text_text_get(EWL_TEXT(entry_t));
 description = ewl_text_text_get(EWL_TEXT(entry_d));

 db = ephoto_db_init();
 ephoto_db_add_album(db, title, description);
 ephoto_db_close(db);

 win = data;
 ewl_widget_destroy(win);

 return;
}

void add_album(Ewl_Widget *w, void *event, void *data)
{
 Ewl_Widget *win, *vbox, *hbox, *text, *button;
 
 win = ewl_window_new();
 ewl_window_title_set(EWL_WINDOW(win), _("Add an Album"));
 ewl_window_name_set(EWL_WINDOW(win), "Add an Album");
 ewl_object_size_request(EWL_OBJECT(win), 215, 150);
 ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, destroy_album_win, win);
 ewl_widget_show(win);

 vbox = ewl_vbox_new();
 ewl_container_child_append(EWL_CONTAINER(win), vbox);
 ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
 ewl_object_alignment_set(EWL_OBJECT(vbox), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(vbox);

 text = ewl_text_new();
 ewl_text_text_set(EWL_TEXT(text), _("Enter the new album name:"));
 ewl_container_child_append(EWL_CONTAINER(vbox), text);
 ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(text);

 entry_t = ewl_entry_new();
 ewl_container_child_append(EWL_CONTAINER(vbox), entry_t);
 ewl_object_alignment_set(EWL_OBJECT(entry_t), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(entry_t);

 text = ewl_text_new();
 ewl_text_text_set(EWL_TEXT(text), _("Enter the new album description:"));
 ewl_container_child_append(EWL_CONTAINER(vbox), text);
 ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(text);

 entry_d = ewl_entry_new();
 ewl_container_child_append(EWL_CONTAINER(vbox), entry_d);
 ewl_object_alignment_set(EWL_OBJECT(entry_d), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(entry_d);

 hbox = ewl_hbox_new();
 ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(hbox);

 button = ewl_button_new();
 ewl_button_image_set(EWL_BUTTON(button),
		      PACKAGE_DATA_DIR "/images/stock_save.png", NULL);
 ewl_button_label_set(EWL_BUTTON(button), _("save"));
 ewl_container_child_append(EWL_CONTAINER(hbox), button);
 ewl_callback_append(button, EWL_CALLBACK_CLICKED, save_album, win);
 ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(button);

 button = ewl_button_new();
 ewl_button_image_set(EWL_BUTTON(button),
		      PACKAGE_DATA_DIR "/images/dialog-close.png", NULL);
 ewl_button_label_set(EWL_BUTTON(button), _("cancel"));
 ewl_container_child_append(EWL_CONTAINER(hbox), button);
 ewl_callback_append(button, EWL_CALLBACK_CLICKED, destroy_album_win, win);
 ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(button);
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
