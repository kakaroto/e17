#include "etk_test.h"
#include <Evas.h>
#include <stdlib.h>

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
   int x, y, w, h;
   int r, g, b, a;
   int cw, ch;

   if (!(canvas = ETK_CANVAS(data)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(canvas))))
      return;

   object = evas_object_rectangle_add(evas);
   evas_object_show(object);
   etk_canvas_object_add(canvas, object);

   /* TODO: make a function to get the geometry of a widget */
   cw = ETK_WIDGET(canvas)->geometry.w;
   ch = ETK_WIDGET(canvas)->geometry.h;
   x = rand() % cw;
   y = rand() % ch;
   evas_object_move(object, x, y);

   w = ETK_MAX(abs(rand() % (cw - x)), 10);
   h = ETK_MAX(abs(rand() % (ch - y)), 10);
   evas_object_resize(object, w, h);

   r = rand() % 255;
   g = rand() % 255;
   b = rand() % 255;
   a = ETK_MAX(rand() % 255, 40);
   evas_object_color_set(object, r, g, b, a);
}
