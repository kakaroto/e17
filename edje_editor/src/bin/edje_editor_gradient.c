/*  Copyright (C) 2006-2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of Edje_editor.
 *  Edje_editor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Edje_editor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Edje_editor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


/***   Implementation   ***/
Etk_Widget*
gradient_frame_create(void)
{
   Etk_Widget *table;
   Etk_Widget *label;
   Etk_Widget *hbox;
   Etk_Combobox_Item *item;

   //table
   table = etk_table_new(2, 5, ETK_TABLE_NOT_HOMOGENEOUS);

   //UI_GradientTypeComboBox
   label = etk_label_new("<b>Type</b>");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, ETK_TABLE_NONE, 0, 0);

   UI_GradientTypeComboBox = etk_combobox_new();
   etk_combobox_items_height_set(ETK_COMBOBOX(UI_GradientTypeComboBox), 24);
   etk_combobox_column_add(ETK_COMBOBOX(UI_GradientTypeComboBox),
                           ETK_COMBOBOX_IMAGE, 27, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_GradientTypeComboBox),
                           ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_EXPAND_FILL, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_GradientTypeComboBox));
   etk_table_attach_default(ETK_TABLE(table), UI_GradientTypeComboBox, 1, 1, 0, 0);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_GradientTypeComboBox),
                  etk_image_new_from_edje(EdjeFile,"GRAD_LINEAR.PNG"), "linear");
   item = etk_combobox_item_append(ETK_COMBOBOX(UI_GradientTypeComboBox),
                  etk_image_new_from_edje(EdjeFile,"GRAD_RADIAL.PNG"), "radial");
   item = etk_combobox_item_append(ETK_COMBOBOX(UI_GradientTypeComboBox),
                  etk_image_new_from_edje(EdjeFile,"GRAD_RECTANGULAR.PNG"), "rectangular");
   item = etk_combobox_item_append(ETK_COMBOBOX(UI_GradientTypeComboBox),
                  etk_image_new_from_edje(EdjeFile,"GRAD_ANGULAR.PNG"), "angular");
   item = etk_combobox_item_append(ETK_COMBOBOX(UI_GradientTypeComboBox),
                  etk_image_new_from_edje(EdjeFile,"GRAD_SINUSOIDAL.PNG"), "sinusoidal");

   //UI_GradientSpectraComboBox
   label = etk_label_new("<b>Spectra</b>");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1, ETK_TABLE_NONE, 0, 0);

   UI_GradientSpectraComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_GradientSpectraComboBox),
                           ETK_COMBOBOX_OTHER, 45, ETK_COMBOBOX_NONE, 0.5);
   etk_combobox_column_add(ETK_COMBOBOX(UI_GradientSpectraComboBox),
                           ETK_COMBOBOX_LABEL, 100, ETK_COMBOBOX_EXPAND_FILL, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_GradientSpectraComboBox));
   etk_table_attach_default(ETK_TABLE(table), UI_GradientSpectraComboBox, 1, 1, 1, 1);

   //GradientAlphaSlider
   label = etk_label_new("<b>Alpha</b>");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 2, 2,  ETK_TABLE_NONE, 0, 0);
   UI_GradientAlphaSlider = etk_hslider_new(0, 255, 15, 1,20);
   etk_slider_label_set(ETK_SLIDER(UI_GradientAlphaSlider), "%.0f");
   etk_table_attach_default(ETK_TABLE(table), UI_GradientAlphaSlider, 1, 1, 2, 2);
   
   //GradientAngleSlider
   label = etk_label_new("<b>Angle</b>");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 3, 3,  ETK_TABLE_NONE, 0, 0);
   UI_GradientAngleSlider = etk_hslider_new(0, 360, 15, 1,20);
   etk_slider_label_set(ETK_SLIDER(UI_GradientAngleSlider), "%.0f");
   etk_table_attach_default(ETK_TABLE(table), UI_GradientAngleSlider, 1, 1, 3, 3);

   //~ //hbox (first handler)
   //~ hbox = etk_hbox_new(ETK_FALSE, 0);
   //~ etk_table_attach_default(ETK_TABLE(table), hbox, 0, 2, 3, 3);
   
   //~ label = etk_label_new("<b>Start  X </b>");
   //~ etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //~ //UI_GradientRel1XSpinner
   //~ UI_GradientRel1XSpinner = etk_spinner_new(-100.0, 100.0, 0.0, 0.01, 0.1);
   //~ etk_spinner_digits_set(ETK_SPINNER(UI_GradientRel1XSpinner), 2);
   //~ etk_widget_size_request_set(UI_GradientRel1XSpinner, 45, 20);
   //~ etk_box_append(ETK_BOX(hbox), UI_GradientRel1XSpinner,
                  //~ ETK_BOX_START, ETK_BOX_NONE, 0);

   //~ label = etk_label_new("+");
   //~ etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //~ //UI_GradientRel1XOffsetSpinner
   //~ UI_GradientRel1XOffsetSpinner = etk_spinner_new(-2000, 2000, 0, 1, 10);
   //~ etk_widget_size_request_set(UI_GradientRel1XOffsetSpinner, 45, 20);
   //~ etk_box_append(ETK_BOX(hbox), UI_GradientRel1XOffsetSpinner,
                  //~ ETK_BOX_START, ETK_BOX_NONE, 0);

   //~ label = etk_label_new("<b>  Y </b>");
   //~ etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //~ //UI_GradientRel1YSpinner
   //~ UI_GradientRel1YSpinner = etk_spinner_new(-100.0, 100.0, 0.0, 0.01, 0.1);
   //~ etk_spinner_digits_set(ETK_SPINNER(UI_GradientRel1YSpinner), 2);
   //~ etk_widget_size_request_set(UI_GradientRel1YSpinner, 45, 20);
   //~ etk_box_append(ETK_BOX(hbox), UI_GradientRel1YSpinner,
                  //~ ETK_BOX_START, ETK_BOX_NONE, 0);

   //~ label = etk_label_new("+");
   //~ etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //~ //UI_GradientRel1YOffsetSpinner
   //~ UI_GradientRel1YOffsetSpinner = etk_spinner_new(-2000, 2000, 0, 1, 10);
   //~ etk_widget_size_request_set(UI_GradientRel1YOffsetSpinner, 45, 20);
   //~ etk_box_append(ETK_BOX(hbox), UI_GradientRel1YOffsetSpinner,
                  //~ ETK_BOX_START, ETK_BOX_NONE, 0);


   //hbox (second handler)
   //~ hbox = etk_hbox_new(ETK_FALSE, 0);
   //~ etk_table_attach_default(ETK_TABLE(table), hbox, 0, 2, 4, 4);

   //~ label = etk_label_new("<b>End     X </b>");
   //~ etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //~ //UI_GradientRel2XSpinner
   //~ UI_GradientRel2XSpinner = etk_spinner_new(-100.0, 100.0, 0.0, 0.01, 0.1);
   //~ etk_spinner_digits_set(ETK_SPINNER(UI_GradientRel2XSpinner), 2);
   //~ etk_widget_size_request_set(UI_GradientRel2XSpinner, 45, 20);
   //~ etk_box_append(ETK_BOX(hbox), UI_GradientRel2XSpinner,
                  //~ ETK_BOX_START, ETK_BOX_NONE, 0);

   //~ label = etk_label_new("+");
   //~ etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //~ //UI_GradientRel2XOffsetSpinner
   //~ UI_GradientRel2XOffsetSpinner = etk_spinner_new(-2000, 2000, 0, 1, 10);
   //~ etk_widget_size_request_set(UI_GradientRel2XOffsetSpinner, 45, 20);
   //~ etk_box_append(ETK_BOX(hbox), UI_GradientRel2XOffsetSpinner,
                  //~ ETK_BOX_START, ETK_BOX_NONE, 0);

   //~ label = etk_label_new("<b>  Y </b>");
   //~ etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //~ //UI_GradientRel2YSpinner
   //~ UI_GradientRel2YSpinner = etk_spinner_new(-100.0, 100.0, 0.0, 0.01, 0.1);
   //~ etk_spinner_digits_set(ETK_SPINNER(UI_GradientRel2YSpinner), 2);
   //~ etk_widget_size_request_set(UI_GradientRel2YSpinner, 45, 20);
   //~ etk_box_append(ETK_BOX(hbox), UI_GradientRel2YSpinner,
                  //~ ETK_BOX_START, ETK_BOX_NONE, 0);

   //~ label = etk_label_new("+");
   //~ etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //~ //UI_GradientRel2YOffsetSpinner
   //~ UI_GradientRel2YOffsetSpinner = etk_spinner_new(-2000, 2000, 0, 1, 10);
   //~ etk_widget_size_request_set(UI_GradientRel2YOffsetSpinner, 45, 20);
   //~ etk_box_append(ETK_BOX(hbox), UI_GradientRel2YOffsetSpinner,
                  //~ ETK_BOX_START, ETK_BOX_NONE, 0);

   etk_signal_connect("item-activated", ETK_OBJECT(UI_GradientTypeComboBox),
                     ETK_CALLBACK(_gradient_type_combo_activated_cb), NULL);
   etk_signal_connect("item-activated", ETK_OBJECT(UI_GradientSpectraComboBox),
                     ETK_CALLBACK(_gradient_spectra_combo_activated_cb), NULL);
   
   etk_signal_connect("value-changed", ETK_OBJECT(UI_GradientAlphaSlider),
            ETK_CALLBACK(_image_AlphaSlider_value_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_GradientAngleSlider),
            ETK_CALLBACK(_gradient_AngleSlider_value_changed_cb), NULL);

   //~ etk_signal_connect("value-changed", ETK_OBJECT(UI_GradientRel1XSpinner),
                      //~ ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                      //~ (void *)REL1X_SPINNER);
   //~ etk_signal_connect("value-changed", ETK_OBJECT(UI_GradientRel1YSpinner),
                      //~ ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                      //~ (void *)REL1Y_SPINNER);
   //~ etk_signal_connect("value-changed", ETK_OBJECT(UI_GradientRel2XSpinner),
                      //~ ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                      //~ (void *)REL2X_SPINNER);
   //~ etk_signal_connect("value-changed", ETK_OBJECT(UI_GradientRel2YSpinner),
                      //~ ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                      //~ (void *)REL2Y_SPINNER);

   //~ etk_signal_connect("value-changed", ETK_OBJECT(UI_GradientRel1XOffsetSpinner),
                      //~ ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                      //~ (void *)REL1XO_SPINNER);
   //~ etk_signal_connect("value-changed", ETK_OBJECT(UI_GradientRel1YOffsetSpinner),
                      //~ ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                      //~ (void *)REL1YO_SPINNER);
   //~ etk_signal_connect("value-changed", ETK_OBJECT(UI_GradientRel2XOffsetSpinner),
                      //~ ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                      //~ (void *)REL2XO_SPINNER);
   //~ etk_signal_connect("value-changed", ETK_OBJECT(UI_GradientRel2YOffsetSpinner),
                      //~ ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                      //~ (void *)REL2YO_SPINNER);

   return table;
}

void
gradient_frame_update(void)
{
   const char *type;
   char *p;
   int i;
   int alpha;
   Etk_Combobox_Item *item;

   //Disable signal propagation
   etk_signal_block("item-activated", ETK_OBJECT(UI_GradientTypeComboBox),
                     ETK_CALLBACK(_gradient_type_combo_activated_cb), NULL);
   etk_signal_block("item-activated", ETK_OBJECT(UI_GradientSpectraComboBox),
                     ETK_CALLBACK(_gradient_spectra_combo_activated_cb), NULL);

   etk_signal_block("value-changed", ETK_OBJECT(UI_GradientAlphaSlider),
                    ETK_CALLBACK(_image_AlphaSlider_value_changed_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GradientAngleSlider),
                  ETK_CALLBACK(_gradient_AngleSlider_value_changed_cb), NULL);

   etk_signal_block("value-changed", ETK_OBJECT(UI_GradientRel1XSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL1X_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GradientRel1YSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL1Y_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GradientRel2XSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL2X_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GradientRel2YSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL2Y_SPINNER);

   etk_signal_block("value-changed", ETK_OBJECT(UI_GradientRel1XOffsetSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL1XO_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GradientRel1YOffsetSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL1YO_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GradientRel2XOffsetSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL2XO_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GradientRel2YOffsetSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL2YO_SPINNER);

   //Set Gradient Type Combo
   type = edje_edit_state_gradient_type_get(edje_o, Cur.part->string, Cur.state->string);
   if (type)
   {
      i=0;
      while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_GradientTypeComboBox), i)))
      {
         p = etk_combobox_item_field_get(item, 1);
         if (strcmp(p, type) == 0)
            etk_combobox_active_item_set(ETK_COMBOBOX(UI_GradientTypeComboBox), item);
         i++;
      }
      edje_edit_string_free(type);
   }
   else
      etk_combobox_active_item_set(ETK_COMBOBOX(UI_GradientTypeComboBox),
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_GradientTypeComboBox), 0));

   //Set Gradient Spectra Combo
   type = edje_edit_state_gradient_spectra_get(edje_o, Cur.part->string, Cur.state->string);
   if (type)
   {
      i=0;
      while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_GradientSpectraComboBox), i)))
      {
         p = etk_combobox_item_field_get(item, 1);
         if (strcmp(p, type) == 0)
         {
            etk_combobox_active_item_set(ETK_COMBOBOX(UI_GradientSpectraComboBox), item);
            break;
         }
         i++;
      }
      edje_edit_string_free(type);
   }
   else
      etk_combobox_active_item_set(ETK_COMBOBOX(UI_GradientSpectraComboBox),
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_GradientSpectraComboBox), 0));

   //Set alpha slider
   edje_edit_state_color_get(edje_o, Cur.part->string, Cur.state->string,
                             NULL, NULL, NULL, &alpha);
   etk_range_value_set(ETK_RANGE(UI_GradientAlphaSlider), alpha);
   //Set Angle slider
   etk_range_value_set(ETK_RANGE(UI_GradientAngleSlider),
                       edje_edit_state_gradient_angle_get(edje_o,
                           Cur.part->string, Cur.state->string));
   
   //Update Rels spinners
   etk_range_value_set(ETK_RANGE(UI_GradientRel1XSpinner),
      edje_edit_state_gradient_rel1_relative_x_get(edje_o, Cur.part->string,
                                                   Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_GradientRel1YSpinner),
      edje_edit_state_gradient_rel1_relative_y_get(edje_o, Cur.part->string,
                                                   Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_GradientRel1XOffsetSpinner),
      edje_edit_state_gradient_rel1_offset_x_get(edje_o, Cur.part->string,
                                                 Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_GradientRel1YOffsetSpinner),
      edje_edit_state_gradient_rel1_offset_y_get(edje_o, Cur.part->string,
                                                 Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_GradientRel2XSpinner),
      edje_edit_state_gradient_rel2_relative_x_get(edje_o, Cur.part->string,
                                                   Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_GradientRel2YSpinner),
      edje_edit_state_gradient_rel2_relative_y_get(edje_o, Cur.part->string,
                                                   Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_GradientRel2XOffsetSpinner),
      edje_edit_state_gradient_rel2_offset_x_get(edje_o, Cur.part->string,
                                                 Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_GradientRel2YOffsetSpinner),
      edje_edit_state_gradient_rel2_offset_y_get(edje_o, Cur.part->string,
                                                 Cur.state->string));

   //Reenable signal propagation
   etk_signal_unblock("item-activated", ETK_OBJECT(UI_GradientTypeComboBox),
                     ETK_CALLBACK(_gradient_type_combo_activated_cb), NULL);
   etk_signal_unblock("item-activated", ETK_OBJECT(UI_GradientSpectraComboBox),
                     ETK_CALLBACK(_gradient_spectra_combo_activated_cb), NULL);

   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GradientAlphaSlider),
                    ETK_CALLBACK(_image_AlphaSlider_value_changed_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GradientAngleSlider),
                  ETK_CALLBACK(_gradient_AngleSlider_value_changed_cb), NULL);

   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GradientRel1XSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL1X_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GradientRel1YSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL1Y_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GradientRel2XSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL2X_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GradientRel2YSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL2Y_SPINNER);

   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GradientRel1XOffsetSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL1XO_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GradientRel1YOffsetSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL1YO_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GradientRel2XOffsetSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL2XO_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GradientRel2YOffsetSpinner),
                    ETK_CALLBACK(_gradient_RelSpinners_value_changed_cb),
                    (void *)REL2YO_SPINNER);
}

void
gradient_spectra_combo_populate(void)
{
   Evas_List *spectrums, *l;
   Etk_Widget *parent;

   //Block signal propagation
   etk_signal_block("item-activated", ETK_OBJECT(UI_GradientSpectraComboBox),
                    ETK_CALLBACK(_gradient_spectra_combo_activated_cb), NULL);

   etk_combobox_clear(ETK_COMBOBOX(UI_GradientSpectraComboBox));

   parent = ETK_WIDGET(ETK_COMBOBOX(UI_GradientSpectraComboBox)->window);
   spectrums = l = edje_edit_spectrum_list_get(edje_o);
   while(l)
   {
      etk_combobox_item_append(ETK_COMBOBOX(UI_GradientSpectraComboBox),
                               spectra_thumb_create(parent, (char*)l->data),
                               (char*)l->data);
      l = l->next;
   }
   edje_edit_string_list_free(spectrums);

   //Reenable signal propagation
   etk_signal_unblock("item-activated", ETK_OBJECT(UI_GradientSpectraComboBox),
                    ETK_CALLBACK(_gradient_spectra_combo_activated_cb), NULL);
}

Etk_Bool
_gradient_spectra_combo_activated_cb(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
{
   char *spectra;

   spectra = etk_combobox_item_field_get(item, 1);
   printf("SET  SPECTRA %s\n", spectra);

   edje_edit_state_gradient_spectra_set(edje_o, Cur.part->string, Cur.state->string, spectra);

   return ETK_TRUE;
}

Etk_Bool
_gradient_type_combo_activated_cb(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
{
   char *type;

   type = etk_combobox_item_field_get(item, 1);
   if (!type) return ETK_TRUE;

   printf("SET %s\n", type);
   edje_edit_state_gradient_type_set(edje_o, Cur.part->string, Cur.state->string, type);

   canvas_redraw();
   
   return ETK_TRUE;
}
Etk_Bool
_gradient_RelSpinners_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on Gradient RelSpinner EMITTED (value: %f)\n",etk_range_value_get(range));

   if (!etk_string_length_get(Cur.state) || !etk_string_length_get(Cur.part))
      return ETK_TRUE;

   switch ((int)(long)data)
   {
      case REL1X_SPINNER:
         edje_edit_state_gradient_rel1_relative_x_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL1Y_SPINNER:
         edje_edit_state_gradient_rel1_relative_y_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL2X_SPINNER:
         edje_edit_state_gradient_rel2_relative_x_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL2Y_SPINNER:
         edje_edit_state_gradient_rel2_relative_y_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL1XO_SPINNER:
         edje_edit_state_gradient_rel1_offset_x_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL1YO_SPINNER:
         edje_edit_state_gradient_rel1_offset_y_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL2XO_SPINNER:
         edje_edit_state_gradient_rel2_offset_x_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
      case REL2YO_SPINNER:
         edje_edit_state_gradient_rel2_offset_y_set(edje_o, 
                                 Cur.part->string, Cur.state->string,
                                 etk_range_value_get(range));
         break;
   }
   canvas_redraw();
   return ETK_TRUE;
}
Etk_Bool
_gradient_AngleSlider_value_changed_cb(Etk_Object *object, double va, void *data)
{
   printf("AngleSlieder value_changed signale EMIT: %.2f\n",va);

   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   edje_edit_state_gradient_angle_set(edje_o, Cur.part->string, Cur.state->string, (int)va);

   canvas_redraw();

   return ETK_TRUE;
}
