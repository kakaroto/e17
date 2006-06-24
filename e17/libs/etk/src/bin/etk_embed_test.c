#include "etk_test.h"
#include <sys/time.h>
#include <math.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Evas.h>
#include "config.h"

/* Test program *highly* inspired from the evas test program */

#define ETK_TEST_PI     (3.141592654)

static Etk_Widget *_etk_test_embed_widget_new(Evas *evas);
static void _etk_test_embed_update();
static void _etk_test_embed_resize_cb(Ecore_Evas *ecore_evas);
//static void _etk_test_embed_delete_request_cb(Ecore_Evas *ecore_evas);
static int _etk_test_embed_animator_cb(void *data);
static double _etk_test_embed_time_get();

static Ecore_Evas *ecore_evas = NULL;
static Evas_Object *e_logo, *backdrop;
static Evas_Object *panel, *panel_top, *panel_shadow;
static Evas_Object *panel_clip;
static Evas_Object *embed_object;
static Etk_Widget *embed;
static int win_w = 240, win_h = 320;
static double starting_time = 0.0;

/* Creates the window for the embed widget test */
void etk_test_embed_window_create(void *data)
{
   Evas *evas;
   int w, h;
   Etk_Size min_size;
   
   if (ecore_evas)
   {
      ecore_evas_show(ecore_evas);
      return;
   }
   
   ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, 32, 32);
   ecore_evas_title_set(ecore_evas, "Etk Embed Test");
   ecore_evas_callback_resize_set(ecore_evas, _etk_test_embed_resize_cb);
   ecore_evas_callback_delete_request_set(ecore_evas, ecore_evas_hide);
   evas = ecore_evas_get(ecore_evas);

   /* Creates the evas objects */
   backdrop = evas_object_image_add(evas);
   evas_object_image_file_set(backdrop, PACKAGE_DATA_DIR "/images/backdrop.png", NULL);
   evas_object_show(backdrop);
   
   e_logo = evas_object_image_add(evas);
   evas_object_image_file_set(e_logo, PACKAGE_DATA_DIR "/images/e_logo.png", NULL);
   evas_object_show(e_logo);
   
   panel_top = evas_object_image_add(evas);
   evas_object_image_file_set(panel_top, PACKAGE_DATA_DIR "/images/panel_top.png", NULL);
   evas_object_show(panel_top);
   
   panel = evas_object_image_add(evas);
   evas_object_image_file_set(panel, PACKAGE_DATA_DIR "/images/panel.png", NULL);
   evas_object_image_size_get(panel, &w, &h);
   evas_object_image_fill_set(panel, 0, 0, w, h);
   evas_object_show(panel);
   
   panel_shadow = evas_object_image_add(evas);
   evas_object_image_file_set(panel_shadow, PACKAGE_DATA_DIR "/images/panel_shadow.png", NULL);
   evas_object_show(panel_shadow);
   
   panel_clip = evas_object_rectangle_add(evas);
   evas_object_show(panel_clip);
   
   /* Creates the embed widget */
   embed = _etk_test_embed_widget_new(evas);
   embed_object = etk_embed_object_get(ETK_EMBED(embed));
   evas_object_clip_set(embed_object, panel_clip);
   
   etk_widget_size_request(embed, &min_size);
   ecore_evas_resize(ecore_evas, min_size.w + 50, min_size.h + 120);
   ecore_evas_size_min_set(ecore_evas, min_size.w + 50, min_size.h + 120);
   
   /* Updates the position of the objects */
   starting_time = _etk_test_embed_time_get();
   _etk_test_embed_update();
   ecore_animator_add(_etk_test_embed_animator_cb, NULL);
   
   ecore_evas_show(ecore_evas);
}

/* Creates the new embed widget */
static Etk_Widget *_etk_test_embed_widget_new(Evas *evas)
{
   Etk_Widget *embed;
   Etk_Widget *table;
   Etk_Widget *image;
   Etk_Widget *buttons[3];
   Etk_Widget *labels[8];
   Etk_Widget *entries[6];
   int i;
   
   
   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/test.png");
   
   buttons[0] = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_OPEN);
   buttons[1] = etk_check_button_new();
   buttons[2] = etk_check_button_new();
   
   labels[0] = etk_label_new("App Name");
   labels[1] = etk_label_new("Generic Info");
   labels[2] = etk_label_new("Comments");
   labels[3] = etk_label_new("Executable");
   labels[4] = etk_label_new("Window Name");
   labels[5] = etk_label_new("Window Class");
   labels[6] = etk_label_new("Startup Notify");
   labels[7] = etk_label_new("Wait Exit");
   
   for (i = 0; i < 6; i++)
      entries[i] = etk_entry_new();
   

   table = etk_table_new(2, 10, ETK_FALSE);
   etk_table_attach(ETK_TABLE(table), image, 0, 0, 0, 0, 0, 0, ETK_FILL_POLICY_NONE);
   etk_table_attach(ETK_TABLE(table), buttons[0], 1, 1, 0, 0, 0, 0, ETK_FILL_POLICY_HEXPAND | ETK_FILL_POLICY_HFILL);
   
   for (i = 0; i < 6; i++)
   {
      etk_table_attach(ETK_TABLE(table), labels[i], 0, 0, 2 + i, 2 + i, 0, 0, ETK_FILL_POLICY_HFILL);
      etk_table_attach_defaults(ETK_TABLE(table), entries[i], 1, 1, 2 + i, 2 + i);
   }
   
   etk_table_attach(ETK_TABLE(table), labels[6], 0, 0, 8, 8, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), buttons[1], 1, 1, 8, 8);
   etk_table_attach(ETK_TABLE(table), labels[7], 0, 0, 9, 9, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), buttons[2], 1, 1, 9, 9);
   
   
   embed = etk_embed_new(evas);
   etk_container_add(ETK_CONTAINER(embed), table);
   etk_widget_show_all(embed);
   
   return embed;
}

/* Updates the geometry of the objects */
static void _etk_test_embed_update()
{
   double t;
   double percent;
   Etk_Size embed_size;
   int y;
   
   t = ETK_CLAMP(_etk_test_embed_time_get() - starting_time, 0.0, 2.0);
   percent = sin((t / 2.0) * (ETK_TEST_PI / 2));
   y = win_h - (0.75 * percent * win_h);
   
   
   evas_object_move(backdrop, 0, 0);
   evas_object_resize(backdrop, win_w, y);
   evas_object_image_fill_set(backdrop, 0, 0, win_w, 320);
   
   evas_object_move(e_logo, (win_w - ((win_h / 4) - 10)) / 2, 5);
   evas_object_resize(e_logo, (win_h / 4) - 10, (win_h / 4) - 10);
   evas_object_image_fill_set(e_logo, 0, 0, (win_h / 4) - 10, (win_h / 4) - 10);
   
   evas_object_move(panel_top, 0, y - 10);
   evas_object_resize(panel_top, win_w, 10);
   evas_object_image_fill_set(panel_top, 0, 0, win_w, 10);
   
   evas_object_move(panel_shadow, 0, y);
   evas_object_resize(panel_shadow, win_w, 120);
   evas_object_image_fill_set(panel_shadow, 0, 0, win_w, 120);
   
   evas_object_move(panel, 0, y);
   evas_object_resize(panel, win_w, win_h - y);
   
   evas_object_move(panel_clip, 0, y);
   evas_object_resize(panel_clip, win_w, win_h - y);
   evas_object_color_set(panel_clip, 255, 255, 255, percent * 255);
   
   etk_widget_size_request(embed, &embed_size);
   evas_object_move(embed_object, (win_w - embed_size.w) / 2, y + 20);
   evas_object_resize(embed_object, embed_size.w, embed_size.h);
}

/* Called when the window is resized */
static void _etk_test_embed_resize_cb(Ecore_Evas *ecore_evas)
{
   if (!ecore_evas)
      return;
   
   ecore_evas_geometry_get(ecore_evas, NULL, NULL, &win_w, &win_h);
   _etk_test_embed_update();
}

/* Animates the objects */
static int _etk_test_embed_animator_cb(void *data)
{
   _etk_test_embed_update();
   return 1;
}

/* Gets the current time in seconds */
static double _etk_test_embed_time_get()
{
   struct timeval timev;

   gettimeofday(&timev, NULL);
   return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);

}
