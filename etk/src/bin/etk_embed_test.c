/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "etk_test.h"
#include <math.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include <Ecore_X_Cursor.h>
#include <Evas.h>
#include "config.h"

/* Test program *highly* inspired from the Evas test program */

static void _embed_resize_cb(Ecore_Evas *ecore_evas);
static int _embed_animator_cb(void *data);
static Etk_Widget *_embed_widget_new(Evas *evas);
static void _embed_update(void);
static void _embed_position_set(void *position_data, int *x, int *y);
static void _embed_pointer_set(void *pointer_data, Etk_Pointer_Type pointer_type);

static Ecore_Evas *ecore_evas = NULL;
static Evas_Object *e_logo, *backdrop;
static Evas_Object *panel, *panel_top, *panel_shadow;
static Evas_Object *panel_clip;
static Evas_Object *embed_object;
static Etk_Widget *embed;
static int win_w = 240, win_h = 320;
static unsigned int starting_time;

/**************************
 *
 * Creation of the test-app window
 *
 **************************/

/* Creates the window for the embed widget test */
void etk_test_embed_window_create(void *data)
{
   Evas *evas;
   int w, h;
   Etk_Size min_size;

   if (ecore_evas)
   {
      starting_time = etk_current_time_get();
      _embed_update();
      ecore_evas_show(ecore_evas);
      return;
   }

   ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, 32, 32);
   ecore_evas_title_set(ecore_evas, "Etk Embed Test");
   ecore_evas_callback_resize_set(ecore_evas, _embed_resize_cb);
   ecore_evas_callback_delete_request_set(ecore_evas, ecore_evas_hide);
   evas = ecore_evas_get(ecore_evas);

   /* Create the Evas objects */
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

   /* Create the embed widget */
   embed = _embed_widget_new(evas);
   etk_embed_position_method_set(ETK_EMBED(embed), _embed_position_set, ecore_evas);
   etk_embed_pointer_method_set(ETK_EMBED(embed), _embed_pointer_set, ecore_evas);
   embed_object = etk_embed_object_get(ETK_EMBED(embed));
   evas_object_clip_set(embed_object, panel_clip);

   etk_widget_size_request(embed, &min_size);
   ecore_evas_resize(ecore_evas, min_size.w + 50, min_size.h + 120);
   ecore_evas_size_min_set(ecore_evas, min_size.w + 50, min_size.h + 120);

   /* Update the position of the objects */
   starting_time = etk_current_time_get();
   _embed_update();
   ecore_animator_add(_embed_animator_cb, NULL);

   ecore_evas_show(ecore_evas);
}

/**************************
 *
 * Callbacks
 *
 **************************/

/* Called when the window is resized */
static void _embed_resize_cb(Ecore_Evas *ecore_evas)
{
   if (!ecore_evas)
      return;

   ecore_evas_geometry_get(ecore_evas, NULL, NULL, &win_w, &win_h);
   _embed_update();
}

/* Animates the objects */
static int _embed_animator_cb(void *data)
{
   _embed_update();
   return 1;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Creates the new embed widget */
static Etk_Widget *_embed_widget_new(Evas *evas)
{
   Etk_Widget *embed;
   Etk_Widget *table;
   Etk_Widget *image;
   Etk_Widget *combobox;
   Etk_Widget *buttons[2];
   Etk_Widget *labels[8];
   Etk_Widget *entries[6];
   int i;

   /* Create the children of the embed widget:
    * Here we create the same table as in the "Table" test-app */
   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/test.png", NULL);

   combobox = etk_combobox_new_default();
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Emphasis");
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Exhibit");
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Entropy");

   buttons[0] = etk_check_button_new();
   buttons[1] = etk_check_button_new();

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

   table = etk_table_new(2, 10, ETK_TABLE_NOT_HOMOGENEOUS);
   etk_table_attach(ETK_TABLE(table), image, 0, 0, 0, 0, ETK_TABLE_NONE, 0, 0);
   etk_table_attach(ETK_TABLE(table), combobox, 1, 1, 0, 0, ETK_TABLE_HEXPAND, 0, 0);

   for (i = 0; i < 6; i++)
   {
      etk_table_attach(ETK_TABLE(table), labels[i], 0, 0, 2 + i, 2 + i, ETK_TABLE_HFILL, 0, 0);
      etk_table_attach_default(ETK_TABLE(table), entries[i], 1, 1, 2 + i, 2 + i);
   }

   etk_table_attach(ETK_TABLE(table), labels[6], 0, 0, 8, 8, ETK_TABLE_HFILL, 0, 0);
   etk_table_attach_default(ETK_TABLE(table), buttons[0], 1, 1, 8, 8);
   etk_table_attach(ETK_TABLE(table), labels[7], 0, 0, 9, 9, ETK_TABLE_HFILL, 0, 0);
   etk_table_attach_default(ETK_TABLE(table), buttons[1], 1, 1, 9, 9);


   /* Create the embed widget and pack the table into it */
   embed = etk_embed_new(evas);
   etk_container_add(ETK_CONTAINER(embed), table);
   etk_widget_show_all(embed);

   return embed;
}

/* Updates the geometry of the different Evas objects */
static void _embed_update(void)
{
   double t;
   int alpha;
   Etk_Size embed_size;
   int y;

   /* Calculate the current progress */
   t = ETK_MAX(0.0, etk_current_time_get() - starting_time) / 1000.0;
   if (t <= 2.0)
      y = win_h - (0.75 * sin((t / 2.0) * (M_PI / 2)) * win_h);
   else
      y = 0.25 * win_h;
   alpha = ETK_CLAMP((t - 1.0) / 2.0, 0.0, 1.0) * 255;

   /* Update the different object according to the progress */
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
   evas_object_color_set(panel_clip, alpha, alpha, alpha, alpha);

   etk_widget_size_request(embed, &embed_size);
   evas_object_move(embed_object, (win_w - embed_size.w) / 2, y + 20);
   evas_object_resize(embed_object, embed_size.w, embed_size.h);
}

/* Gets the position of the Evas where the embed object belongs, relative to the screen */
static void _embed_position_set(void *position_data, int *x, int *y)
{
   ecore_evas_geometry_get(position_data, x, y, NULL, NULL);
}

/* Sets the pointer of the embed widget */
static void _embed_pointer_set(void *pointer_data, Etk_Pointer_Type pointer_type)
{
   Ecore_Evas *ee;
   Ecore_X_Window window;
   Ecore_X_Cursor cursor;
   int x_pointer_type;

   if (!(ee = pointer_data) || (window = ecore_evas_software_x11_window_get(ee)) == 0)
      return;

   /* We only need to support the "text_edit" and the default cursors here
    * since we only have entries in the embed widget */
   switch (pointer_type)
   {
      case ETK_POINTER_TEXT_EDIT:
         x_pointer_type = ECORE_X_CURSOR_XTERM;
         break;
      default:
         x_pointer_type = ECORE_X_CURSOR_LEFT_PTR;
         break;
   }

   if (pointer_type == ETK_POINTER_NONE)
      ecore_x_window_cursor_set(window, 0);
   else if ((cursor = ecore_x_cursor_shape_get(x_pointer_type)))
      ecore_x_window_cursor_set(window, cursor);
   else
      ETK_WARNING("Unable to find the X cursor \"%d\"", pointer_type);
}
