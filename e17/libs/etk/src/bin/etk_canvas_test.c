#include "etk_test.h"
#include <Evas.h>
#include <stdlib.h>
#include "config.h"

#include "Ecore.h"

static Etk_Widget *win = NULL;
Etk_Widget *canvas;

static void _etk_test_canvas_object_add(void *data);

static Ecore_Timer *_etk_test_canvas_timer = NULL;
static Ecore_Timer *_etk_test_canvas_timer2 = NULL;

static int _etk_test_canvas_update(void *data);
static int _etk_test_canvas_update2(void *data);

/* Creates the vbox for the progress bars */
static void _etk_test_canvas_progress_bars_create(int x, int y)
{
   Etk_Widget *vbox;
   Etk_Widget *pbar;
   Etk_Widget *pbar2;

   vbox = etk_vbox_new(ETK_TRUE, 5);

   pbar = etk_progress_bar_new_with_text("0% done");
   etk_box_append(ETK_BOX(vbox), pbar, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   pbar2 = etk_progress_bar_new_with_text("Loading...");
   etk_progress_bar_pulse_step_set(ETK_PROGRESS_BAR(pbar2), 0.015);
   etk_box_append(ETK_BOX(vbox), pbar2, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   _etk_test_canvas_timer = ecore_timer_add(0.05, _etk_test_canvas_update, pbar);
   _etk_test_canvas_timer2 = ecore_timer_add(0.025, _etk_test_canvas_update2, pbar2);

   etk_signal_connect_swapped_by_code(ETK_OBJECT_DESTROYED_SIGNAL, ETK_OBJECT(pbar), ETK_CALLBACK(ecore_timer_del), _etk_test_canvas_timer);
   etk_signal_connect_swapped_by_code(ETK_OBJECT_DESTROYED_SIGNAL, ETK_OBJECT(pbar2), ETK_CALLBACK(ecore_timer_del), _etk_test_canvas_timer2);

   etk_canvas_put(ETK_CANVAS(canvas), vbox, x, y);
}

/* Updates the first progress bar */
static int _etk_test_canvas_update(void *data)
{
   Etk_Widget *pbar;
   double fraction;
   char text[32];

   pbar = data;

   fraction = etk_progress_bar_fraction_get(ETK_PROGRESS_BAR(pbar));
   fraction += 0.01;

   if (fraction > 1.0)
      fraction = 0.0;

   snprintf(text, sizeof(text), "%d%% done", (int)(fraction * 100.0));
   etk_progress_bar_text_set(ETK_PROGRESS_BAR(pbar), text);
   etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(pbar), fraction);

   return 1;
}

/* Updates the second progress bar */
static int _etk_test_canvas_update2(void *data)
{
   Etk_Widget *pbar;

   pbar = data;
   etk_progress_bar_pulse(ETK_PROGRESS_BAR(pbar));

   return 1;
}

/* Creates the window for the canvas test */
void etk_test_canvas_window_create(void *data)
{
   Etk_Widget *vbox;
   Etk_Widget *button;
	 int i;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Canvas Test");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   vbox = etk_vbox_new(ETK_FALSE, 5);
   etk_container_add(ETK_CONTAINER(win), vbox);

   canvas = etk_canvas_new();
   etk_widget_size_request_set(canvas, 500, 350);
   etk_box_append(ETK_BOX(vbox), canvas, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   button = etk_button_new_from_stock(ETK_STOCK_LIST_ADD);
   etk_button_label_set(ETK_BUTTON(button), "Add object");
   etk_signal_connect_swapped_by_code(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(button), ETK_CALLBACK(_etk_test_canvas_object_add), canvas);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   /* add some progess bars to the canvas */
   for (i = 0; i < 3; i++)
   {
      _etk_test_canvas_progress_bars_create(50 + 50 * i, 50 + 50 * i);
   }

   etk_widget_show_all(win);
}

/* Adds a rectangle object to the canvas */
static void _etk_test_canvas_object_add(void *data)
{
   Etk_Canvas *canvas;
   Evas *evas;
   Evas_Object *object;
   Etk_Widget *etk_evas_object;
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
      w = ETK_MAX(abs(rand() % (cw - x)), 10);
      h = ETK_MAX(abs(rand() % (ch - y)), 10);
      evas_object_resize(object, w, h);

      r = rand() % 255;
      g = rand() % 255;
      b = rand() % 255;
      a = ETK_MAX(rand() % 255, 40);
      evas_color_argb_premul(a, &r, &g, &b);

      evas_object_color_set(object, r, g, b, a);
      etk_evas_object = etk_evas_object_new();
			etk_evas_object_set_object(ETK_EVAS_OBJECT(etk_evas_object), object);
   }
   /* Or add an image */
   else
   {
      object = evas_object_image_add(evas);
      evas_object_image_file_set(object, PACKAGE_DATA_DIR "/images/test.png", NULL);
      evas_object_image_fill_set(object, 0, 0, 48, 48);
      evas_object_resize(object, 48, 48);
      etk_evas_object = etk_evas_object_new();
      etk_evas_object_set_object(ETK_EVAS_OBJECT(etk_evas_object), object);
   }

   etk_canvas_put(canvas, etk_evas_object, x, y);
   etk_widget_show_all(etk_evas_object);
}
