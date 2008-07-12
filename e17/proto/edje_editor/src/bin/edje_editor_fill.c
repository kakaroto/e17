#include <string.h>
#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


Etk_Widget*
fill_frame_create(void)
{
   Etk_Widget *vbox, *hbox;
   Etk_Widget *label;
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   
   //hbox (origin)
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   label = etk_label_new("<b>Origin X </b>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_FillRelXSpinner
   UI_FillRelXSpinner = etk_spinner_new(-100.0, 100.0, 0.0, 0.01, 0.1);
   etk_spinner_digits_set(ETK_SPINNER(UI_FillRelXSpinner), 2);
   etk_widget_size_request_set(UI_FillRelXSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_FillRelXSpinner,
                  ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("+");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_FillOffsetXSpinner
   UI_FillOffsetXSpinner = etk_spinner_new(-2000, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_FillOffsetXSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_FillOffsetXSpinner,
                  ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<b>  Y </b>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_FillRelYSpinner
   UI_FillRelYSpinner = etk_spinner_new(-100.0, 100.0, 0.0, 0.01, 0.1);
   etk_spinner_digits_set(ETK_SPINNER(UI_FillRelYSpinner), 2);
   etk_widget_size_request_set(UI_FillRelYSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_FillRelYSpinner,
                  ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("+");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_FillOffsetYSpinner
   UI_FillOffsetYSpinner = etk_spinner_new(-2000, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_FillOffsetYSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_FillOffsetYSpinner,
                  ETK_BOX_START, ETK_BOX_NONE, 0);
   
   
   //hbox (size)
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   label = etk_label_new("<b>Size    X  </b>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_FillSizeRelXSpinner
   UI_FillSizeRelXSpinner = etk_spinner_new(-100.0, 100.0, 0.0, 0.01, 0.1);
   etk_spinner_digits_set(ETK_SPINNER(UI_FillSizeRelXSpinner), 2);
   etk_widget_size_request_set(UI_FillSizeRelXSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_FillSizeRelXSpinner,
                  ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("+");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_FillSizeOffsetXSpinner
   UI_FillSizeOffsetXSpinner = etk_spinner_new(-2000, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_FillSizeOffsetXSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_FillSizeOffsetXSpinner,
                  ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<b>  Y </b>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_FillSizeRelYSpinner
   UI_FillSizeRelYSpinner = etk_spinner_new(-100.0, 100.0, 0.0, 0.01, 0.1);
   etk_spinner_digits_set(ETK_SPINNER(UI_FillSizeRelYSpinner), 2);
   etk_widget_size_request_set(UI_FillSizeRelYSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_FillSizeRelYSpinner,
                  ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("+");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_FillSizeOffsetYSpinner
   UI_FillSizeOffsetYSpinner = etk_spinner_new(-2000, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_FillSizeOffsetYSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_FillSizeOffsetYSpinner,
                  ETK_BOX_START, ETK_BOX_NONE, 0);
   
   
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FillRelXSpinner),
                      ETK_CALLBACK(_fill_spinners_value_changed_cb),
                      (void *)REL1X_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FillRelYSpinner),
                      ETK_CALLBACK(_fill_spinners_value_changed_cb),
                      (void *)REL1Y_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FillOffsetXSpinner),
                      ETK_CALLBACK(_fill_spinners_value_changed_cb),
                      (void *)REL1XO_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FillOffsetYSpinner),
                      ETK_CALLBACK(_fill_spinners_value_changed_cb),
                      (void *)REL1YO_SPINNER);

   etk_signal_connect("value-changed", ETK_OBJECT(UI_FillSizeRelXSpinner),
                      ETK_CALLBACK(_fill_size_spinners_value_changed_cb),
                      (void *)REL1X_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FillSizeRelYSpinner),
                      ETK_CALLBACK(_fill_size_spinners_value_changed_cb),
                      (void *)REL1Y_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FillSizeOffsetXSpinner),
                      ETK_CALLBACK(_fill_size_spinners_value_changed_cb),
                      (void *)REL1XO_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FillSizeOffsetYSpinner),
                      ETK_CALLBACK(_fill_size_spinners_value_changed_cb),
                      (void *)REL1YO_SPINNER);

   
   return vbox;
}
void
fill_frame_update(void)
{
   if (!etk_string_length_get(Cur.part)) return;
   if (!etk_string_length_get(Cur.state)) return;
   
   //Block Signal
   etk_signal_block("value-changed", ETK_OBJECT(UI_FillRelXSpinner),
                    ETK_CALLBACK(_fill_spinners_value_changed_cb),
                    (void *)REL1X_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_FillRelYSpinner),
                    ETK_CALLBACK(_fill_spinners_value_changed_cb),
                    (void *)REL1Y_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_FillOffsetXSpinner),
                    ETK_CALLBACK(_fill_spinners_value_changed_cb),
                    (void *)REL1XO_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_FillOffsetYSpinner),
                    ETK_CALLBACK(_fill_spinners_value_changed_cb),
                    (void *)REL1YO_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_FillSizeRelXSpinner),
                    ETK_CALLBACK(_fill_size_spinners_value_changed_cb),
                    (void *)REL1X_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_FillSizeRelYSpinner),
                    ETK_CALLBACK(_fill_size_spinners_value_changed_cb),
                    (void *)REL1Y_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_FillSizeOffsetXSpinner),
                    ETK_CALLBACK(_fill_size_spinners_value_changed_cb),
                    (void *)REL1XO_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_FillSizeOffsetYSpinner),
                    ETK_CALLBACK(_fill_size_spinners_value_changed_cb),
                    (void *)REL1YO_SPINNER);
   
   //Update origin spinners
   etk_range_value_set(ETK_RANGE(UI_FillRelXSpinner),
      edje_edit_state_fill_origin_relative_x_get(edje_o, Cur.part->string,
                                                 Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_FillRelYSpinner),
      edje_edit_state_fill_origin_relative_y_get(edje_o, Cur.part->string,
                                                 Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_FillOffsetXSpinner),
      edje_edit_state_fill_origin_offset_x_get(edje_o, Cur.part->string,
                                               Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_FillOffsetYSpinner),
      edje_edit_state_fill_origin_offset_y_get(edje_o, Cur.part->string,
                                               Cur.state->string));
   
   //Update size spinners
   etk_range_value_set(ETK_RANGE(UI_FillSizeRelXSpinner),
      edje_edit_state_fill_size_relative_x_get(edje_o, Cur.part->string,
                                                 Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_FillSizeRelYSpinner),
      edje_edit_state_fill_size_relative_y_get(edje_o, Cur.part->string,
                                                 Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_FillSizeOffsetXSpinner),
      edje_edit_state_fill_size_offset_x_get(edje_o, Cur.part->string,
                                               Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_FillSizeOffsetYSpinner),
      edje_edit_state_fill_size_offset_y_get(edje_o, Cur.part->string,
                                               Cur.state->string));
   
   //UnBlock Signals
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_FillRelXSpinner),
                      ETK_CALLBACK(_fill_spinners_value_changed_cb),
                      (void *)REL1X_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_FillRelYSpinner),
                      ETK_CALLBACK(_fill_spinners_value_changed_cb),
                      (void *)REL1Y_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_FillOffsetXSpinner),
                      ETK_CALLBACK(_fill_spinners_value_changed_cb),
                      (void *)REL1XO_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_FillOffsetYSpinner),
                      ETK_CALLBACK(_fill_spinners_value_changed_cb),
                      (void *)REL1YO_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_FillSizeRelXSpinner),
                      ETK_CALLBACK(_fill_size_spinners_value_changed_cb),
                      (void *)REL1X_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_FillSizeRelYSpinner),
                      ETK_CALLBACK(_fill_size_spinners_value_changed_cb),
                      (void *)REL1Y_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_FillSizeOffsetXSpinner),
                      ETK_CALLBACK(_fill_size_spinners_value_changed_cb),
                      (void *)REL1XO_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_FillSizeOffsetYSpinner),
                      ETK_CALLBACK(_fill_size_spinners_value_changed_cb),
                      (void *)REL1YO_SPINNER);
}


Etk_Bool
_fill_spinners_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on Fill Origin Spinners EMITTED (value: %f)\n",etk_range_value_get(range));

   if (!etk_string_length_get(Cur.state) || !etk_string_length_get(Cur.part))
      return ETK_TRUE;
   
   switch ((int)data)
   {
      case REL1X_SPINNER:
         edje_edit_state_fill_size_relative_x_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL1Y_SPINNER:
         edje_edit_state_fill_size_relative_y_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL1XO_SPINNER:
         edje_edit_state_fill_size_offset_x_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL1YO_SPINNER:
         edje_edit_state_fill_size_offset_y_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      default:
         break;
   }

   return ETK_TRUE;
}
Etk_Bool
_fill_size_spinners_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on Fill Size Spinners EMITTED (value: %f)\n",etk_range_value_get(range));

   if (!etk_string_length_get(Cur.state) || !etk_string_length_get(Cur.part))
      return ETK_TRUE;
   
   switch ((int)data)
   {
      case REL1X_SPINNER:
         edje_edit_state_fill_origin_relative_x_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL1Y_SPINNER:
         edje_edit_state_fill_origin_relative_y_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL1XO_SPINNER:
         edje_edit_state_fill_origin_offset_x_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL1YO_SPINNER:
         edje_edit_state_fill_origin_offset_y_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      default:
         break;
   }
   return ETK_TRUE;
}
