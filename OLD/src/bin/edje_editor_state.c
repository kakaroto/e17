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
state_frame_create(void)
{
   Etk_Widget *vbox;
   Etk_Widget *hbox;
   Etk_Widget *label;
   Etk_Combobox_Item *ComboItem;

   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 0);
  // etk_container_add(ETK_CONTAINER(UI_DescriptionFrame), vbox);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("<b>Name</b>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //StateEntry
   UI_StateEntry = etk_entry_new();
   UI_StateEntryImage = etk_image_new_from_stock(ETK_STOCK_DIALOG_OK,
                                                   ETK_STOCK_SMALL);
   etk_entry_image_set(ETK_ENTRY(UI_StateEntry), ETK_ENTRY_IMAGE_SECONDARY,
                       ETK_IMAGE(UI_StateEntryImage));
   etk_box_append(ETK_BOX(hbox), UI_StateEntry, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_StateIndexSpinner
   //~ UI_StateIndexSpinner = etk_spinner_new(0.0, 1.0, 0.0, 0.1, 1.0);
   //~ etk_spinner_digits_set(ETK_SPINNER(UI_StateIndexSpinner), 1);
   //~ etk_widget_size_request_set(UI_StateIndexSpinner,45, 20);
   //~ etk_box_append(ETK_BOX(hbox),UI_StateIndexSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("<b>Aspect</b>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("min:");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 1.0, NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_AspectMinSpinner
   UI_AspectMinSpinner = etk_spinner_new(0.0, 100.0, 0.0, 0.1, 1.0);
   etk_spinner_digits_set(ETK_SPINNER(UI_AspectMinSpinner), 1);
   etk_widget_size_request_set(UI_AspectMinSpinner,45, 20);
   etk_box_append(ETK_BOX(hbox), UI_AspectMinSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("max:");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 1.0, NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_AspectMaxSpinner
   UI_AspectMaxSpinner = etk_spinner_new(0.0, 100.0, 0.0, 0.1, 1.0);
   etk_spinner_digits_set(ETK_SPINNER(UI_AspectMaxSpinner), 1);
   etk_widget_size_request_set(UI_AspectMaxSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_AspectMaxSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_AspectCombo
   UI_AspectComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_AspectComboBox), ETK_COMBOBOX_LABEL, 30, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_AspectComboBox));
   etk_box_append(ETK_BOX(hbox), UI_AspectComboBox, ETK_BOX_START, ETK_BOX_NONE, 0);

   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_AspectComboBox), "None");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_ASPECT_PREFER_NONE);

   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_AspectComboBox), "Vertical");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_ASPECT_PREFER_VERTICAL);

   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_AspectComboBox), "Horizontal");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_ASPECT_PREFER_HORIZONTAL);

   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_AspectComboBox), "Both");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_ASPECT_PREFER_BOTH);
   
   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<b>Size</b> min");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_StateMinWSpinner
   UI_StateMinWSpinner = etk_spinner_new(0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_StateMinWSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_StateMinWSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("x");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 0.5, NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_StateMinHSpinner
   UI_StateMinHSpinner = etk_spinner_new(0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_StateMinHSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_StateMinHSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("max");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 1.0, NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_StateMaxWSpinner
   UI_StateMaxWSpinner = etk_spinner_new(0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_StateMaxWSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_StateMaxWSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("x");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 0.5, NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_StateMaxHspinner
   UI_StateMaxHSpinner = etk_spinner_new(0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_StateMaxHSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_StateMaxHSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   //UI_StateAlignHSpinner
   label = etk_label_new("<b>Align</b> H");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   UI_StateAlignHSpinner = etk_spinner_new(0, 1, 0, 0.01, 0.1);
   etk_spinner_digits_set(ETK_SPINNER(UI_StateAlignHSpinner), 2);
   etk_widget_size_request_set(UI_StateAlignHSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_StateAlignHSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_StateAlignVSpinner
   label = etk_label_new("V");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   UI_StateAlignVSpinner = etk_spinner_new(0, 1, 0, 0.01, 0.1);
   etk_spinner_digits_set(ETK_SPINNER(UI_StateAlignVSpinner), 2);
   etk_widget_size_request_set(UI_StateAlignVSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_StateAlignVSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_StateVisibleCheck
   UI_StateVisibleCheck = etk_check_button_new_with_label("Visible");
   etk_widget_padding_set(UI_StateVisibleCheck, 20, 0, 0, 0);
   etk_box_append(ETK_BOX(hbox), UI_StateVisibleCheck, ETK_BOX_START, ETK_BOX_NONE, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_StateCCEntry
   label = etk_label_new("Color Class");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   UI_StateCCEntry = etk_entry_new();
   etk_box_append(ETK_BOX(hbox), UI_StateCCEntry, ETK_BOX_START, ETK_BOX_NONE, 0);

   etk_signal_connect("key-down", ETK_OBJECT(UI_StateEntry),
                      ETK_CALLBACK(_state_Entry_key_down_cb), NULL);
   etk_signal_connect("mouse-click", ETK_OBJECT(UI_StateEntryImage),
                      ETK_CALLBACK(_state_EntryImage_clicked_cb), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_StateEntry),
                      ETK_CALLBACK(_group_NamesEntry_text_changed_cb), NULL);

   etk_signal_connect("value-changed", ETK_OBJECT(UI_AspectMinSpinner),
                      ETK_CALLBACK(_state_AspectSpinner_value_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_AspectMaxSpinner),
                      ETK_CALLBACK(_state_AspectSpinner_value_changed_cb), NULL);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_AspectComboBox),
                      ETK_CALLBACK(_state_AspectComboBox_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateMinWSpinner),
                      ETK_CALLBACK(_state_MinMaxSpinner_value_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateMinHSpinner),
                      ETK_CALLBACK(_state_MinMaxSpinner_value_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateMaxWSpinner),
                      ETK_CALLBACK(_state_MinMaxSpinner_value_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateMaxHSpinner),
                      ETK_CALLBACK(_state_MinMaxSpinner_value_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateAlignVSpinner),
                      ETK_CALLBACK(_text_FontAlignSpinner_value_changed_cb),
                      (void*)STATE_ALIGNV_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateAlignHSpinner),
                      ETK_CALLBACK(_text_FontAlignSpinner_value_changed_cb),
                      (void*)STATE_ALIGNH_SPINNER);
   etk_signal_connect("toggled", ETK_OBJECT(UI_StateVisibleCheck),
                      ETK_CALLBACK(_state_VisibleCheck_toggled_cb), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_StateCCEntry),
                      ETK_CALLBACK(_state_CCEntry_text_changed_cb), NULL);

   return vbox;
}

void
state_frame_update(void)
{
   const char* cc;
   
   //Stop signal propagation
   etk_signal_block("text-changed", ETK_OBJECT(UI_StateEntry),
                    _group_NamesEntry_text_changed_cb, NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_AspectMinSpinner),
                    ETK_CALLBACK(_state_AspectSpinner_value_changed_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_AspectMaxSpinner),
                    ETK_CALLBACK(_state_AspectSpinner_value_changed_cb), NULL);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_AspectComboBox),
                    ETK_CALLBACK(_state_AspectComboBox_changed_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_StateMinWSpinner),
                    ETK_CALLBACK(_state_MinMaxSpinner_value_changed_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_StateMinHSpinner),
                    ETK_CALLBACK(_state_MinMaxSpinner_value_changed_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_StateMaxWSpinner),
                    ETK_CALLBACK(_state_MinMaxSpinner_value_changed_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_StateMaxHSpinner),
                    ETK_CALLBACK(_state_MinMaxSpinner_value_changed_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_StateAlignVSpinner),
                    ETK_CALLBACK(_text_FontAlignSpinner_value_changed_cb),
                    (void*)STATE_ALIGNV_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_StateAlignHSpinner),
                    ETK_CALLBACK(_text_FontAlignSpinner_value_changed_cb),
                    (void*)STATE_ALIGNH_SPINNER);
   etk_signal_block("toggled", ETK_OBJECT(UI_StateVisibleCheck),
                    ETK_CALLBACK(_state_VisibleCheck_toggled_cb), NULL);
   etk_signal_block("text-changed", ETK_OBJECT(UI_StateCCEntry),
                    ETK_CALLBACK(_state_CCEntry_text_changed_cb), NULL);

   if (etk_string_length_get(Cur.state))
   {
      //Set description name & index
      etk_entry_text_set(ETK_ENTRY(UI_StateEntry),Cur.state->string);
      etk_widget_hide(ETK_WIDGET(UI_StateEntryImage));
      if (!strcmp(Cur.state->string, "default 0.00"))
         etk_widget_disabled_set(ETK_WIDGET(UI_StateEntry), ETK_TRUE);
      else
         etk_widget_disabled_set(ETK_WIDGET(UI_StateEntry), ETK_FALSE);

      //Set aspect min & max
      etk_range_value_set(ETK_RANGE(UI_AspectMinSpinner),
         edje_edit_state_aspect_min_get(edje_o, Cur.part->string, Cur.state->string));
      etk_range_value_set(ETK_RANGE(UI_AspectMaxSpinner),
         edje_edit_state_aspect_max_get(edje_o, Cur.part->string, Cur.state->string));

      //Set aspect pref Combo
      etk_combobox_active_item_set(ETK_COMBOBOX(UI_AspectComboBox),
         etk_combobox_nth_item_get(ETK_COMBOBOX(UI_AspectComboBox), 
            edje_edit_state_aspect_pref_get(edje_o, Cur.part->string, Cur.state->string)));

      //Set min e max size
      etk_range_value_set(ETK_RANGE(UI_StateMinWSpinner),
         edje_edit_state_min_w_get(edje_o, Cur.part->string, Cur.state->string));
      etk_range_value_set(ETK_RANGE(UI_StateMinHSpinner),
         edje_edit_state_min_h_get(edje_o, Cur.part->string, Cur.state->string));
      etk_range_value_set(ETK_RANGE(UI_StateMaxWSpinner),
         edje_edit_state_max_w_get(edje_o, Cur.part->string, Cur.state->string));
      etk_range_value_set(ETK_RANGE(UI_StateMaxHSpinner),
         edje_edit_state_max_h_get(edje_o, Cur.part->string, Cur.state->string));

      //Set description align & valign
      etk_range_value_set(ETK_RANGE(UI_StateAlignHSpinner),
         edje_edit_state_align_x_get(edje_o, Cur.part->string, Cur.state->string));
      etk_range_value_set(ETK_RANGE(UI_StateAlignVSpinner),
         edje_edit_state_align_y_get(edje_o, Cur.part->string, Cur.state->string));

      //Set visible checkbox
      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(UI_StateVisibleCheck),
         edje_edit_state_visible_get(edje_o, Cur.part->string, Cur.state->string));
      
      //Set Color Class Entry
      cc = edje_edit_state_color_class_get(edje_o, Cur.part->string, Cur.state->string);
      etk_entry_text_set(ETK_ENTRY(UI_StateCCEntry), cc);
      edje_edit_string_free(cc);
   }

   //ReEnable Signal Propagation
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_StateEntry),
                      _group_NamesEntry_text_changed_cb, NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_AspectMinSpinner),
                      ETK_CALLBACK(_state_AspectSpinner_value_changed_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_AspectMaxSpinner),
                      ETK_CALLBACK(_state_AspectSpinner_value_changed_cb), NULL);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_AspectComboBox),
                      ETK_CALLBACK(_state_AspectComboBox_changed_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_StateMinWSpinner),
                      ETK_CALLBACK(_state_MinMaxSpinner_value_changed_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_StateMinHSpinner),
                      ETK_CALLBACK(_state_MinMaxSpinner_value_changed_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_StateMaxWSpinner),
                      ETK_CALLBACK(_state_MinMaxSpinner_value_changed_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_StateMaxHSpinner),
                      ETK_CALLBACK(_state_MinMaxSpinner_value_changed_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_StateAlignVSpinner),
                      ETK_CALLBACK(_text_FontAlignSpinner_value_changed_cb),
                      (void*)STATE_ALIGNV_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_StateAlignHSpinner),
                      ETK_CALLBACK(_text_FontAlignSpinner_value_changed_cb),
                      (void*)STATE_ALIGNH_SPINNER);
   etk_signal_unblock("toggled", ETK_OBJECT(UI_StateVisibleCheck),
                      ETK_CALLBACK(_state_VisibleCheck_toggled_cb), NULL);
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_StateCCEntry),
                      ETK_CALLBACK(_state_CCEntry_text_changed_cb), NULL);

}


/***   Callbacks   ***/
Etk_Bool
_state_Entry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   printf("PRESSED %s\n", event->keyname);
   if (!strcmp("default 0.00", Cur.state->string))
   {
      dialog_alert_show("You can't rename default 0.0");
      return ETK_TRUE;
   }

   if (!strcmp(event->keyname, "Return"))
      _state_EntryImage_clicked_cb(ETK_OBJECT(ETK_ENTRY(object)->secondary_image),
                                   NULL);
   return ETK_TRUE;
}

Etk_Bool
_state_EntryImage_clicked_cb(Etk_Object *object, void *data)
{
   const char *name;

   printf("Mouse Click Signal on StateEntryImage Emitted\n");

   name = etk_entry_text_get(ETK_ENTRY(UI_StateEntry));

   if (!name || !etk_string_length_get(Cur.state)) return ETK_TRUE;

   if (!strcmp(name, Cur.state->string))
   {
      etk_widget_hide(ETK_WIDGET(UI_StateEntryImage));
      return ETK_TRUE;
   }

   /* Change state name */
   if (strcmp("default 0.00", Cur.state->string))
   {
      if (edje_edit_state_name_set(edje_o, Cur.part->string, Cur.state->string, name))
      {
         /* update tree */
         Etk_Tree_Row *row;
         row = etk_tree_selected_row_get(ETK_TREE(UI_PartsTree));
         etk_tree_row_fields_set(row,TRUE,
                                    COL_NAME, EdjeFile, "DESC.PNG", name,
                                    NULL);
         /* update Cur */
         Cur.state = etk_string_set(Cur.state, name);
         /* Hide the entry image */
         etk_widget_hide(ETK_WIDGET(UI_StateEntryImage));
      }
      else
         dialog_alert_show("<b>Wrong name format</b><br>Name must be in the form:<br>\"default 0.00\"");
   }
   else
      dialog_alert_show("You can't rename default 0.0");

   return ETK_TRUE;
}

Etk_Bool
_state_AspectSpinner_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on AspectMinSpinner EMITTED\n");
   edje_edit_state_aspect_min_set(edje_o, Cur.part->string, Cur.state->string,
                           etk_range_value_get(ETK_RANGE(UI_AspectMinSpinner)));
   edje_edit_state_aspect_max_set(edje_o, Cur.part->string, Cur.state->string,
                           etk_range_value_get(ETK_RANGE(UI_AspectMaxSpinner)));
   return ETK_TRUE;
}

Etk_Bool
_state_AspectComboBox_changed_cb(Etk_Combobox *combobox, void *data)
{
   printf("Active Item Changed Signal on AspectComboBox EMITTED\n");
   int pref;
   pref = (int)(long)etk_combobox_item_data_get(etk_combobox_active_item_get (combobox));
   edje_edit_state_aspect_pref_set(edje_o, Cur.part->string, Cur.state->string, pref);
   return ETK_TRUE;
}

Etk_Bool
_state_MinMaxSpinner_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("Active Item Changed Signal on MinMaxSpinners EMITTED\n");

   edje_edit_state_min_w_set(edje_o, Cur.part->string, Cur.state->string,
                           etk_range_value_get(ETK_RANGE(UI_StateMinWSpinner)));
   edje_edit_state_min_h_set(edje_o, Cur.part->string, Cur.state->string,
                           etk_range_value_get(ETK_RANGE(UI_StateMinHSpinner)));
   edje_edit_state_max_w_set(edje_o, Cur.part->string, Cur.state->string,
                           etk_range_value_get(ETK_RANGE(UI_StateMaxWSpinner)));
   edje_edit_state_max_h_set(edje_o, Cur.part->string, Cur.state->string,
                           etk_range_value_get(ETK_RANGE(UI_StateMaxHSpinner)));

   canvas_redraw();
   return ETK_TRUE;
}

Etk_Bool
_state_VisibleCheck_toggled_cb(Etk_Toggle_Button *button, void *data)
{
   edje_edit_state_visible_set(edje_o, Cur.part->string, Cur.state->string,
                               etk_toggle_button_active_get(button));
   return ETK_TRUE;
}
Etk_Bool
_state_CCEntry_text_changed_cb(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on CC Entry Emitted\n");
   edje_edit_state_color_class_set(edje_o, Cur.part->string, Cur.state->string,
                                   etk_entry_text_get(ETK_ENTRY(UI_StateCCEntry)));
   return ETK_TRUE;
}
