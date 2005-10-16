#include "etk_test.h"
#include <Evas.h>
#include <stdlib.h>
#include "../../config.h"

static void _etk_test_canvas_object_add(void *data);

static Etk_Bool _etk_test_canvas_window_deleted_cb (void *data)
{
   Etk_Window *win = data;
   etk_widget_hide (ETK_WIDGET(win));
   return 1;
}

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
   etk_window_title_set(ETK_WINDOW(win), _("Etk Canvas Test"));

   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_canvas_window_deleted_cb), win);	
	
   vbox = etk_vbox_new(FALSE, 5);
   etk_container_add(ETK_CONTAINER(win), vbox);

   canvas = etk_canvas_new();
   etk_widget_size_request_set(canvas, 300, 200);
   etk_box_pack_start(ETK_BOX(vbox), canvas, TRUE, TRUE, 0);

   button = etk_button_new_with_label(_("Add object"));
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_canvas_object_add), canvas);
   etk_box_pack_start(ETK_BOX(vbox), button, FALSE, FALSE, 0);

   etk_widget_show_all(win);
}

static void _etk_test_canvas_object_add(void *data)
{
   Etk_Canvas *canvas;
   Evas *evas;
   Evas_Object *object;
   int x, y, w, h;
   int r, g, b, a;

   if (!(canvas = ETK_CANVAS(data)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(canvas))))
      return;

   object = evas_object_rectangle_add(evas);
   etk_canvas_object_add(canvas, object);

   x = rand() % 300;
   y = rand() % 200;
   evas_object_move(object, x, y);

   w = ETK_MAX(abs(rand() % (300 - x)), 10);
   h = ETK_MAX(abs(rand() % (200 - y)), 10);
   evas_object_resize(object, w, h);

   r = rand() % 255;
   g = rand() % 255;
   b = rand() % 255;
   a = ETK_MAX(rand() % 255, 40);
   evas_object_color_set(object, r, g, b, a);

   //printf("%d %d %d %d / %d %d %d %d\n", x, y, w, h, r, g, b, a);

   evas_object_show(object);
   evas_object_raise(object);
}
