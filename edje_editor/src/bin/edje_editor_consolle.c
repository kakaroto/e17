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

/***   Locals   ***/
static void _consolle_entry_item_append_ifnotexist(Etk_Widget *combo_entry, const char *text);
static Etk_Widget* _create_signal_embed(void);

static Etk_Widget *UI_SignalEmitEntry;
static Etk_Widget *UI_SourceEmitEntry;

/***   Callbacks   ***/
Etk_Bool
_consolle_button_click_cb(Etk_Button *button, void *data)
{
   const char *sig, *sou;

   sig = etk_entry_text_get(ETK_ENTRY(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SignalEmitEntry))));
   sou = etk_entry_text_get(ETK_ENTRY(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SourceEmitEntry))));

   edje_object_signal_emit(edje_o, sig, sou);

   _consolle_entry_item_append_ifnotexist(UI_SignalEmitEntry, sig);
   _consolle_entry_item_append_ifnotexist(UI_SourceEmitEntry, sou);

   return ETK_TRUE;
}

Etk_Bool
_consolle_combobox_entry_activated_cb(Etk_Combobox_Entry *combo, void *data)
{
   Etk_Combobox_Entry_Item *item;
   Etk_Widget *entry;
   char *str;

   entry = etk_combobox_entry_entry_get(combo);
   item = etk_combobox_entry_active_item_get(combo);

   str = etk_combobox_entry_item_field_get(item, 0);
   etk_entry_text_set(ETK_ENTRY(entry), str);

   return ETK_TRUE;
}


/***   Internals   ***/
static void
_consolle_entry_item_append_ifnotexist(Etk_Widget *combo_entry, const char *text)
{
   Etk_Combobox_Entry_Item *item;
   char *str;

   if (!text || strlen(text) < 1) return;

   item = etk_combobox_entry_first_item_get(ETK_COMBOBOX_ENTRY(combo_entry));
   while (item)
   {
      str = etk_combobox_entry_item_field_get(item, 0);
      if (!strcmp(str, text))
         return;
      item = etk_combobox_entry_item_next_get(item);
   }

   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(combo_entry), text);
}

static Etk_Widget*
_create_signal_embed(void)
{
   Etk_Widget *hbox;
   Etk_Widget *label;
   Etk_Widget *button;

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 3);

   //UI_SignalEmitEntry
   label = etk_label_new("<b>Signal</b>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   UI_SignalEmitEntry = etk_combobox_entry_new();
   etk_combobox_entry_items_height_set(ETK_COMBOBOX_ENTRY(UI_SignalEmitEntry), 18);
   etk_combobox_entry_column_add(ETK_COMBOBOX_ENTRY(UI_SignalEmitEntry),
                  ETK_COMBOBOX_ENTRY_LABEL, 75, ETK_COMBOBOX_ENTRY_EXPAND, 0.0);
   etk_combobox_entry_build(ETK_COMBOBOX_ENTRY(UI_SignalEmitEntry));
   etk_box_append(ETK_BOX(hbox), UI_SignalEmitEntry, 0, ETK_BOX_NONE, 0);

   //UI_SourceEmitEntry
   label = etk_label_new("<b>Source</b>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   UI_SourceEmitEntry = etk_combobox_entry_new();
   etk_combobox_entry_items_height_set(ETK_COMBOBOX_ENTRY(UI_SourceEmitEntry), 18);
   etk_combobox_entry_column_add(ETK_COMBOBOX_ENTRY(UI_SourceEmitEntry),
                  ETK_COMBOBOX_ENTRY_LABEL, 75, ETK_COMBOBOX_ENTRY_EXPAND, 0.0);
   etk_combobox_entry_build(ETK_COMBOBOX_ENTRY(UI_SourceEmitEntry));
   etk_box_append(ETK_BOX(hbox), UI_SourceEmitEntry, ETK_BOX_START, ETK_BOX_NONE, 0);

   //button
   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_OK);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Emit", NULL);
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_SignalEmitEntry),
                      ETK_CALLBACK(_consolle_combobox_entry_activated_cb), NULL);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_SourceEmitEntry),
                      ETK_CALLBACK(_consolle_combobox_entry_activated_cb), NULL);

   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_consolle_button_click_cb), NULL);
   return hbox;
}


/***   Implementation   ***/
Evas_Object *
consolle_create(void)
{
   Etk_Widget *embed;

   EV_Consolle = edje_object_add(UI_evas);
   edje_object_file_set(EV_Consolle, EdjeFile, "Consolle");
   evas_object_show(EV_Consolle);

   //Embed Signal Emit
   embed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(embed), _create_signal_embed());
   etk_embed_position_method_set(ETK_EMBED(embed), window_embed_position_set,
                                 UI_ecore_MainWin);
   etk_widget_show_all(embed);
   edje_object_part_swallow(EV_Consolle,"signal_swallow",
                            etk_embed_object_get(ETK_EMBED(embed)));
   return EV_Consolle;
}

void
consolle_clear(void)
{
   edje_object_part_text_set(EV_Consolle, "line1", "");
   edje_object_part_text_set(EV_Consolle, "line2", "");
   edje_object_part_text_set(EV_Consolle, "line3", "");
   edje_object_part_text_set(EV_Consolle, "line4", "");
   edje_object_part_text_set(EV_Consolle, "line5", "");

   while(stack)
   {
      evas_stringshare_del(eina_list_data_get(stack));
      stack = eina_list_remove_list(stack, stack);
   }
   consolle_count = 0;
}
void
consolle_log(char *text)
{
   //printf("LOG: %s\n", text);

   stack = eina_list_prepend(stack, evas_stringshare_add(text));

   while (eina_list_count(stack) > 5)
   {
      evas_stringshare_del(eina_list_data_get(eina_list_last(stack)));
      stack = eina_list_remove_list(stack, eina_list_last(stack));
   }

   edje_object_part_text_set(EV_Consolle, "line1", eina_list_nth(stack, 0));
   edje_object_part_text_set(EV_Consolle, "line2", eina_list_nth(stack, 1));
   edje_object_part_text_set(EV_Consolle, "line3", eina_list_nth(stack, 2));
   edje_object_part_text_set(EV_Consolle, "line4", eina_list_nth(stack, 3));
   edje_object_part_text_set(EV_Consolle, "line5", eina_list_nth(stack, 4));
}
