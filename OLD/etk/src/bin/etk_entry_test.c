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

static Etk_Bool _normal_print_cb(Etk_Object *object, void *data);
static Etk_Bool _password_show_cb(Etk_Object *object, void *data);

static Etk_Widget *_label_normal = NULL;
static Etk_Widget *_entry_normal = NULL;

/**************************
 *
 * Creation of the test-app window
 *
 **************************/

/* Creates the window for the entry test */
void etk_test_entry_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *vbox2;
   Etk_Widget *frame;
   Etk_Widget *separator;
   Etk_Widget *button;
   Etk_Widget *entry;
   Etk_Widget *image;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Entry Test");
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);


   /*******************
    * Normal entry
    *******************/
   frame = etk_frame_new("Normal Entry");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);

   _entry_normal = etk_entry_new();
   etk_entry_text_set(ETK_ENTRY(_entry_normal), "Here is some text");
   etk_box_append(ETK_BOX(vbox2), _entry_normal, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_widget_size_request_set(_entry_normal, 140, -1);

   image = etk_image_new_from_stock(ETK_STOCK_DOCUMENT_PRINT, ETK_STOCK_SMALL);
   etk_entry_image_set(ETK_ENTRY(_entry_normal), ETK_ENTRY_IMAGE_PRIMARY, ETK_IMAGE(image));
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_CLICK_SIGNAL, ETK_OBJECT(image), ETK_CALLBACK(_normal_print_cb), NULL);
   etk_entry_clear_button_add(ETK_ENTRY(_entry_normal));

   _label_normal = etk_label_new(NULL);
   etk_box_append(ETK_BOX(vbox2), _label_normal, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   separator = etk_hseparator_new();
   etk_box_append(ETK_BOX(vbox), separator, ETK_BOX_START, ETK_BOX_NONE, 6);


   /*******************
    * Disabled entry
    *******************/
   frame = etk_frame_new("Disabled Entry");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   entry = etk_entry_new();
   etk_widget_disabled_set(entry, ETK_TRUE);
   etk_entry_text_set(ETK_ENTRY(entry), "The disabled text");
   etk_entry_clear_button_add(ETK_ENTRY(entry));
   etk_container_add(ETK_CONTAINER(frame), entry);

   separator = etk_hseparator_new();
   etk_box_append(ETK_BOX(vbox), separator, ETK_BOX_START, ETK_BOX_NONE, 6);


   /*******************
    * Password entry
    *******************/
   frame = etk_frame_new("Password Entry");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);

   entry = etk_entry_new();
   etk_entry_text_set(ETK_ENTRY(entry), "Password");
   etk_entry_password_mode_set(ETK_ENTRY(entry), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox2), entry, ETK_BOX_START, ETK_BOX_FILL, 0);

   button = etk_check_button_new_with_label("Password Visible");
   etk_box_append(ETK_BOX(vbox2), button, ETK_BOX_START, ETK_BOX_FILL, 0);
   etk_signal_connect_by_code(ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL, ETK_OBJECT(button), ETK_CALLBACK(_password_show_cb), entry);

   etk_widget_show_all(win);
}

/**************************
 *
 * Callbacks
 *
 **************************/

/* Prints the text of the normal entry in the label when the "print" icon is clicked */
static Etk_Bool _normal_print_cb(Etk_Object *object, void *data)
{
   etk_label_set(ETK_LABEL(_label_normal), etk_entry_text_get(ETK_ENTRY(_entry_normal)));
   return ETK_TRUE;
}

/* Toggles the password mode of the password entry when the "Password Visible" checkbox is toggled */
static Etk_Bool _password_show_cb(Etk_Object *object, void *data)
{
   etk_entry_password_mode_set(ETK_ENTRY(data), !etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(object)));
   return ETK_TRUE;
}
