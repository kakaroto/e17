#include "ephoto.h"
Ewl_Widget *vwin;
Ewl_Widget *ibox;
Ewl_Widget *image_view;

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
 ewl_image_file_set(EWL_IMAGE(image_view), path, NULL);
 ewl_container_child_append(EWL_CONTAINER(ibox), image_view);
 ewl_image_proportional_set(EWL_IMAGE(image_view), TRUE);
 ewl_object_fill_policy_set(EWL_OBJECT(image_view), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(image_view), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(image_view);

 ewl_widget_configure(ibox);
}

void zoom_in(Ewl_Widget *w, void *event, void *data)
{
 int ow, oh;
 
 ewl_object_current_size_get(EWL_OBJECT(image_view), &ow, &oh);
 
 ewl_image_size_set(EWL_IMAGE(image_view), ow*2, oh*2);
 
 ewl_widget_configure(ibox);
}

void zoom_out(Ewl_Widget *w, void *event, void *data)
{
 int ow, oh;
 
 ewl_object_current_size_get(EWL_OBJECT(image_view), &ow, &oh);
 
 ewl_image_size_set(EWL_IMAGE(image_view), ow/2, oh/2);
 
 ewl_widget_configure(ibox);
}

void original_size(Ewl_Widget *w, void *event, void *data)
{
 const char *path;

 path = ewl_image_file_path_get(EWL_IMAGE(image_view));

 ewl_widget_destroy(image_view);
 
 image_view = ewl_image_new();
 ewl_image_file_set(EWL_IMAGE(image_view), path, NULL);
 ewl_container_child_append(EWL_CONTAINER(ibox), image_view);
 ewl_image_proportional_set(EWL_IMAGE(image_view), TRUE);
 ewl_object_fill_policy_set(EWL_OBJECT(image_view), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(image_view), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(image_view);
 
 ewl_widget_configure(ibox);
}

void rotate_left(Ewl_Widget *w, void *event, void *data)
{
 unsigned int *im_data, *im_data_new;
 int index, ind, i, j, ni, nj, ew, eh, nw, nh;

 im_data = evas_object_image_data_get(EWL_IMAGE(image_view)->image, FALSE);
 evas_object_image_size_get(EWL_IMAGE(image_view)->image, &ew, &eh);
 index = 0;
 
 im_data_new = malloc(sizeof(unsigned int) * ew * eh);
 
 nw = eh;
 nh = ew;
 
 for (i = 0; i < nh; i++)
 {
  for (j = 0; j <  nw; j++)
  {
   ni = j;
   nj = nh - i - 1;

   ind = ni * nh + nj;
   
   im_data_new[index] = im_data[ind];
   index++;
  }
 }

 evas_object_image_size_set(EWL_IMAGE(image_view)->image, nw, nh);
 evas_object_image_data_set(EWL_IMAGE(image_view)->image, im_data_new);
 evas_object_image_data_update_add(EWL_IMAGE(image_view)->image, 0, 0, nw, nh);
 ewl_widget_configure(image_view);
 ewl_widget_configure(ibox);
}

void rotate_right(Ewl_Widget *w, void *event, void *data)
{
 unsigned int *im_data, *im_data_new;
 int index, ind, i, j, ni, nj, ew, eh, nw, nh;

 im_data = evas_object_image_data_get(EWL_IMAGE(image_view)->image, FALSE);
 evas_object_image_size_get(EWL_IMAGE(image_view)->image, &ew, &eh);
 index = 0;

 im_data_new = malloc(sizeof(unsigned int) * ew * eh);

 nw = eh;
 nh = ew;

 for (i = 0; i < nh; i++)
 {
  for (j = 0; j < nw; j++)
  {
   ni = nw - j - 1;
   nj = i;

   ind = ni * nh + nj;

   im_data_new[index] = im_data[ind];
   index++;
  }
 }

 evas_object_image_size_set(EWL_IMAGE(image_view)->image, nw, nh);
 evas_object_image_data_set(EWL_IMAGE(image_view)->image, im_data_new);
 evas_object_image_data_update_add(EWL_IMAGE(image_view)->image, 0, 0, nw, nh);
 ewl_widget_configure(image_view);
 ewl_widget_configure(ibox); 
}
 
void view_images(Ewl_Widget *w, void *event, void *data)
{
 char *current_image;
 Ewl_Widget *button;
 Ewl_Widget *vbox;
 Ewl_Widget *scrollpane;
 Ewl_Widget *freebox;
 Ewl_Widget *icon;
 Ewl_Widget *image;
 Ewl_Widget *hbox;
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
 ewl_window_title_set(EWL_WINDOW(vwin), gettext("Ephoto Image Viewer"));
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
 
 ibox = ewl_scrollpane_new();
 ewl_object_fill_policy_set(EWL_OBJECT(ibox), EWL_FLAG_FILL_ALL);
 ewl_object_alignment_set(EWL_OBJECT(ibox), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(vbox), ibox);
 ewl_widget_show(ibox);
 
 image_view = ewl_image_new();
 ewl_container_child_append(EWL_CONTAINER(ibox), image_view);
 ewl_image_proportional_set(EWL_IMAGE(image_view), TRUE);
 ewl_object_fill_policy_set(EWL_OBJECT(image_view), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(image_view), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(image_view);

 hbox = ewl_hbox_new();
 ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(hbox);

 button = ewl_button_new();
 ewl_button_image_set(EWL_BUTTON(button), 
		      PACKAGE_DATA_DIR "/images/search.png", NULL);
 ewl_button_label_set(EWL_BUTTON(button), gettext("In"));
 ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), button);
 ewl_callback_append(button, EWL_CALLBACK_CLICKED, zoom_in, image_view);
 ewl_widget_show(button);
 
 button = ewl_button_new();
 ewl_button_image_set(EWL_BUTTON(button), 
		      PACKAGE_DATA_DIR "/images/search.png", NULL);
 ewl_button_label_set(EWL_BUTTON(button), gettext("Out"));
 ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), button);
 ewl_callback_append(button, EWL_CALLBACK_CLICKED, zoom_out, image_view);
 ewl_widget_show(button);

 button = ewl_button_new();
 ewl_button_image_set(EWL_BUTTON(button), 
		      PACKAGE_DATA_DIR "/images/search.png", NULL);
 ewl_button_label_set(EWL_BUTTON(button), "1:1");
 ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), button);
 ewl_callback_append(button, EWL_CALLBACK_CLICKED, original_size, image_view);
 ewl_widget_show(button);

 button = ewl_button_new();
 ewl_button_image_set(EWL_BUTTON(button), 
		      PACKAGE_DATA_DIR "/images/undo.png", NULL);
 ewl_button_label_set(EWL_BUTTON(button), gettext("Left"));
 ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), button);
 ewl_callback_append(button, EWL_CALLBACK_CLICKED, rotate_left, NULL);
 ewl_widget_show(button);

 button = ewl_button_new();
 ewl_button_image_set(EWL_BUTTON(button), 
		      PACKAGE_DATA_DIR "/images/redo.png", NULL);
 ewl_button_label_set(EWL_BUTTON(button), gettext("Right"));
 ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), button);
 ewl_callback_append(button, EWL_CALLBACK_CLICKED, rotate_right, NULL);
 ewl_widget_show(button);

 ecore_dlist_goto_first(view_thumbs);
 while (ecore_dlist_current(view_thumbs))
 {
  current_image = ecore_dlist_current(view_thumbs);    

  cell = ewl_cell_new();
  ewl_container_child_append(EWL_CONTAINER(freebox), cell);
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
 if (data) 
 	ewl_callback_append(vbox, EWL_CALLBACK_SHOW, change_image, data);
 if (!data)
	ewl_callback_append(vbox, EWL_CALLBACK_SHOW, change_image, 
			    (char *)ecore_dlist_goto_first(view_thumbs));
}
