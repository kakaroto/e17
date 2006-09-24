#include "ephoto.h"
Ewl_Widget *vwin;
Ewl_Widget *image_view;
Ewl_Widget *image_vbox;

void destroy_vwin(Ewl_Widget *w, void *event, void *data)
{
 ewl_widget_destroy(w);
}

void change_image(Ewl_Widget *w, void *event, void *data)
{
 char *path;

 path = data;
 
 ewl_widget_destroy(image_view);

 image_view = ewl_image_new();
 ewl_image_file_set(EWL_IMAGE(image_view), (char *)data, NULL);
 ewl_container_child_append(EWL_CONTAINER(image_vbox), image_view);
 ewl_image_size_set(EWL_IMAGE(image_view), 400, 200);
 ewl_image_proportional_set(EWL_IMAGE(image_view), TRUE);
 ewl_object_fill_policy_set(EWL_OBJECT(image_view), EWL_FLAG_FILL_ALL);
 ewl_object_alignment_set(EWL_OBJECT(image_view), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(image_view);	  
}

void view_images(Ewl_Widget *w, void *event, void *data)
{
 char *current_image;
 Ewl_Widget *scrollpane;
 Ewl_Widget *freebox;
 Ewl_Widget *icon;
 Ewl_Widget *image;
 Ewl_Widget *shadow;
 Ewl_Widget *vbox;
 Ewl_Widget *cell;
 Ecore_List *view_thumbs;
 
 view_thumbs = current_thumbs;
 
 if (vwin)
 {
  if (VISIBLE(vwin))
  {
   ewl_widget_destroy(vwin);
  }
 }
 vwin = ewl_window_new();
 ewl_window_title_set(EWL_WINDOW(vwin), "Ephoto Image Viewer");
 ewl_window_name_set(EWL_WINDOW(vwin), "Ephoto Image Viewer");
 ewl_object_size_request(EWL_OBJECT(vwin), 480, 480);
 ewl_callback_append(vwin, EWL_CALLBACK_DELETE_WINDOW, destroy_vwin, NULL);
 ewl_widget_show(vwin);

 vbox = ewl_vbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(vwin), vbox);
 ewl_box_spacing_set(EWL_BOX(vbox), 10);
 ewl_widget_show(vbox);
 
 scrollpane = ewl_scrollpane_new();
 ewl_container_child_append(EWL_CONTAINER(vbox), scrollpane);
 ewl_object_fill_policy_set(EWL_OBJECT(scrollpane), EWL_FLAG_FILL_ALL);
 ewl_object_maximum_size_set(EWL_OBJECT(scrollpane), 99999, 85);
 ewl_widget_show(scrollpane);
 
 freebox = ewl_vfreebox_new();
 ewl_freebox_layout_type_set(EWL_FREEBOX(freebox), EWL_FREEBOX_LAYOUT_AUTO);
 ewl_container_child_append(EWL_CONTAINER(scrollpane), freebox);
 ewl_object_fill_policy_set(EWL_OBJECT(freebox), EWL_FLAG_FILL_ALL);
 ewl_object_maximum_size_set(EWL_OBJECT(freebox), 99999, 75);
 ewl_widget_show(freebox);

 image_vbox = ewl_vbox_new();
 ewl_theme_data_str_set(image_vbox, "/image/group",
                        ewl_theme_data_str_get(m->entry, "group"));
 ewl_container_child_append(EWL_CONTAINER(vbox), image_vbox);
 ewl_object_fill_policy_set(EWL_OBJECT(image_vbox), EWL_FLAG_FILL_ALL);
 ewl_widget_show(image_vbox);
 
 image_view = ewl_image_new();
 ewl_image_file_set(EWL_IMAGE(image_view), (char *)data, NULL);
 ewl_container_child_append(EWL_CONTAINER(image_vbox), image_view);
 ewl_image_size_set(EWL_IMAGE(image_view), 400, 200);
 ewl_image_proportional_set(EWL_IMAGE(image_view), TRUE);
 ewl_object_fill_policy_set(EWL_OBJECT(image_view), EWL_FLAG_FILL_ALL);
 ewl_object_alignment_set(EWL_OBJECT(image_view), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(image_view);
 
 ecore_dlist_goto_first(view_thumbs);
 while (ecore_dlist_current(view_thumbs))
 {
  current_image = ecore_dlist_current(view_thumbs);    
  
  shadow = ewl_shadow_new();
  ewl_container_child_append(EWL_CONTAINER(freebox), shadow);
  ewl_object_minimum_size_set(EWL_OBJECT(shadow), 66, 66);
  ewl_object_maximum_size_set(EWL_OBJECT(shadow), 66, 66);
  ewl_widget_show(shadow);

  cell = ewl_cell_new();
  ewl_container_child_append(EWL_CONTAINER(shadow), cell);
  ewl_object_minimum_size_set(EWL_OBJECT(cell), 50, 50);
  ewl_object_maximum_size_set(EWL_OBJECT(cell), 50, 50);
  ewl_theme_data_str_set(cell, "/cell/group",
                         ewl_theme_data_str_get(m->entry, "group"));
  ewl_callback_append(cell, EWL_CALLBACK_CLICKED, change_image, current_image);
  ewl_widget_show(cell);
  
  image = ewl_image_thumbnail_new();
  ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(image), current_image);
  ewl_container_child_append(EWL_CONTAINER(cell), image);
  ewl_image_size_set(EWL_IMAGE(image), 48, 48);
  ewl_image_proportional_set(EWL_IMAGE(image), TRUE);
  ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
  ewl_widget_show(image);
  
  ecore_dlist_next(view_thumbs);
 }  
}
