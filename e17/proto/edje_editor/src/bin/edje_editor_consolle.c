#include <string.h>
#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


void
emit_entry_item_append_ifnot(Etk_Widget *combo_entry, const char *text)
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

Evas_Object *
create_consolle(void)
{
   Etk_Widget *embed;
   
   Consolle = edje_object_add(ecore_evas_get(UI_ecore_MainWin));
   edje_object_file_set(Consolle, EdjeFile, "Consolle");
   evas_object_show(Consolle);
   
   //Embed Signal Emit
   embed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(embed), create_signal_embed());
   etk_embed_position_method_set(ETK_EMBED(embed), _embed_position_set,
                                 UI_ecore_MainWin);
   etk_widget_show_all(embed);
   edje_object_part_swallow(Consolle,"signal_swallow",
                            etk_embed_object_get(ETK_EMBED(embed)));
   return Consolle;
}

void
ConsolleClear(void)
{
   edje_object_part_text_set(Consolle, "line1", "");
   edje_object_part_text_set(Consolle, "line2", "");
   edje_object_part_text_set(Consolle, "line3", "");
   edje_object_part_text_set(Consolle, "line4", "");
   edje_object_part_text_set(Consolle, "line5", "");
   
   while(stack)
   {
      evas_stringshare_del(evas_list_data(stack));
      stack = evas_list_remove_list(stack, stack);
   }
   consolle_count = 0;
}
void
ConsolleLog(char *text)
{
   //printf("LOG: %s\n", text);
   
   stack = evas_list_prepend(stack, evas_stringshare_add(text));
   
   while (evas_list_count(stack) > 5)
   {
      evas_stringshare_del(evas_list_data(evas_list_last(stack)));
      stack = evas_list_remove_list(stack, evas_list_last(stack));
   }

   edje_object_part_text_set(Consolle, "line1", evas_list_nth(stack, 0));
   edje_object_part_text_set(Consolle, "line2", evas_list_nth(stack, 1));
   edje_object_part_text_set(Consolle, "line3", evas_list_nth(stack, 2));
   edje_object_part_text_set(Consolle, "line4", evas_list_nth(stack, 3));
   edje_object_part_text_set(Consolle, "line5", evas_list_nth(stack, 4));
}

Etk_Widget*
create_signal_embed(void)
{
   Etk_Widget *hbox;
   Etk_Widget *label;
   Etk_Widget *button;
   
   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 3);
   
   //UI_SignalEmitEntry
   label = etk_label_new("<b>Signal</b>");
   etk_box_append(ETK_BOX(hbox), label, 0, ETK_BOX_NONE, 0);
   UI_SignalEmitEntry = etk_combobox_entry_new();
   etk_combobox_entry_items_height_set(ETK_COMBOBOX_ENTRY(UI_SignalEmitEntry), 18);
   etk_combobox_entry_column_add(ETK_COMBOBOX_ENTRY(UI_SignalEmitEntry),
                  ETK_COMBOBOX_ENTRY_LABEL, 75, ETK_COMBOBOX_ENTRY_EXPAND, 0.0);
   etk_combobox_entry_build(ETK_COMBOBOX_ENTRY(UI_SignalEmitEntry));
   etk_box_append(ETK_BOX(hbox), UI_SignalEmitEntry, 0, ETK_BOX_NONE, 0);
   
   //UI_SourceEmitEntry
   label = etk_label_new("<b>Source</b>");
   etk_box_append(ETK_BOX(hbox), label, 0, ETK_BOX_NONE, 0);
   UI_SourceEmitEntry = etk_combobox_entry_new();
   etk_combobox_entry_items_height_set(ETK_COMBOBOX_ENTRY(UI_SourceEmitEntry), 18);
   etk_combobox_entry_column_add(ETK_COMBOBOX_ENTRY(UI_SourceEmitEntry),
                  ETK_COMBOBOX_ENTRY_LABEL, 75, ETK_COMBOBOX_ENTRY_EXPAND, 0.0);
   etk_combobox_entry_build(ETK_COMBOBOX_ENTRY(UI_SourceEmitEntry));
   etk_box_append(ETK_BOX(hbox), UI_SourceEmitEntry, 0, ETK_BOX_NONE, 0);
   
   //button
   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_OK);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Emit", NULL);
   etk_box_append(ETK_BOX(hbox), button, 0, ETK_BOX_NONE, 0);
   
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_SignalEmitEntry),
                      ETK_CALLBACK(on_SignalEmitEntry_activated), NULL);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_SourceEmitEntry),
                      ETK_CALLBACK(on_SignalEmitEntry_activated), NULL);
   
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(on_AllButton_click), (void*)EMIT_SIGNAL);
   
   return hbox;
}

void
PopulateSourceComboEntry(void)
{
   Evas_List *l;
   char *image_name;
   printf("Populate Program Source ComboEntry\n");
    
   //Stop signal propagation
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_SourceEntry),
                  ETK_CALLBACK(on_SourceEntry_item_changed), NULL);
   
   etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(UI_SourceEntry));
   
   l = edje_edit_parts_list_get(edje_o);
   while (l)
   {
      image_name = GetPartTypeImage(edje_edit_part_type_get(edje_o, (char*)l->data));
      etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SourceEntry),
                  etk_image_new_from_edje(EdjeFile, image_name),
                  (char *)l->data);
      free(image_name);
      
      l = l->next;
   }
   edje_edit_string_list_free(l);
   
   //Renable  signal propagation
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_SourceEntry),
                     ETK_CALLBACK(on_SourceEntry_item_changed), NULL);
}

void
PopulateSignalComboEntry(void)
{
   printf("Populate Program Signal ComboEntry\n");
   
   //Stop signal propagation
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_SignalEntry),
                  ETK_CALLBACK(on_SignalEntry_item_changed), NULL);
   
   etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(UI_SignalEntry));
      
   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
            etk_image_new_from_edje(EdjeFile, "DESC.PNG"), "program,start");
   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
            etk_image_new_from_edje(EdjeFile, "DESC.PNG"), "program,stop");
   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
            etk_image_new_from_edje(EdjeFile, "DESC.PNG"), "load");
   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
            etk_image_new_from_edje(EdjeFile, "DESC.PNG"), "show");
   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
            etk_image_new_from_edje(EdjeFile, "DESC.PNG"), "hide");
   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
            etk_image_new_from_edje(EdjeFile, "DESC.PNG"), "resize");
   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
            etk_image_new_from_edje(EdjeFile, "DESC.PNG"), "mouse,in");
   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
            etk_image_new_from_edje(EdjeFile, "DESC.PNG"), "mouse,out");
   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
            etk_image_new_from_edje(EdjeFile, "DESC.PNG"), "mouse,move");
   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
            etk_image_new_from_edje(EdjeFile, "DESC.PNG"), "mouse,down,1");
   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
            etk_image_new_from_edje(EdjeFile, "DESC.PNG"), "mouse,up,1");
   etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
            etk_image_new_from_edje(EdjeFile, "DESC.PNG"), "mouse,clicked,1");
   
   //Renable  signal propagation
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_SignalEntry),
                     ETK_CALLBACK(on_SignalEntry_item_changed), NULL);
}


Etk_Bool
on_SignalEmitEntry_activated(Etk_Combobox_Entry *combo, void *data)
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
