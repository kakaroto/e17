#include "ephoto.h"
Ecore_Timer *timer;

int change_picture(void *data)
{
 char *image_path;
 Ewl_Widget *w;
 
 w = data;
 ecore_list_next(current_thumbs);
 image_path = ecore_list_current(current_thumbs);
 if(image_path)
 {
  ewl_image_file_set(EWL_IMAGE(w), image_path, NULL);
 }
 else 
 {
  ecore_timer_del(timer);
  ewl_widget_destroy(w->parent->parent);
 }
}
 
void start_slideshow(Ewl_Widget *w, void *event, void *data)
{
 Ewl_Widget *window;
 Ewl_Widget *vbox;
 Ewl_Widget *image;
 char *image_path;
 
 image_path = ecore_list_goto_first(current_thumbs);
 
 if (!image_path) return;
 
 window = ewl_window_new();
 ewl_window_fullscreen_set(EWL_WINDOW(window), 1);
 ewl_widget_show(window);

 vbox = ewl_vbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(window), vbox);
 ewl_widget_show(vbox);

 image = ewl_image_new();
 ewl_image_file_set(EWL_IMAGE(image), image_path, NULL);
 ewl_object_fill_policy_set(EWL_OBJECT(image), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(vbox), image);
 ewl_widget_show(image);

 timer = ecore_timer_add(3, change_picture, image);
}
