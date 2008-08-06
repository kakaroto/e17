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

#include <string.h>
#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


/***   Implementation   ***/
Etk_Widget*
text_frame_create(Evas *evas)
{
   Etk_Widget *vbox;
   Etk_Widget *hbox;
   Etk_Widget *label;
   Etk_Widget *table;
   Etk_Combobox_Item *ComboItem;

   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 5);

   //table
   table = etk_table_new(5, 4, ETK_TABLE_NOT_HOMOGENEOUS);
   etk_box_append(ETK_BOX(vbox), table, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("Text");
   etk_object_properties_set(ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default(ETK_TABLE(table),label, 0, 0, 0,0);

   //Text Entry
   UI_TextEntry = etk_entry_new ();
   //etk_widget_size_request_set(UI_TextEntry,30, 30);
   etk_table_attach_default(ETK_TABLE(table),UI_TextEntry, 1, 4, 0,0);

   label = etk_label_new("Font");
   etk_object_properties_set(ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default(ETK_TABLE(table),label, 0, 0, 1,1);

   //FontComboBox
   UI_FontComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_FontComboBox),
                           ETK_COMBOBOX_IMAGE, 20, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_FontComboBox),
                           ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_FontComboBox));
   etk_table_attach_default(ETK_TABLE(table),UI_FontComboBox, 1, 1, 1,1);

   //FontAddButton
   UI_FontAddButton = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_OPEN);
   etk_object_properties_set(ETK_OBJECT(UI_FontAddButton), "label","",NULL);
   etk_table_attach_default(ETK_TABLE(table),UI_FontAddButton, 2, 2, 1, 1);


   label = etk_label_new("Size");
   etk_object_properties_set(ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default(ETK_TABLE(table),label, 3, 3, 1,1);

   //FontSizeSpinner
   UI_FontSizeSpinner = etk_spinner_new(0, 200, 0, 1, 10);
   etk_widget_size_request_set(UI_FontSizeSpinner, 45, 20);
   etk_table_attach_default(ETK_TABLE(table),UI_FontSizeSpinner, 4, 4, 1,1);

   //FontAlignHSpinner
   label = etk_label_new("Align");
   etk_table_attach_default(ETK_TABLE(table),label,0,0, 2,2);

   UI_FontAlignHSpinner = etk_spinner_new(0, 1, 0, 0.01, 0.1);
   etk_spinner_digits_set(ETK_SPINNER(UI_FontAlignHSpinner), 2);
   etk_widget_size_request_set(UI_FontAlignHSpinner, 45, 20);
   etk_table_attach_default(ETK_TABLE(table),UI_FontAlignHSpinner, 1, 1, 2,2);

   //FontAlignVSpinner
   label = etk_label_new("V Align");
   etk_table_attach_default(ETK_TABLE(table),label, 2, 2, 2,2);

   UI_FontAlignVSpinner =  etk_spinner_new (0, 1, 0, 0.01, 0.1);
   etk_spinner_digits_set(ETK_SPINNER(UI_FontAlignVSpinner), 2);
   etk_widget_size_request_set(UI_FontAlignVSpinner, 45, 20);
   etk_table_attach_default(ETK_TABLE(table),UI_FontAlignVSpinner, 3, 4, 2,2);

   //PartEffectComboBox
   label = etk_label_new("Effect");
   etk_table_attach_default(ETK_TABLE(table),label, 0, 0, 3,3);

   UI_EffectComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_EffectComboBox), 
      ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_EffectComboBox),
      ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_EffectComboBox));

   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje(EdjeFile,"NONE.PNG"), "Plain");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_TEXT_EFFECT_PLAIN);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje(EdjeFile,"NONE.PNG"), "Outline");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_TEXT_EFFECT_OUTLINE);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje(EdjeFile,"NONE.PNG"), "Soft Outline");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_TEXT_EFFECT_SOFT_OUTLINE);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje(EdjeFile,"NONE.PNG"), "Shadow");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_TEXT_EFFECT_SHADOW);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje(EdjeFile,"NONE.PNG"), "Soft Shadow");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_TEXT_EFFECT_SOFT_SHADOW);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje(EdjeFile,"NONE.PNG"), "Outline Shadow");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_TEXT_EFFECT_OUTLINE_SHADOW);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje(EdjeFile,"NONE.PNG"), "Outline Soft Shadow");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_TEXT_EFFECT_OUTLINE_SOFT_SHADOW);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje(EdjeFile,"NONE.PNG"), "Far Shadow");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_TEXT_EFFECT_FAR_SHADOW);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje(EdjeFile,"NONE.PNG"), "Far Soft Shadow");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_TEXT_EFFECT_FAR_SOFT_SHADOW);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje(EdjeFile,"NONE.PNG"), "Glow");
   etk_combobox_item_data_set(ComboItem, (void*)EDJE_TEXT_EFFECT_GLOW);

   etk_table_attach_default(ETK_TABLE(table),UI_EffectComboBox,1,4,3,3);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 10);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Color buttons
   etk_box_append(ETK_BOX(hbox),
                  window_color_button_create("Text",
                                             COLOR_OBJECT_TEXT, 30, 30, evas),
                  ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_box_append(ETK_BOX(hbox),
                  window_color_button_create("Shadow",
                                             COLOR_OBJECT_SHADOW, 30, 30, evas),
                  ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_box_append(ETK_BOX(hbox),
                  window_color_button_create("Outline",
                                             COLOR_OBJECT_OUTLINE, 30, 30, evas),
                  ETK_BOX_START, ETK_BOX_EXPAND, 0);

   etk_signal_connect("clicked", ETK_OBJECT(UI_FontAddButton),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_FONT_FILE_ADD);
   etk_signal_connect("item-activated", ETK_OBJECT(UI_FontComboBox),
                      ETK_CALLBACK(_text_FontComboBox_item_activated_cb), NULL);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_EffectComboBox),
                      ETK_CALLBACK(_text_EffectComboBox_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FontSizeSpinner),
                      ETK_CALLBACK(_text_FontSizeSpinner_value_changed_cb), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_TextEntry),
                      ETK_CALLBACK(_text_Entry_text_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FontAlignVSpinner),
                      ETK_CALLBACK(_text_FontAlignSpinner_value_changed_cb),
                      (void*)TEXT_ALIGNV_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FontAlignHSpinner),
                      ETK_CALLBACK(_text_FontAlignSpinner_value_changed_cb),
                      (void*)TEXT_ALIGNH_SPINNER);

   return vbox;
}

void 
text_font_combo_populate(void)
{
   Evas_List *l;
   Etk_Combobox_Item *ComboItem;

   //Stop signal propagation
   etk_signal_disconnect("item-activated", ETK_OBJECT(UI_FontComboBox),
                         ETK_CALLBACK(_text_FontComboBox_item_activated_cb), NULL);

   printf("Populate Fonts Combo\n");

   etk_combobox_clear(ETK_COMBOBOX(UI_FontComboBox));

   Evas_List *fonts;
   fonts = l = edje_edit_fonts_list_get(edje_o);
   while (l)
   {
      ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_FontComboBox),
                     etk_image_new_from_stock(ETK_STOCK_PREFERENCES_DESKTOP_FONT,ETK_STOCK_SMALL), 
                     l->data);
      l = l->next;
   }
   edje_edit_string_list_free(fonts);

   //Renable  signal propagation
   etk_signal_connect("item-activated", ETK_OBJECT(UI_FontComboBox),
                      ETK_CALLBACK(_text_FontComboBox_item_activated_cb), NULL);

}

void
text_frame_update(void)
{
   int eff_num = 0;
   //int alpha;
   int r, g, b;
   Etk_Combobox_Item *item = NULL;
   char *combo_data;
   const char *t;
   const char *font;

   if (!etk_string_length_get(Cur.state)) return;
   if (!etk_string_length_get(Cur.part)) return;

   printf("Update Text Frame: %s\n",Cur.state->string);

   //Stop signal propagation
   etk_signal_block("text-changed", ETK_OBJECT(UI_TextEntry),
                    _text_Entry_text_changed_cb, NULL);
   etk_signal_block("item-activated", ETK_OBJECT(UI_FontComboBox),
                    ETK_CALLBACK(_text_FontComboBox_item_activated_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_FontAlignHSpinner),
                    ETK_CALLBACK(_text_FontAlignSpinner_value_changed_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_FontAlignVSpinner),
                    ETK_CALLBACK(_text_FontAlignSpinner_value_changed_cb), NULL);

   //Set Text Text in Cur.eps
   t = edje_edit_state_text_get(edje_o,Cur.part->string,Cur.state->string);
   printf("TEXT: %s\n",t);
   etk_entry_text_set(ETK_ENTRY(UI_TextEntry), t);
   edje_edit_string_free(t);

   //Set the font size spinner
   etk_range_value_set(ETK_RANGE(UI_FontSizeSpinner), 
      (float)edje_edit_state_text_size_get(edje_o, Cur.part->string, Cur.state->string));

   //Set the font align spinners
   etk_range_value_set(ETK_RANGE(UI_FontAlignHSpinner),
                        edje_edit_state_text_align_x_get(edje_o,
                                                         Cur.part->string,
                                                         Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_FontAlignVSpinner),
                        edje_edit_state_text_align_y_get(edje_o,
                                                         Cur.part->string,
                                                         Cur.state->string));

   //Set the font combobox
   font = edje_edit_state_font_get(edje_o, Cur.part->string, Cur.state->string);
   if (font)
   {
      //Loop for all the item in the Combobox
      item = etk_combobox_first_item_get(ETK_COMBOBOX(UI_FontComboBox));
      while (item)
      {
         combo_data = etk_combobox_item_field_get(item, 1);
         printf("COMBODATA: %s\n",combo_data);
         if (combo_data && (strcmp(combo_data, font) == 0))
         {
            //If we found the item set active and break
            etk_combobox_active_item_set(ETK_COMBOBOX(UI_FontComboBox),item);
            break;
         }
         item = etk_combobox_item_next_get(item);
      }
   }
   else
      etk_combobox_active_item_set(ETK_COMBOBOX(UI_FontComboBox),
                  etk_combobox_first_item_get(ETK_COMBOBOX(UI_FontComboBox)));//TODO change all combobox like this one

   edje_edit_string_free(font);

   //Set Effect ComboBox
   eff_num = edje_edit_part_effect_get(edje_o, Cur.part->string);
   eff_num--;
   if (eff_num < 0) eff_num = 0;

   etk_combobox_active_item_set(ETK_COMBOBOX(UI_EffectComboBox),
      etk_combobox_nth_item_get(ETK_COMBOBOX(UI_EffectComboBox), eff_num));

   //Set Text color Rects
   edje_edit_state_color_get(edje_o, Cur.part->string, Cur.state->string,&r,&g,&b,NULL);
   evas_object_color_set(TextColorObject, r, g, b, 255);
   edje_edit_state_color2_get(edje_o, Cur.part->string, Cur.state->string,&r,&g,&b,NULL);
   evas_object_color_set(ShadowColorObject, r, g, b, 255);
   edje_edit_state_color3_get(edje_o, Cur.part->string, Cur.state->string,&r,&g,&b,NULL);
   evas_object_color_set(OutlineColorObject, r, g, b, 255);

   //Renable  signal propagation
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_TextEntry),
                      ETK_CALLBACK(_text_Entry_text_changed_cb), NULL);
   etk_signal_unblock("item-activated", ETK_OBJECT(UI_FontComboBox),
                      ETK_CALLBACK(_text_FontComboBox_item_activated_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_FontAlignHSpinner),
                      ETK_CALLBACK(_text_FontAlignSpinner_value_changed_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_FontAlignVSpinner),
                      ETK_CALLBACK(_text_FontAlignSpinner_value_changed_cb), NULL);
}


/***   Callbacks   ***/
Etk_Bool
_text_FontComboBox_item_activated_cb(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
{
   printf("Changed Signal on FontComboBox EMITTED \n");

   char *font;
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;

   font = etk_combobox_item_field_get(item, 1);

   edje_edit_state_font_set(edje_o, Cur.part->string, Cur.state->string, font);

   return ETK_TRUE;
}

Etk_Bool
_text_EffectComboBox_changed_cb(Etk_Combobox *combobox, void *data)
{
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;

   edje_edit_part_effect_set(edje_o, Cur.part->string,
      (int)(long)etk_combobox_item_data_get(etk_combobox_active_item_get(combobox)));

   canvas_redraw();

   return ETK_TRUE;
}

Etk_Bool
_text_FontSizeSpinner_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on FontSizeSpinner EMITTED (value: %d)\n",(int)etk_range_value_get(range));

   edje_edit_state_text_size_set(edje_o, Cur.part->string, Cur.state->string,
                                 (int)etk_range_value_get(range));

   canvas_redraw();
   return ETK_TRUE;
}


Etk_Bool
_text_Entry_text_changed_cb(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on TextEntry EMITTED (value %s)\n",etk_entry_text_get(ETK_ENTRY(object)));
   edje_edit_state_text_set(edje_o, Cur.part->string, Cur.state->string,
                            etk_entry_text_get(ETK_ENTRY(object)));

   canvas_redraw();
   return ETK_TRUE;
}

Etk_Bool
_text_FontAlignSpinner_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on AlignSpinner (h or v, text or part) EMITTED (value: %.2f)\n",etk_range_value_get(range));

   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   
   if ((int)(long)data == TEXT_ALIGNH_SPINNER)
      edje_edit_state_text_align_x_set(edje_o, Cur.part->string, Cur.state->string,
                                       (double)etk_range_value_get(range));
   if ((int)(long)data == TEXT_ALIGNV_SPINNER)
      edje_edit_state_text_align_y_set(edje_o, Cur.part->string, Cur.state->string,
                                       (double)etk_range_value_get(range));
   if ((int)(long)data == STATE_ALIGNH_SPINNER)
      edje_edit_state_align_x_set(edje_o, Cur.part->string, Cur.state->string,
                                  (double)etk_range_value_get(range));
   if ((int)(long)data == STATE_ALIGNV_SPINNER)
      edje_edit_state_align_y_set(edje_o, Cur.part->string, Cur.state->string,
                                  (double)etk_range_value_get(range));

   return ETK_TRUE;
}
