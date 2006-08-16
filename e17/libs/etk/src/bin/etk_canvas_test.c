#include "etk_test.h"
#include <Evas.h>
#include <stdlib.h>
#include "config.h"

static void _etk_test_canvas_object_add(void *data);

/* Creates the window for the canvas test */
void etk_test_canvas_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *canvas;
   Etk_Widget *vbox;
   Etk_Widget *button;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Canvas Test");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);	

   vbox = etk_vbox_new(ETK_FALSE, 5);
   etk_container_add(ETK_CONTAINER(win), vbox);

   canvas = etk_canvas_new();
   etk_widget_size_request_set(canvas, 300, 200);
   etk_box_append(ETK_BOX(vbox), canvas, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   button = etk_button_new_with_label("Add object");
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_canvas_object_add), canvas);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   etk_widget_show_all(win);
}

/* Adds a rectangle object to the canvas */
static void _etk_test_canvas_object_add(void *data)
{
   Etk_Canvas *canvas;
   Evas *evas;
   Evas_Object *object;
   int x, y;
   int cw, ch;

   if (!(canvas = ETK_CANVAS(data)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(canvas))))
      return;
   
   etk_widget_geometry_get(ETK_WIDGET(canvas), NULL, NULL, &cw, &ch);
   x = rand() % cw;
   y = rand() % ch;
   
   /* Add a rectangle */
   if (rand() % 4 != 0)
   {
      int w, h;
      int r, g, b, a;
      
      object = evas_object_rectangle_add(evas);
      etk_canvas_object_add(canvas, object);
      
      w = ETK_MAX(abs(rand() % (cw - x)), 10);
      h = ETK_MAX(abs(rand() % (ch - y)), 10);
      evas_object_resize(object, w, h);
      
      r = rand() % 255;
      g = rand() % 255;
      b = rand() % 255;
      a = ETK_MAX(rand() % 255, 40);
      evas_object_color_set(object, r, g, b, a);
   }
   /* Or add an image */
   else
   {
      object = evas_object_image_add(evas);
      etk_canvas_object_add(canvas, object);
      
      evas_object_image_file_set(object, PACKAGE_DATA_DIR "/images/test.png", NULL);
      evas_object_image_fill_set(object, 0, 0, 48, 48);
      evas_object_resize(object, 48, 48);
   }
   
   etk_canvas_object_move(canvas, object, x, y);
   evas_object_show(object);
}
