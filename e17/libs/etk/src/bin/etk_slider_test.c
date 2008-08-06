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

static Etk_Bool _slider_value_changed_cb(Etk_Object *object, double value, void *data);
static Etk_Bool _inverted_toggled_cb(Etk_Object *object, void *data);
static Etk_Bool _show_label_toggled_cb(Etk_Object *object, void *data);
static Etk_Bool _continuous_toggled_cb(Etk_Object *object, void *data);
static Etk_Bool _disabled_toggled_cb(Etk_Object *object, void *data);
static Etk_Bool _maximum_changed_cb(Etk_Object *object, double value, void *data);

static Etk_Widget *_label = NULL;
static Etk_Widget *_hslider = NULL;
static Etk_Widget *_vslider = NULL;
static Etk_Widget *_hspinner = NULL;
static Etk_Widget *_vspinner = NULL;


/**************************
 *
 * Creation of the test-app window
 *
 **************************/

/* Creates the window for the slider test */
void etk_test_slider_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *table;
   Etk_Widget *table2;
   Etk_Widget *separator;
   Etk_Widget *frame;
   Etk_Widget *hbox;
   Etk_Widget *vbox;
   Etk_Widget *check;
   Etk_Widget *label;

   if (win)
   {
      etk_widget_show(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Slider Test");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   table = etk_table_new(2, 3, ETK_TABLE_HHOMOGENEOUS);
   etk_container_add(ETK_CONTAINER(win), table);
   table2 = etk_table_new(2, 2, ETK_TABLE_NOT_HOMOGENEOUS);
   etk_table_attach_default(ETK_TABLE(table), table2, 0, 1, 0, 0);

   /* Create the main label */
   _label = etk_label_new("<title><font_size=28>128 x 128</font_size></title>");
   etk_label_alignment_set(ETK_LABEL(_label), 0.5, 0.5);
   etk_widget_size_request_set(_label, 150, 150);
   etk_table_attach_default(ETK_TABLE(table2), _label, 0, 0, 0, 0);

   /* Create the horizontal slider */
   _hslider = etk_hslider_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_slider_label_set(ETK_SLIDER(_hslider), "%.0f");
   etk_table_attach(ETK_TABLE(table2), _hslider, 0, 0, 1, 1, ETK_TABLE_HEXPAND | ETK_TABLE_HFILL, 0, 0);
   etk_signal_connect_by_code(ETK_RANGE_VALUE_CHANGED_SIGNAL, ETK_OBJECT(_hslider), ETK_CALLBACK(_slider_value_changed_cb), NULL);

   /* Create the vertical slider */
   _vslider = etk_vslider_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_slider_label_set(ETK_SLIDER(_vslider), "%.0f");
   etk_table_attach(ETK_TABLE(table2), _vslider, 1, 1, 0, 0, ETK_TABLE_VEXPAND | ETK_TABLE_VFILL, 0, 0);
   etk_signal_connect_by_code(ETK_RANGE_VALUE_CHANGED_SIGNAL, ETK_OBJECT(_vslider), ETK_CALLBACK(_slider_value_changed_cb), NULL);

   separator = etk_hseparator_new();
   etk_table_attach(ETK_TABLE(table), separator, 0, 1, 1, 1, ETK_TABLE_HFILL, 0, 0);


   /* Create the settings frame for the horizontal slider */
   frame = etk_frame_new("HSlider Properties");
   etk_table_attach(ETK_TABLE(table), frame, 0, 0, 2, 2, ETK_TABLE_HEXPAND | ETK_TABLE_FILL, 0, 0);
   vbox = etk_vbox_new(ETK_TRUE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox);

   check = etk_check_button_new_with_label("Show Label");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect_by_code(ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL, ETK_OBJECT(check), ETK_CALLBACK(_show_label_toggled_cb), _hslider);

   check = etk_check_button_new_with_label("Inverted");
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect_by_code(ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL, ETK_OBJECT(check), ETK_CALLBACK(_inverted_toggled_cb), _hslider);

   check = etk_check_button_new_with_label("Continuous Update");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect_by_code(ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL, ETK_OBJECT(check), ETK_CALLBACK(_continuous_toggled_cb), _hslider);

   check = etk_check_button_new_with_label("Disabled");
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect_by_code(ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL, ETK_OBJECT(check), ETK_CALLBACK(_disabled_toggled_cb), _hslider);

   hbox = etk_hbox_new(ETK_FALSE, 5);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   label = etk_label_new("Maximum:");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   _hspinner = etk_spinner_new(10.0, 1000.0, 255.0, 1.0, 10.0);
   etk_box_append(ETK_BOX(hbox), _hspinner, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect_by_code(ETK_RANGE_VALUE_CHANGED_SIGNAL, ETK_OBJECT(_hspinner), ETK_CALLBACK(_maximum_changed_cb), _hslider);


   /* Create the settings frame for the vertical slider */
   frame = etk_frame_new("VSlider Properties");
   etk_table_attach(ETK_TABLE(table), frame, 1, 1, 2, 2, ETK_TABLE_HEXPAND | ETK_TABLE_FILL, 0, 0);
   vbox = etk_vbox_new(ETK_TRUE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox);

   check = etk_check_button_new_with_label("Show Label");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect_by_code(ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL, ETK_OBJECT(check), ETK_CALLBACK(_show_label_toggled_cb), _vslider);

   check = etk_check_button_new_with_label("Inverted");
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect_by_code(ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL, ETK_OBJECT(check), ETK_CALLBACK(_inverted_toggled_cb), _vslider);

   check = etk_check_button_new_with_label("Continuous Update");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect_by_code(ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL, ETK_OBJECT(check), ETK_CALLBACK(_continuous_toggled_cb), _vslider);

   check = etk_check_button_new_with_label("Disabled");
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect_by_code(ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL, ETK_OBJECT(check), ETK_CALLBACK(_disabled_toggled_cb), _vslider);

   hbox = etk_hbox_new(ETK_FALSE, 5);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   label = etk_label_new("Maximum:");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   _vspinner = etk_spinner_new(10.0, 1000.0, 255.0, 1.0, 10.0);
   etk_box_append(ETK_BOX(hbox), _vspinner, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect_by_code(ETK_RANGE_VALUE_CHANGED_SIGNAL, ETK_OBJECT(_vspinner), ETK_CALLBACK(_maximum_changed_cb), _vslider);


   etk_widget_show_all(win);
}

/**************************
 *
 * Callbacks
 *
 **************************/

/* Called when one of the slider's value has been changed */
static Etk_Bool _slider_value_changed_cb(Etk_Object *object, double value, void *data)
{
   char buf[128];

   snprintf(buf, sizeof(buf), "<title><font_size=28>%.0f x %.0f</font_size></title>",
      etk_range_value_get(ETK_RANGE(_hslider)), etk_range_value_get(ETK_RANGE(_vslider)));
   etk_label_set(ETK_LABEL(_label), buf);
   return ETK_TRUE;
}

/* Called when one of the "Inverted" check buttons is toggled */
static Etk_Bool _inverted_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *toggle;
   Etk_Slider *slider;

   if (!(toggle = ETK_TOGGLE_BUTTON(object)) || !(slider = ETK_SLIDER(data)))
      return ETK_TRUE;
   etk_slider_inverted_set(slider, etk_toggle_button_active_get(toggle));
   return ETK_TRUE;
}

/* Called when one of the "Show Label" check buttons is toggled */
static Etk_Bool _show_label_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *toggle;
   Etk_Slider *slider;

   if (!(toggle = ETK_TOGGLE_BUTTON(object)) || !(slider = ETK_SLIDER(data)))
      return ETK_TRUE;

   if (etk_toggle_button_active_get(toggle))
      etk_slider_label_set(slider, "%.0f");
   else
      etk_slider_label_set(slider, NULL);
   return ETK_TRUE;
}

/* Called when one of the "Continuous Update" check buttons is toggled */
static Etk_Bool _continuous_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *toggle;
   Etk_Slider *slider;

   if (!(toggle = ETK_TOGGLE_BUTTON(object)) || !(slider = ETK_SLIDER(data)))
      return ETK_TRUE;

   if (etk_toggle_button_active_get(toggle))
      etk_slider_update_policy_set(slider, ETK_SLIDER_CONTINUOUS);
   else
      etk_slider_update_policy_set(slider, ETK_SLIDER_DELAYED);
   return ETK_TRUE;
}

/* Called when one of the "Disabled" check buttons is toggled */
static Etk_Bool _disabled_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *toggle;
   Etk_Widget *slider;
   Etk_Bool disabled;

   if (!(toggle = ETK_TOGGLE_BUTTON(object)) || !(slider = ETK_WIDGET(data)))
      return ETK_TRUE;

   disabled = etk_toggle_button_active_get(toggle);
   if (slider == _hslider)
   {
      etk_widget_disabled_set(_hslider, disabled);
      etk_widget_disabled_set(_hspinner, disabled);
   }
   else
   {
      etk_widget_disabled_set(_vslider, disabled);
      etk_widget_disabled_set(_vspinner, disabled);
   }
   return ETK_TRUE;
}

/* Called when the value of one the "Maximum" spinners is changed */
static Etk_Bool _maximum_changed_cb(Etk_Object *object, double value, void *data)
{
   Etk_Range *range;

   if (!(range = ETK_RANGE(data)))
      return ETK_TRUE;
   etk_range_range_set(range, 0.0, value);
   return ETK_TRUE;
}
