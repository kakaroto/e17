#include "ephoto.h"

void ok_album(Ewl_Widget *w, void *event, void *data);
void ok_slideshow(Ewl_Widget *w, void *event, void *data);
void cancel(Ewl_Widget *w, void *event, void *data);

void destroy_cb(Ewl_Widget *w, void *event, void *data)
{
 ewl_widget_destroy(w);
 ewl_main_quit();
 w = NULL;
 event = NULL;
 data = NULL;
 return;
}

void ok_album(Ewl_Widget *w, void *event, void *data)
{
 char *home;
 char *entry_text;
 char album_path[PATH_MAX];
 Ewl_Widget *win;
 Ewl_Widget *vbox;
 Ewl_Widget *icon;
 Ewl_Widget *text;
 Ewl_Widget *button;
 sqlite3 *db;
 
 home = getenv("HOME");
 entry_text = ewl_text_text_get(EWL_TEXT(data));

 snprintf(album_path, PATH_MAX, "%s/.ephoto/albums/%s", home, entry_text);
 
 if (strcmp(entry_text, " ") != 0)
 {
  if (!ecore_file_exists(album_path))
  {
   sqlite3_open(album_path, &db);
   sqlite3_exec(db, "create table images(images varchar(255));", NULL, 0, 0);
   sqlite3_close(db);

   m->icon = ewl_icon_new();
   ewl_icon_label_set(EWL_ICON(m->icon), entry_text);
   ewl_object_alignment_set(EWL_OBJECT(m->icon), EWL_FLAG_ALIGN_CENTER);
   ewl_callback_append(m->icon, EWL_CALLBACK_CLICKED, album_clicked_cb, NULL);
   ewl_container_child_append(EWL_CONTAINER(m->albums), m->icon);
   ewl_widget_show(m->icon);
  }
 
  else
  {
   win = ewl_window_new();
   ewl_window_title_set(EWL_WINDOW(win), "Whoops");
   ewl_window_name_set(EWL_WINDOW(win), "Whoops");
   ewl_object_size_request(EWL_OBJECT(win), 243, 50);
   ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, cancel, win);
   ewl_widget_show(win);
  
   vbox = ewl_vbox_new();
   ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
   ewl_container_child_append(EWL_CONTAINER(win), vbox);
   ewl_widget_show(vbox);
  
   text = ewl_text_new();
   ewl_text_text_set(EWL_TEXT(text), "Whoops! This album already exists!");
   ewl_container_child_append(EWL_CONTAINER(vbox), text);
   ewl_widget_show(text);

   button = ewl_button_new();
   ewl_button_label_set(EWL_BUTTON(button), "Ok");
   ewl_container_child_append(EWL_CONTAINER(vbox), button);
   ewl_callback_append(button, EWL_CALLBACK_CLICKED, cancel, win);
   ewl_widget_show(button);
  }
 }
 else
 {
  win = ewl_window_new();
  ewl_window_title_set(EWL_WINDOW(win), "Whoops");
  ewl_window_name_set(EWL_WINDOW(win), "Whoops");
  ewl_object_size_request(EWL_OBJECT(win), 243, 50);
  ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, cancel, win);
  ewl_widget_show(win);

  vbox = ewl_vbox_new();
  ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
  ewl_container_child_append(EWL_CONTAINER(win), vbox);
  ewl_widget_show(vbox);

  text = ewl_text_new();
  ewl_text_text_set(EWL_TEXT(text), "Whoops! Bad album name!!");
  ewl_container_child_append(EWL_CONTAINER(vbox), text);
  ewl_widget_show(text);
 
  button = ewl_button_new();
  ewl_button_label_set(EWL_BUTTON(button), "Ok");
  ewl_container_child_append(EWL_CONTAINER(vbox), button);
  ewl_callback_append(button, EWL_CALLBACK_CLICKED, cancel, win);
  ewl_widget_show(button);
 }

 w = NULL;
 event = NULL;
 data = NULL;
 return;
}

void ok_slideshow(Ewl_Widget *w, void *event, void *data)
{
 char *home;
 char *entry_text;
 char slideshow_path[PATH_MAX];
 Ewl_Widget *win;
 Ewl_Widget *vbox;
 Ewl_Widget *icon;
 Ewl_Widget *text;
 Ewl_Widget *button;
 sqlite3 *db;

 home = getenv("HOME");
 entry_text = ewl_text_text_get(EWL_TEXT(data));

 snprintf(slideshow_path, PATH_MAX, "%s/.ephoto/slideshows/%s", home, entry_text);

 if (strcmp(entry_text, " ") != 0)
 {
  if (!ecore_file_exists(slideshow_path))
  {
   sqlite3_open(slideshow_path, &db);
   sqlite3_exec(db, "create table slideshows(images varchar(255), settings varchar(255));", NULL, 0, 0);
   sqlite3_close(db);

   m->icon = ewl_icon_new();
   ewl_icon_label_set(EWL_ICON(m->icon), entry_text);
   ewl_object_alignment_set(EWL_OBJECT(m->icon), EWL_FLAG_ALIGN_CENTER);
   ewl_callback_append(icon, EWL_CALLBACK_CLICKED, slideshow_clicked_cb, NULL);
   ewl_container_child_append(EWL_CONTAINER(m->slideshows), m->icon);
   ewl_widget_show(m->icon);
  }

  else
  {
   win = ewl_window_new();
   ewl_window_title_set(EWL_WINDOW(win), "Whoops");
   ewl_window_name_set(EWL_WINDOW(win), "Whoops");
   ewl_object_size_request(EWL_OBJECT(win), 243, 50);
   ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, cancel, win);
   ewl_widget_show(win);

   vbox = ewl_vbox_new();
   ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
   ewl_container_child_append(EWL_CONTAINER(win), vbox);
   ewl_widget_show(vbox);

   text = ewl_text_new();
   ewl_text_text_set(EWL_TEXT(text), "Whoops! This slideshow already exists!");
   ewl_container_child_append(EWL_CONTAINER(vbox), text);
   ewl_widget_show(text);

   button = ewl_button_new();
   ewl_button_label_set(EWL_BUTTON(button), "Ok");
   ewl_container_child_append(EWL_CONTAINER(vbox), button);
   ewl_callback_append(button, EWL_CALLBACK_CLICKED, cancel, win);
   ewl_widget_show(button);
  }
 }
 else
 {
  win = ewl_window_new();
  ewl_window_title_set(EWL_WINDOW(win), "Whoops");
  ewl_window_name_set(EWL_WINDOW(win), "Whoops");
  ewl_object_size_request(EWL_OBJECT(win), 243, 50);
  ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, cancel, win);
  ewl_widget_show(win);

  vbox = ewl_vbox_new();
  ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
  ewl_container_child_append(EWL_CONTAINER(win), vbox);
  ewl_widget_show(vbox);

  text = ewl_text_new();
  ewl_text_text_set(EWL_TEXT(text), "Whoops! Bad slideshow name!!");
  ewl_container_child_append(EWL_CONTAINER(vbox), text);
  ewl_widget_show(text);

  button = ewl_button_new();
  ewl_button_label_set(EWL_BUTTON(button), "Ok");
  ewl_container_child_append(EWL_CONTAINER(vbox), button);
  ewl_callback_append(button, EWL_CALLBACK_CLICKED, cancel, win);
  ewl_widget_show(button);
 }

 w = NULL;
 event = NULL;
 data = NULL;
 return;
}


void cancel(Ewl_Widget *w, void *event, void *data)
{
 ewl_widget_destroy(EWL_WIDGET(data));
 w = NULL;
 event = NULL;
 data = NULL;
 return;
}

void add_album_cb(Ewl_Widget *w, void *event, void *data)
{
 Ewl_Widget *win;
 Ewl_Widget *vbox;
 Ewl_Widget *text;
 Ewl_Widget *entry;
 Ewl_Widget *hbox;
 Ewl_Widget *button;

 win = ewl_window_new();
 ewl_window_title_set(EWL_WINDOW(win), "Create an album");
 ewl_window_name_set(EWL_WINDOW(win), "Create an album");
 ewl_object_size_request(EWL_OBJECT(win), 243, 73);
 ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, cancel, win);
 ewl_widget_show(win);

 vbox = ewl_vbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(win), vbox);
 ewl_widget_show(vbox);

 text = ewl_text_new();
 ewl_text_text_set(EWL_TEXT(text), "Enter the name of the album you wish to create");
 ewl_container_child_append(EWL_CONTAINER(vbox), text);
 ewl_widget_show(text);

 entry = ewl_entry_new();
 ewl_container_child_append(EWL_CONTAINER(vbox), entry);
 ewl_widget_show(entry);

 hbox = ewl_hbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
 ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
 ewl_widget_show(hbox);

 button = ewl_button_new();
 ewl_button_label_set(EWL_BUTTON(button), "Ok");
 ewl_container_child_append(EWL_CONTAINER(hbox), button);
 ewl_callback_append(button, EWL_CALLBACK_CLICKED, ok_album, entry);
 ewl_widget_show(button);

 button = ewl_button_new();
 ewl_button_label_set(EWL_BUTTON(button), "Cancel");
 ewl_container_child_append(EWL_CONTAINER(hbox), button);
 ewl_callback_append(button, EWL_CALLBACK_CLICKED, cancel, win);
 ewl_widget_show(button);

 w = NULL;
 event = NULL;
 data = NULL;
 return;
}

void add_slideshow_cb(Ewl_Widget *w, void *event, void *data)
{
 Ewl_Widget *win;
 Ewl_Widget *vbox;
 Ewl_Widget *text;
 Ewl_Widget *entry;
 Ewl_Widget *hbox;
 Ewl_Widget *button;

 win = ewl_window_new();
 ewl_window_title_set(EWL_WINDOW(win), "Create a slideshow");
 ewl_window_name_set(EWL_WINDOW(win), "Create an slideshow");
 ewl_object_size_request(EWL_OBJECT(win), 243, 73);
 ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, cancel, win);
 ewl_widget_show(win);

 vbox = ewl_vbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(win), vbox);
 ewl_widget_show(vbox);

 text = ewl_text_new();
 ewl_text_text_set(EWL_TEXT(text), "Enter the name of the slideshow you wish to create");
 ewl_container_child_append(EWL_CONTAINER(vbox), text);
 ewl_widget_show(text);

 entry = ewl_entry_new();
 ewl_container_child_append(EWL_CONTAINER(vbox), entry);
 ewl_widget_show(entry);

 hbox = ewl_hbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
 ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
 ewl_widget_show(hbox);

 button = ewl_button_new();
 ewl_button_label_set(EWL_BUTTON(button), "Ok");
 ewl_container_child_append(EWL_CONTAINER(hbox), button);
 ewl_callback_append(button, EWL_CALLBACK_CLICKED, ok_slideshow, entry);
 ewl_widget_show(button);

 button = ewl_button_new();
 ewl_button_label_set(EWL_BUTTON(button), "Cancel");
 ewl_container_child_append(EWL_CONTAINER(hbox), button);
 ewl_callback_append(button, EWL_CALLBACK_CLICKED, cancel, win);
 ewl_widget_show(button);

 w = NULL;
 event = NULL;
 data = NULL;
 return;
}

void album_clicked_cb(Ewl_Widget *w, void *event, void *data)
{
 w = NULL;
 event = NULL;
 data = NULL;
 return;
}

void slideshow_clicked_cb(Ewl_Widget *w, void *event, void *data)
{
 w = NULL;
 event = NULL;
 data = NULL;
 return;
}
