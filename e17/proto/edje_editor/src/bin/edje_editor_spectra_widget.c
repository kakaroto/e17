#include <string.h>
#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"

/******************************************************************/
/***   Spectra Widget   *******************************************/
/***   TODO make this a real  Etk_Widget with events  *************/

void _spectra_widget_gradient_draw(Etk_Widget *spectra);
void _spectra_widget_colorpicker_color_set(Etk_Widget *spectra, int r, int g, int b, int a);
Etk_Bool _spectra_widget_size_request_cb(Etk_Widget *widget, Etk_Size *size, void *data);
Etk_Bool _spectra_widget_color_mouse_down_cb(Etk_Widget *widget, Etk_Event_Mouse_Down *event, void *data);
Etk_Bool _spectra_widget_arrow_mouse_down_cb(Etk_Widget *widget, Etk_Event_Mouse_Down *event, void *data);
Etk_Bool _spectra_widget_arrow_mouse_up_cb(Etk_Widget *widget, Etk_Event_Mouse_Up *event, void *data);
Etk_Bool _spectra_widget_arrow_mouse_move_cb(Etk_Widget *widget, Etk_Event_Mouse_Move *event, void *data);
Etk_Bool _spectra_widget_colorpicker_change_cb(Etk_Colorpicker *colorpicker, void *data);
Etk_Bool _spectra_widget_add_button_click_cb(Etk_Button *button, void *data);
Etk_Bool _spectra_widget_del_button_click_cb(Etk_Button *button, void *data);
Etk_Bool _spectra_widget_entry_changed_cb(Etk_Entry *entry, void *data);


/* implementation */
Etk_Widget*
spectra_widget_new(Evas *evas)
{
   Etk_Widget *canvas;
   Etk_Widget *wid;
   Etk_Widget *vbox, *vbox2;
   Etk_Widget *hbox;
   Etk_Widget *colorp;
   Etk_Widget *label;
   Etk_Widget *entry;
   Etk_Widget *button;
   Etk_Widget *frame;
   Evas_Object *gradient;
   
   //vbox (main widget object)
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_signal_connect("size-requested", ETK_OBJECT(vbox),
                      ETK_CALLBACK(_spectra_widget_size_request_cb), NULL);
   
   frame = etk_frame_new("Spectra Editor");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);
   
   hbox = etk_hbox_new(ETK_FALSE, 5);
   etk_container_add(ETK_CONTAINER(vbox2), hbox);
   
   
   //Spectra Name Entry
   label = etk_label_new("<b>Spectra name: </b>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   entry = etk_entry_new();
   etk_signal_connect("text-changed", ETK_OBJECT(entry), 
                      ETK_CALLBACK(_spectra_widget_entry_changed_cb), vbox);
   etk_box_append(ETK_BOX(hbox), entry, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   //AddStopButton
   button = etk_button_new_from_stock(ETK_STOCK_LIST_ADD);
   etk_button_style_set(ETK_BUTTON(button), ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(button), 
                      ETK_CALLBACK(_spectra_widget_add_button_click_cb), vbox);
   etk_box_append(ETK_BOX(hbox), button, 0, ETK_BOX_START, 0);
   
   //DelStopButton
   button = etk_button_new_from_stock(ETK_STOCK_LIST_REMOVE);
   etk_button_style_set(ETK_BUTTON(button), ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_spectra_widget_del_button_click_cb), vbox);
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   
   //Spectra Canvas
   canvas = etk_canvas_new();
   etk_widget_padding_set(canvas, 0, 0, 10, 0);
   etk_box_append(ETK_BOX(vbox2), canvas, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   //Spectra Gradient
   gradient = evas_object_gradient_add(evas);
   evas_object_gradient_type_set(gradient, "linear", NULL);
   evas_object_gradient_fill_angle_set(gradient, 90);
   wid = etk_evas_object_new_from_object(gradient);
   etk_widget_size_request_set(wid, 20, 20);
   etk_box_append(ETK_BOX(vbox2), wid, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   //Spectra ColorPicker
   frame = etk_frame_new("Stop Color");
   etk_widget_padding_set(frame, 0, 0, 10, 0);
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   colorp = etk_colorpicker_new();
   etk_colorpicker_use_alpha_set (ETK_COLORPICKER(colorp), TRUE);
   //etk_widget_disabled_set(colorp, ETK_TRUE);
   etk_signal_connect("color-changed", ETK_OBJECT(colorp),
                      ETK_CALLBACK(_spectra_widget_colorpicker_change_cb), vbox);
   etk_container_add(ETK_CONTAINER(frame), colorp);
   
   etk_object_data_set(ETK_OBJECT(vbox), "canvas_o", canvas);
   etk_object_data_set(ETK_OBJECT(vbox), "gradient_o", gradient);
   etk_object_data_set(ETK_OBJECT(vbox), "colorpicker_o", colorp);
   etk_object_data_set(ETK_OBJECT(vbox), "entry_o", entry);
   etk_object_data_set(ETK_OBJECT(canvas), "images_list", NULL);
   etk_object_data_set(ETK_OBJECT(colorp), "current_image_o", NULL);

   return vbox;
}

void
spectra_widget_name_set(Etk_Widget *spectra, const char *name)
{
   Etk_Widget *entry;
   
   if (!spectra || !name) return;
   
   entry = etk_object_data_get(ETK_OBJECT(spectra), "entry_o");
   etk_signal_block("text-changed", ETK_OBJECT(entry), 
                    ETK_CALLBACK(_spectra_widget_entry_changed_cb), spectra);
   etk_entry_text_set(ETK_ENTRY(entry), name);
   etk_signal_unblock("text-changed", ETK_OBJECT(entry), 
                      ETK_CALLBACK(_spectra_widget_entry_changed_cb), spectra);
   
}

const char*
spectra_widget_name_get(Etk_Widget *spectra)
{
   Etk_Widget *entry;
   
   if (!spectra) return NULL;
   
   entry = etk_object_data_get(ETK_OBJECT(spectra), "entry_o");
   return etk_entry_text_get(ETK_ENTRY(entry));
}

void
spectra_widget_color_add(Etk_Widget *spectra, int r, int g, int b, int a, int d)
{
   Evas_List *images;
   Etk_Widget *canvas;
   Etk_Widget *image;
   Etk_Widget *arrow;
   Etk_Widget *colorp;
   
   canvas = etk_object_data_get(ETK_OBJECT(spectra), "canvas_o");
   
   //Arrow
   arrow = etk_image_new_from_edje(EdjeFile, "ARROW.PNG");
   etk_object_data_set(ETK_OBJECT(arrow), "spectra", spectra);
   etk_signal_connect("mouse-down", ETK_OBJECT(arrow),
                      ETK_CALLBACK(_spectra_widget_arrow_mouse_down_cb), canvas);
   etk_signal_connect("mouse-up", ETK_OBJECT(arrow),
                      ETK_CALLBACK(_spectra_widget_arrow_mouse_up_cb), canvas);
   etk_canvas_put(ETK_CANVAS(canvas), arrow, 0, 0);
   
   //Color Image
   image = etk_image_new_from_edje(EdjeFile, "COLOR.PNG");
   etk_widget_color_set(image, r, g, b, 255);
   etk_object_data_set(ETK_OBJECT(image), "spectra", spectra);
   etk_object_data_set(ETK_OBJECT(image), "color_r", (void*)r);
   etk_object_data_set(ETK_OBJECT(image), "color_g", (void*)g);
   etk_object_data_set(ETK_OBJECT(image), "color_b", (void*)b);
   etk_object_data_set(ETK_OBJECT(image), "color_a", (void*)a);
   etk_object_data_set(ETK_OBJECT(image), "color_d", (void*)d);
   etk_object_data_set(ETK_OBJECT(image), "arrow_o", arrow);
   etk_signal_connect("mouse-down", ETK_OBJECT(image),
                      ETK_CALLBACK(_spectra_widget_color_mouse_down_cb), canvas);
   etk_canvas_put(ETK_CANVAS(canvas), image, d, 0);
   etk_widget_show(image);
   etk_widget_raise(arrow);
   
   //Update images_list
   images = etk_object_data_get(ETK_OBJECT(canvas), "images_list");
   images = evas_list_append(images, image);
   etk_object_data_set(ETK_OBJECT(canvas), "images_list", images);
   
   //Set in colorpicker
   _spectra_widget_colorpicker_color_set(spectra, r, g, b, a);
   colorp = etk_object_data_get(ETK_OBJECT(spectra), "colorpicker_o");
   etk_object_data_set(ETK_OBJECT(colorp), "current_image_o", image);
   
   _spectra_widget_gradient_draw(spectra);
}

void
spectra_widget_color_clear_all(Etk_Widget* spectra)
{
   Evas_Object *gradient;
   Evas_Object *canvas;
   Evas_List *images;
   
   gradient = etk_object_data_get(ETK_OBJECT(spectra), "gradient_o");
   canvas = etk_object_data_get(ETK_OBJECT(spectra), "canvas_o");
   images = etk_object_data_get(ETK_OBJECT(canvas), "images_list");
   
   evas_object_gradient_clear(gradient);
   while (images)
   {
      Etk_Widget *arrow;
      Etk_Widget *image;
      image = images->data;
      arrow = etk_object_data_get(ETK_OBJECT(image), "arrow_o");
      
      etk_object_destroy(ETK_OBJECT(image));
      etk_object_destroy(ETK_OBJECT(arrow));
      
      images = evas_list_remove_list(images, images);
   }
   
   etk_object_data_set(ETK_OBJECT(canvas), "images_list", NULL);
}

int
spectra_widget_colors_num_get(Etk_Widget* spectra)
{
   Etk_Widget *canvas;
   Evas_List *images;
   
   canvas = etk_object_data_get(ETK_OBJECT(spectra), "canvas_o");
   images = etk_object_data_get(ETK_OBJECT(canvas), "images_list");
   
   return images ? evas_list_count(images) : -1 ;
}

void
spectra_widget_color_get(Etk_Widget *spectra, int color_num, int *r, int *g, int *b, int *a, int *d)
{
   Etk_Widget *canvas;
   Etk_Widget *image;
   Evas_List *images;

   canvas = etk_object_data_get(ETK_OBJECT(spectra), "canvas_o");
   images = etk_object_data_get(ETK_OBJECT(canvas), "images_list");
   
   if (!images) return;
   image = evas_list_nth(images, color_num);
   if (!image) return;
   
   *r = (int)etk_object_data_get(ETK_OBJECT(image), "color_r");
   *g = (int)etk_object_data_get(ETK_OBJECT(image), "color_g");
   *b = (int)etk_object_data_get(ETK_OBJECT(image), "color_b");
   *a = (int)etk_object_data_get(ETK_OBJECT(image), "color_a");
   *d = (int)etk_object_data_get(ETK_OBJECT(image), "color_d");
}

/* internals */
void
_spectra_widget_gradient_draw(Etk_Widget *spectra)
{
   Evas_Object *gradient;
   Etk_Widget *canvas;
   Evas_List *images, *l;
   int canvas_w, canvas_h, dtot = 0, total = 0;
   double rapporto;
   
   //printf("GRAD DRAW\n");
   
   canvas = etk_object_data_get(ETK_OBJECT(spectra), "canvas_o");
   images = etk_object_data_get(ETK_OBJECT(canvas), "images_list");
   gradient = etk_object_data_get(ETK_OBJECT(spectra), "gradient_o");
   etk_widget_geometry_get(canvas, NULL, NULL, &canvas_w, &canvas_h);
   
   //Calc total stops value
   for (l = images; l; l = l->next)
   {
      Etk_Widget *image;
      image = l->data;
      total += (int)etk_object_data_get(ETK_OBJECT(image), "color_d");
   }
   
   rapporto = (float)canvas_w / (float)total;
   
   //Recreate evas gradient
   evas_object_gradient_clear(gradient);
   for (l = images; l; l = l->next)
   {
      int r, g, b, a, d;
      Etk_Widget *image;
      Etk_Widget *arrow;
      
      image = l->data;
      
      r = (int)etk_object_data_get(ETK_OBJECT(image), "color_r");
      g = (int)etk_object_data_get(ETK_OBJECT(image), "color_g");
      b = (int)etk_object_data_get(ETK_OBJECT(image), "color_b");
      a = (int)etk_object_data_get(ETK_OBJECT(image), "color_a");
      d = (int)etk_object_data_get(ETK_OBJECT(image), "color_d");
      arrow = etk_object_data_get(ETK_OBJECT(image), "arrow_o");
      
      evas_object_gradient_color_stop_add(gradient, r, g, b, a, d);
      dtot += d;
      
      //Draw image
      if (!l->prev)
         etk_canvas_move(ETK_CANVAS(canvas), image, -1, 0);
      else if (!l->next)
         etk_canvas_move(ETK_CANVAS(canvas), image, canvas_w - 13, 0);
      else
         etk_canvas_move(ETK_CANVAS(canvas), image, (dtot - d / 2) * rapporto , 0);
      
      //Draw arrow
      if  (l->next)
      {
         etk_canvas_move(ETK_CANVAS(canvas), arrow, dtot * rapporto, 0);
         etk_widget_show(arrow);
      }
   }
}

void
_spectra_widget_colorpicker_color_set(Etk_Widget *spectra, int r, int g, int b, int a)
{
   Etk_Widget *colorp;
   Etk_Color col;
   
   colorp = etk_object_data_get(ETK_OBJECT(spectra), "colorpicker_o");
   
   col.r = r;
   col.g = g;
   col.b = b;
   col.a = a;
   
   etk_signal_block("color-changed", ETK_OBJECT(colorp),
                    ETK_CALLBACK(_spectra_widget_colorpicker_change_cb), spectra);
   etk_colorpicker_current_color_set(ETK_COLORPICKER(colorp), col);
   etk_signal_unblock("color-changed", ETK_OBJECT(colorp),
                      ETK_CALLBACK(_spectra_widget_colorpicker_change_cb), spectra);
}

/* callbacks */
Etk_Bool
_spectra_widget_size_request_cb(Etk_Widget *widget, Etk_Size *size, void *data)
{
   Etk_Widget *canvas;
   Evas_Object *gradient;
   int canvas_w, canvas_h;
   
   canvas = etk_object_data_get(ETK_OBJECT(widget), "canvas_o");
   gradient = etk_object_data_get(ETK_OBJECT(widget), "gradient_o");
   
   etk_widget_geometry_get(canvas, NULL, NULL, &canvas_w, &canvas_h);
   evas_object_gradient_fill_set(gradient, 0, 0, canvas_h, canvas_w);
   
   _spectra_widget_gradient_draw(widget);
   return ETK_TRUE;
}

Etk_Bool
_spectra_widget_color_mouse_down_cb(Etk_Widget *widget, Etk_Event_Mouse_Down *event, void *data)
{
   Etk_Widget *spectra;
   Etk_Widget *colorp;
   int r, g, b, a;
   //printf("MOUSE DOWN\n");
   
   spectra = etk_object_data_get(ETK_OBJECT(widget), "spectra");
   colorp = etk_object_data_get(ETK_OBJECT(spectra), "colorpicker_o");
   
   etk_object_data_set(ETK_OBJECT(colorp), "current_image_o", widget);
   
   r = (int)etk_object_data_get(ETK_OBJECT(widget), "color_r");
   g = (int)etk_object_data_get(ETK_OBJECT(widget), "color_g");
   b = (int)etk_object_data_get(ETK_OBJECT(widget), "color_b");
   a = (int)etk_object_data_get(ETK_OBJECT(widget), "color_a");
   
   _spectra_widget_colorpicker_color_set(spectra, r, g, b, a);
   //etk_widget_disabled_set(colorp, ETK_FALSE);
   
   return ETK_TRUE;
}

Etk_Bool
_spectra_widget_arrow_mouse_down_cb(Etk_Widget *widget, Etk_Event_Mouse_Down *event, void *data)
{
   etk_signal_connect("mouse-move", ETK_OBJECT(widget),
                      ETK_CALLBACK(_spectra_widget_arrow_mouse_move_cb), data);
   
   return ETK_TRUE;
}

Etk_Bool
_spectra_widget_arrow_mouse_up_cb(Etk_Widget *widget, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Widget *spectra;
   spectra = etk_object_data_get(ETK_OBJECT(widget), "spectra");
   etk_signal_disconnect("mouse-move", ETK_OBJECT(widget),
                         ETK_CALLBACK(_spectra_widget_arrow_mouse_move_cb), data);
   spectra_window_gradient_changed(spectra); //TODO emit signal instead of calling this function
   return ETK_TRUE;
}

Etk_Bool
_spectra_widget_arrow_mouse_move_cb(Etk_Widget *widget, Etk_Event_Mouse_Move *event, void *data)
{
   Etk_Widget *canvas = data;
   Etk_Widget *spectra;
   int mouse_x, mouse_y;
   int canvas_x, canvas_w;
   int arrow_w;
   int x;
   
   //Get datas
   spectra = etk_object_data_get(ETK_OBJECT(widget), "spectra");
   evas_pointer_output_xy_get(etk_widget_toplevel_evas_get(widget), &mouse_x, &mouse_y);
   etk_widget_geometry_get(canvas, &canvas_x, NULL, &canvas_w, NULL);
   etk_widget_geometry_get(widget, NULL, NULL, &arrow_w, NULL);

   //Move arrow
   x = mouse_x - canvas_x;
   if (x < 5) x = 5;
   if (x > canvas_w - 7) x = canvas_w - 7;
   etk_canvas_move(ETK_CANVAS(canvas), widget, x - arrow_w / 2, 0);
   
   //Recalc stops
   Evas_List *images;
   int total = 0;
   images = etk_object_data_get(ETK_OBJECT(canvas), "images_list");
   
   while (images)
   {
      Etk_Widget *image;
      Etk_Widget *point;
      int px, py;
      
      image = images->data;
      point = etk_object_data_get(ETK_OBJECT(image), "arrow_o");
      etk_canvas_child_position_get(ETK_CANVAS(canvas), point, &px, &py);
      
      if (images->next)
         etk_object_data_set(ETK_OBJECT(image), "color_d", (void*)px - total);
      else
         etk_object_data_set(ETK_OBJECT(image), "color_d", (void*)canvas_w - total);
      
      total = px;
      images = images->next;
   }
   
   _spectra_widget_gradient_draw(spectra);
   
   return ETK_TRUE;
}

Etk_Bool
_spectra_widget_colorpicker_change_cb(Etk_Colorpicker *colorpicker, void *data)
{
   Etk_Widget *image;
   Etk_Color col;
   
   image = etk_object_data_get(ETK_OBJECT(colorpicker), "current_image_o");
   if (!image) return ETK_TRUE;
   
   col = etk_colorpicker_current_color_get(colorpicker);
   etk_object_data_set(ETK_OBJECT(image), "color_r", (void*)col.r);
   etk_object_data_set(ETK_OBJECT(image), "color_g", (void*)col.g);
   etk_object_data_set(ETK_OBJECT(image), "color_b", (void*)col.b);
   etk_object_data_set(ETK_OBJECT(image), "color_a", (void*)col.a);
   
   etk_widget_color_set(image, col.r, col.g, col.b, 255);
   
   _spectra_widget_gradient_draw(data);   
   spectra_window_gradient_changed(data);

   return ETK_TRUE;
}

Etk_Bool
_spectra_widget_add_button_click_cb(Etk_Button *button, void *data)
{
   Etk_Widget *spectra = data;
   Etk_Widget *canvas;
   Etk_Widget *image;
   Evas_List *images;
   int r, g, b, d;
   
   //printf("ADD STOP\n");
   
   canvas = etk_object_data_get(ETK_OBJECT(spectra), "canvas_o");
   images = etk_object_data_get(ETK_OBJECT(canvas), "images_list");
   
   if (images)
   {
      images = evas_list_last(images);
      image = images->data;
   
      r = (int)etk_object_data_get(ETK_OBJECT(image), "color_r");
      g = (int)etk_object_data_get(ETK_OBJECT(image), "color_g");
      b = (int)etk_object_data_get(ETK_OBJECT(image), "color_b");
      d = (int)etk_object_data_get(ETK_OBJECT(image), "color_d");
      spectra_widget_color_add(data, g, b, r, 255, d);
   }
   else
   {
      spectra_widget_color_add(data, 255, 255, 255, 255, 10);
   }
   
   spectra_window_gradient_changed(spectra);
   
   return ETK_TRUE;
}

Etk_Bool
_spectra_widget_del_button_click_cb(Etk_Button *button, void *data)
{
   
   Etk_Widget *spectra = data;
   Etk_Widget *image;
   Etk_Widget *canvas;
   Etk_Widget *colorp;
   Etk_Widget *arrow;
   Evas_List *images;
   
   //printf("DEL STOP\n");
   
   colorp = etk_object_data_get(ETK_OBJECT(spectra), "colorpicker_o");
   image = etk_object_data_get(ETK_OBJECT(colorp), "current_image_o");
   
   if (!image) return ETK_TRUE;
   
   canvas = etk_object_data_get(ETK_OBJECT(spectra), "canvas_o"); 
   arrow = etk_object_data_get(ETK_OBJECT(image), "arrow_o"); 
   images = etk_object_data_get(ETK_OBJECT(canvas), "images_list");
   
   if (evas_list_count(images) < 3) return ETK_TRUE; //keep at least 2 colors
   
   images = evas_list_remove(images, image);
   etk_object_data_set(ETK_OBJECT(canvas), "images_list", images);
   
   etk_object_destroy(ETK_OBJECT(image));
   etk_object_destroy(ETK_OBJECT(arrow));

   etk_object_data_set(ETK_OBJECT(colorp), "current_image_o", NULL);
   _spectra_widget_colorpicker_color_set(spectra, 255, 255, 255, 255);
  // etk_widget_disabled_set(colorp, ETK_TRUE);

   //Hide last arrow
   images = evas_list_last(images);
   image = images->data;
   arrow = etk_object_data_get(ETK_OBJECT(image), "arrow_o");
   etk_widget_hide(arrow);
   
   _spectra_widget_gradient_draw(data);
   spectra_window_gradient_changed(spectra);
   
   return ETK_TRUE;
}

Etk_Bool
_spectra_widget_entry_changed_cb(Etk_Entry *entry, void *data)
{
   spectra_window_gradient_name_changed(data);
   return ETK_TRUE;
}
