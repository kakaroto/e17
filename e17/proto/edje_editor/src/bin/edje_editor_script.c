/*  Copyright (C) 2006-2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of Edje_editor.
 *  Edje_editor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Edje_editor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Edje_editor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


Etk_Widget*
script_frame_create(void)
{
   Etk_Widget *vbox, *hbox, *vbox2;
   Etk_Widget *button;
   Etk_Widget *sv;
   Etk_Widget *send_frame;

   //vbox2
   vbox2 = etk_vbox_new(ETK_FALSE, 0);

   //send_frame
   send_frame = etk_frame_new("Send Messages");
   etk_box_append(ETK_BOX(vbox2), send_frame, ETK_BOX_START, ETK_BOX_NONE, 0);

   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_bin_child_set(ETK_BIN(send_frame), vbox);

   //UI_MsgSendTypeComboBox
   Etk_Combobox_Item *item;
   UI_MsgSendTypeComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_MsgSendTypeComboBox),
                           ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_MsgSendTypeComboBox),
                           ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_MsgSendTypeComboBox));
   etk_box_append(ETK_BOX(vbox), UI_MsgSendTypeComboBox,
                  ETK_BOX_START, ETK_BOX_NONE, 0);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_MsgSendTypeComboBox),
                                   etk_image_new_from_edje(EdjeFile,"DESC.PNG"),
                                   "Send a String");
   etk_combobox_item_data_set(item, (void*)EDJE_MESSAGE_STRING);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_MsgSendTypeComboBox),
                                   etk_image_new_from_edje(EdjeFile,"DESC.PNG"),
                                   "Send an Integer");
   etk_combobox_item_data_set(item, (void*)EDJE_MESSAGE_INT);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_MsgSendTypeComboBox),
                                   etk_image_new_from_edje(EdjeFile,"DESC.PNG"),
                                   "Send a Float");
   etk_combobox_item_data_set(ETK_COMBOBOX_ITEM(item),
                              (void*)EDJE_MESSAGE_FLOAT);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_IntFloatSpinner
   UI_IntFloatSpinner = etk_spinner_new(-9999.0, 9999.0, 0.0, 0.1, 1.0);
   etk_spinner_digits_set(ETK_SPINNER(UI_IntFloatSpinner), 0);
   etk_widget_disabled_set(UI_IntFloatSpinner, ETK_TRUE);
   etk_box_append(ETK_BOX(hbox), UI_IntFloatSpinner,
                  ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_MsgSendEntry
   UI_MsgSendEntry = etk_combobox_entry_new();
   etk_combobox_entry_items_height_set(ETK_COMBOBOX_ENTRY(UI_MsgSendEntry), 18);
   etk_combobox_entry_column_add(ETK_COMBOBOX_ENTRY(UI_MsgSendEntry),
                  ETK_COMBOBOX_ENTRY_LABEL, 75, ETK_COMBOBOX_ENTRY_EXPAND, 0.0);
   etk_combobox_entry_build(ETK_COMBOBOX_ENTRY(UI_MsgSendEntry));
   etk_box_append(ETK_BOX(hbox), UI_MsgSendEntry, 0, ETK_BOX_EXPAND_FILL, 0);

   //Send message button
   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_OK);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Send", NULL);
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   //ScriptBox
   UI_ScriptBox = etk_text_view_new();
   sv = etk_scrolled_view_new();
   etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(sv),
                                 ETK_POLICY_AUTO, ETK_POLICY_AUTO);
   etk_bin_child_set(ETK_BIN(sv), UI_ScriptBox);
   etk_box_append(ETK_BOX(vbox2), sv, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //ScriptSaveButton
   UI_ScriptSaveButton = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE);
   etk_object_properties_set(ETK_OBJECT(UI_ScriptSaveButton),
                             "label","Save script",NULL);
   etk_box_append(ETK_BOX(vbox2), UI_ScriptSaveButton,
                  ETK_BOX_START, ETK_BOX_NONE, 0);

   etk_signal_connect("clicked", ETK_OBJECT(UI_ScriptSaveButton),
            ETK_CALLBACK(_window_all_button_click_cb), (void*)SAVE_SCRIPT);

   etk_signal_connect("item-activated", ETK_OBJECT(UI_MsgSendTypeComboBox),
                      ETK_CALLBACK(_script_MsgTypeComboBox_activated_cb), NULL);
   etk_signal_connect("pressed", ETK_OBJECT(button),
                      ETK_CALLBACK(_script_MsgSend_pressed_cb), NULL);

   return vbox2;
}

void
script_frame_update(void)
{
   if (etk_string_length_get(Cur.group))
   {
      edje_edit_script_get(edje_o);
      //printf("Update group script: %s\n",edje_edit_script_get(edje_o));
   }
}


Etk_Bool
_script_MsgTypeComboBox_activated_cb(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
{
   unsigned char spinner = ETK_FALSE;
   unsigned char entry = ETK_FALSE;

   switch ((int)(long)etk_combobox_item_data_get(item))
   {
      case EDJE_MESSAGE_STRING:
         spinner = ETK_FALSE;
         entry = ETK_TRUE;
         break;
      case EDJE_MESSAGE_INT:
         spinner = ETK_TRUE;
         entry = ETK_FALSE;
         etk_spinner_digits_set(ETK_SPINNER(UI_IntFloatSpinner), 0);
         etk_range_increments_set(ETK_RANGE(UI_IntFloatSpinner), 1, 10);
         break;
      case EDJE_MESSAGE_FLOAT:
         spinner = ETK_TRUE;
         entry = ETK_FALSE;
         etk_spinner_digits_set(ETK_SPINNER(UI_IntFloatSpinner), 2);
         etk_range_increments_set(ETK_RANGE(UI_IntFloatSpinner), 0.1, 1);
         break;
      default:
         break;
   }
   etk_widget_disabled_set(UI_IntFloatSpinner, !spinner);
   etk_widget_disabled_set(UI_MsgSendEntry, !entry);
   return ETK_TRUE;
}

Etk_Bool
_script_MsgSend_pressed_cb(Etk_Button *button, void *data)
{
   int type, id;
   const char *string;
   float value;
   Edje_Message_String msg_str;
   Edje_Message_Int msg_int;
   Edje_Message_Float msg_float;

   type = (int)(long)etk_combobox_item_data_get(etk_combobox_active_item_get(
                                       ETK_COMBOBOX(UI_MsgSendTypeComboBox)));
   string = etk_entry_text_get(ETK_ENTRY(etk_combobox_entry_entry_get(
                                       ETK_COMBOBOX_ENTRY(UI_MsgSendEntry))));
   value = etk_range_value_get(ETK_RANGE(UI_IntFloatSpinner));
   id = 1;  //TODO Whats this?

   switch (type)
   {
      case EDJE_MESSAGE_STRING:
         msg_str.str = (char*)string;
         edje_object_message_send(edje_o, type, id, &msg_str);
         break;
      case EDJE_MESSAGE_INT:
         msg_int.val = (int)value;
         edje_object_message_send(edje_o, type, id, &msg_int);
         break;
      case EDJE_MESSAGE_FLOAT:
         msg_float.val = value;
         edje_object_message_send(edje_o, type, id, &msg_float);
         break;
      default:
         break;
   }

   return ETK_TRUE;
}
