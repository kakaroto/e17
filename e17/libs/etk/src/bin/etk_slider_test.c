#include "etk_test.h"

static void _slider_value_changed_cb(Etk_Object *object, double value, void *data);
static void _inverted_toggled_cb(Etk_Object *object, void *data);
static void _show_label_toggled_cb(Etk_Object *object, void *data);
static void _continuous_toggled_cb(Etk_Object *object, void *data);
static void _disabled_toggled_cb(Etk_Object *object, void *data);
static void _maximum_changed_cb(Etk_Object *object, double value, void *data);

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
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   table = etk_table_new(2, 3, ETK_FALSE);
   etk_container_add(ETK_CONTAINER(win), table);
   table2 = etk_table_new(2, 2, ETK_FALSE);
   etk_table_attach_default(ETK_TABLE(table), table2, 0, 1, 0, 0);
   
   /* Create the main label */
   _label = etk_label_new("<title><font_size=28>128 x 128</font_size></title>");
   etk_label_alignment_set(ETK_LABEL(_label), 0.5, 0.5);
   etk_widget_size_request_set(_label, 150, 150);
   etk_table_attach_default(ETK_TABLE(table2), _label, 0, 0, 0, 0);
   
   /* Create the horizontal slider */
   _hslider = etk_hslider_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_slider_label_set(ETK_SLIDER(_hslider), "%.0f");
   etk_table_attach(ETK_TABLE(table2), _hslider, 0, 0, 1, 1, 0, 0, ETK_TABLE_HEXPAND | ETK_TABLE_HFILL);
   etk_signal_connect("value_changed", ETK_OBJECT(_hslider), ETK_CALLBACK(_slider_value_changed_cb), NULL);
   
   /* Create the vertical slider */
   _vslider = etk_vslider_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_slider_label_set(ETK_SLIDER(_vslider), "%.0f");
   etk_table_attach(ETK_TABLE(table2), _vslider, 1, 1, 0, 0, 0, 0, ETK_TABLE_VEXPAND | ETK_TABLE_VFILL);
   etk_signal_connect("value_changed", ETK_OBJECT(_vslider), ETK_CALLBACK(_slider_value_changed_cb), NULL);
   
   separator = etk_hseparator_new();
   etk_table_attach(ETK_TABLE(table), separator, 0, 1, 1, 1, 0, 0, ETK_TABLE_HFILL);
   
   
   /* Create the settings frame for the horizontal slider */
   frame = etk_frame_new("HSlider Properties");
   etk_table_attach(ETK_TABLE(table), frame, 0, 0, 2, 2, 0, 0, ETK_TABLE_HEXPAND | ETK_TABLE_FILL);
   vbox = etk_vbox_new(ETK_TRUE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox);
   
   check = etk_check_button_new_with_label("Show Label");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check), ETK_CALLBACK(_show_label_toggled_cb), _hslider);
   
   check = etk_check_button_new_with_label("Inverted");
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check), ETK_CALLBACK(_inverted_toggled_cb), _hslider);
   
   check = etk_check_button_new_with_label("Continuous Update");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check), ETK_CALLBACK(_continuous_toggled_cb), _hslider);
   
   check = etk_check_button_new_with_label("Disabled");
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check), ETK_CALLBACK(_disabled_toggled_cb), _hslider);
   
   hbox = etk_hbox_new(ETK_FALSE, 5);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   label = etk_label_new("Maximum:");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   _hspinner = etk_spinner_new(10.0, 1000.0, 255.0, 1.0, 10.0);
   etk_box_append(ETK_BOX(hbox), _hspinner, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect("value_changed", ETK_OBJECT(_hspinner), ETK_CALLBACK(_maximum_changed_cb), _hslider);
   
   
   /* Create the settings frame for the vertical slider */
   frame = etk_frame_new("VSlider Properties");
   etk_table_attach(ETK_TABLE(table), frame, 1, 1, 2, 2, 0, 0, ETK_TABLE_HEXPAND | ETK_TABLE_FILL);
   vbox = etk_vbox_new(ETK_TRUE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox);
   
   check = etk_check_button_new_with_label("Show Label");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check), ETK_CALLBACK(_show_label_toggled_cb), _vslider);
   
   check = etk_check_button_new_with_label("Inverted");
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check), ETK_CALLBACK(_inverted_toggled_cb), _vslider);
   
   check = etk_check_button_new_with_label("Continuous Update");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check), ETK_CALLBACK(_continuous_toggled_cb), _vslider);
   
   check = etk_check_button_new_with_label("Disabled");
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check), ETK_CALLBACK(_disabled_toggled_cb), _vslider);
   
   hbox = etk_hbox_new(ETK_FALSE, 5);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   label = etk_label_new("Maximum:");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   _vspinner = etk_spinner_new(10.0, 1000.0, 255.0, 1.0, 10.0);
   etk_box_append(ETK_BOX(hbox), _vspinner, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect("value_changed", ETK_OBJECT(_vspinner), ETK_CALLBACK(_maximum_changed_cb), _vslider);
   
   
   etk_widget_show_all(win);
}

/**************************
 *
 * Callbacks
 *
 **************************/

/* Called when one of the slider's value has been changed */
static void _slider_value_changed_cb(Etk_Object *object, double value, void *data)
{
   char buf[128];
   
   snprintf(buf, sizeof(buf), "<title><font_size=28>%.0f x %.0f</font_size></title>",
      etk_range_value_get(ETK_RANGE(_hslider)), etk_range_value_get(ETK_RANGE(_vslider)));
   etk_label_set(ETK_LABEL(_label), buf);
}

/* Called when one of the "Inverted" check buttons is toggled */
static void _inverted_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *toggle;
   Etk_Slider *slider;
   
   if (!(toggle = ETK_TOGGLE_BUTTON(object)) || !(slider = ETK_SLIDER(data)))
      return;
   etk_slider_inverted_set(slider, etk_toggle_button_active_get(toggle));
}

/* Called when one of the "Show Label" check buttons is toggled */
static void _show_label_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *toggle;
   Etk_Slider *slider;
   
   if (!(toggle = ETK_TOGGLE_BUTTON(object)) || !(slider = ETK_SLIDER(data)))
      return;
   
   if (etk_toggle_button_active_get(toggle))
      etk_slider_label_set(slider, "%.0f");
   else
      etk_slider_label_set(slider, NULL);
}

/* Called when one of the "Continuous Update" check buttons is toggled */
static void _continuous_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *toggle;
   Etk_Slider *slider;
   
   if (!(toggle = ETK_TOGGLE_BUTTON(object)) || !(slider = ETK_SLIDER(data)))
      return;
   
   if (etk_toggle_button_active_get(toggle))
      etk_slider_update_policy_set(slider, ETK_SLIDER_CONTINUOUS);
   else
      etk_slider_update_policy_set(slider, ETK_SLIDER_DELAYED);
}

/* Called when one of the "Disabled" check buttons is toggled */
static void _disabled_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *toggle;
   Etk_Widget *slider;
   Etk_Bool disabled;
   
   if (!(toggle = ETK_TOGGLE_BUTTON(object)) || !(slider = ETK_WIDGET(data)))
      return;
   
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
}

/* Called when the value of one the "Maximum" spinners is changed */
static void _maximum_changed_cb(Etk_Object *object, double value, void *data)
{
   Etk_Range *range;
   
   if (!(range = ETK_RANGE(data)))
      return;
   etk_range_range_set(range, 0.0, value);
}
