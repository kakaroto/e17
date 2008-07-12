#include <string.h>
#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"



/***   Implementation   ***/
Etk_Widget*
part_frame_create(void)
{
   Etk_Widget *table;
   Etk_Widget *label;
   Etk_Widget *frame;
   Etk_Widget *hbox;

   //table
   table = etk_table_new(2, 4, ETK_TABLE_NOT_HOMOGENEOUS);

   //PartNameEntry
   label = etk_label_new("<b>Name</b>");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0,ETK_TABLE_NONE,0,0);
   UI_PartNameEntry = etk_entry_new();
   UI_PartNameEntryImage = etk_image_new_from_stock(ETK_STOCK_DIALOG_OK,
                                                    ETK_STOCK_SMALL);
   etk_entry_image_set(ETK_ENTRY(UI_PartNameEntry), ETK_ENTRY_IMAGE_SECONDARY,
                       ETK_IMAGE(UI_PartNameEntryImage));
   etk_table_attach_default(ETK_TABLE(table),UI_PartNameEntry, 1, 1, 0, 0);

   //UI_CliptoComboBox
   label = etk_label_new("<b>Clip to</b>");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1,ETK_TABLE_NONE,0,0);

   UI_CliptoComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_CliptoComboBox),
                           ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_CliptoComboBox),
                           ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_CliptoComboBox));
   etk_table_attach_default(ETK_TABLE(table), UI_CliptoComboBox, 1, 1, 1, 1);

   //events frame
   frame = etk_frame_new("Mouse events");
   etk_table_attach(ETK_TABLE(table), frame, 0, 1, 2, 2, 
                     ETK_TABLE_EXPAND_FILL, 0, 0);
   //events hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_widget_padding_set(hbox, 10, 0, 0, 0);
   etk_container_add(ETK_CONTAINER(frame), hbox); 

   //PartEventsCheck
   UI_PartEventsCheck = etk_check_button_new_with_label("Accept</b>");
   etk_box_append(ETK_BOX(hbox), UI_PartEventsCheck,
                  ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //PartEventRepeatCheck
   UI_PartEventsRepeatCheck = etk_check_button_new_with_label("Repeat</b>");
   etk_box_append(ETK_BOX(hbox), UI_PartEventsRepeatCheck,
                  ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_PartSourceComboBox
   UI_PartSourceLabel = etk_label_new("<b>Source</b>");
   etk_table_attach(ETK_TABLE(table), UI_PartSourceLabel,
                    0, 0, 3, 3, ETK_TABLE_NONE, 0, 0);

   UI_PartSourceComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_PartSourceComboBox),
                           ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_PartSourceComboBox));
   etk_table_attach_default(ETK_TABLE(table), UI_PartSourceComboBox, 1, 1, 3, 3);


   etk_signal_connect("text-changed", ETK_OBJECT(UI_PartNameEntry),
         ETK_CALLBACK(_group_NamesEntry_text_changed_cb), NULL);   
   etk_signal_connect("key-down", ETK_OBJECT(UI_PartNameEntry),
         ETK_CALLBACK(_part_NameEntry_key_down_cb), NULL);
   etk_signal_connect("mouse-click", ETK_OBJECT(UI_PartNameEntryImage),
                      ETK_CALLBACK(_part_NameEntryImage_clicked_cb), NULL);
   etk_signal_connect("toggled", ETK_OBJECT(UI_PartEventsCheck),
                      ETK_CALLBACK(_part_EventsCheck_toggled_cb), NULL);
   etk_signal_connect("toggled", ETK_OBJECT(UI_PartEventsRepeatCheck),
                      ETK_CALLBACK(_part_EventsRepeatCheck_toggled_cb), NULL);
   etk_signal_connect("item-activated", ETK_OBJECT(UI_CliptoComboBox),
                     ETK_CALLBACK(_part_CliptoComboBox_item_activated_cb), NULL);
   etk_signal_connect("item-activated", ETK_OBJECT(UI_PartSourceComboBox),
                      ETK_CALLBACK(_part_SourceComboBox_item_activated_cb), NULL);
   return table;
}

void
part_frame_update(void)
{
   //Stop signal propagation
   etk_signal_block("text-changed",ETK_OBJECT(UI_PartNameEntry),
                    _group_NamesEntry_text_changed_cb, NULL);
   etk_signal_block("toggled",ETK_OBJECT(UI_PartEventsCheck),
                    _part_EventsCheck_toggled_cb, NULL);
   etk_signal_block("toggled",ETK_OBJECT(UI_PartEventsRepeatCheck),
                    _part_EventsRepeatCheck_toggled_cb, NULL);
   etk_signal_block("item-activated", ETK_OBJECT(UI_CliptoComboBox),
                    ETK_CALLBACK(_part_CliptoComboBox_item_activated_cb), NULL);
   etk_signal_block("item-activated", ETK_OBJECT(UI_PartSourceComboBox),
                    ETK_CALLBACK(_part_SourceComboBox_item_activated_cb), NULL);

   if (etk_string_length_get(Cur.part))
   {
      etk_entry_text_set(ETK_ENTRY(UI_PartNameEntry), Cur.part->string);
      etk_widget_hide(ETK_WIDGET(UI_PartNameEntryImage));

      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(UI_PartEventsCheck),
                     edje_edit_part_mouse_events_get(edje_o, Cur.part->string));
      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(UI_PartEventsRepeatCheck),
                     edje_edit_part_repeat_events_get(edje_o, Cur.part->string));


      /* Update clip_to combobox */
      Etk_Combobox_Item *item = NULL;
      const char *clipto;
      int i;
      char *p;

      clipto = edje_edit_part_clip_to_get(edje_o, Cur.part->string);

      if (clipto)
      {
         //Loop for all the item in the Combobox
         i=1;
         while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_CliptoComboBox),i)))
         {
            p = etk_combobox_item_field_get(item, 1);
            if (!strcmp(p, clipto))
               etk_combobox_active_item_set(ETK_COMBOBOX(UI_CliptoComboBox),item);
            i++;
         }
      }
      else
         etk_combobox_active_item_set(ETK_COMBOBOX(UI_CliptoComboBox),
               etk_combobox_first_item_get(ETK_COMBOBOX(UI_CliptoComboBox)));
      edje_edit_string_free(clipto);


      /* Update PartSource combobox */
      const char *source;
      source = edje_edit_part_source_get(edje_o, Cur.part->string);

      if (source)
      {
         //Loop for all the item in the Combobox
         i=1;
         while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_PartSourceComboBox),i)))
         {
            p = etk_combobox_item_field_get(item, 0);
            if (!strcmp(p, source))
               etk_combobox_active_item_set(ETK_COMBOBOX(UI_PartSourceComboBox),item);
            i++;
         }
      }
      else
         etk_combobox_active_item_set(ETK_COMBOBOX(UI_PartSourceComboBox),
               etk_combobox_first_item_get(ETK_COMBOBOX(UI_PartSourceComboBox)));

      edje_edit_string_free(source);
   }
   
   //Show/hide Sourcecombo for part EDJE_PART_TYPE_GROUP
   if (edje_edit_part_type_get(edje_o, Cur.part->string) == EDJE_PART_TYPE_GROUP)
   {
      etk_widget_show(UI_PartSourceComboBox);
      etk_widget_show(UI_PartSourceLabel);
   }
   else
   {
      etk_widget_hide(UI_PartSourceComboBox);
      etk_widget_hide(UI_PartSourceLabel);
   }

   //ReEnable Signal Propagation
   etk_signal_unblock("text-changed",ETK_OBJECT(UI_PartNameEntry),
                      _group_NamesEntry_text_changed_cb, NULL);
   etk_signal_unblock("toggled",ETK_OBJECT(UI_PartEventsCheck),
                      _part_EventsCheck_toggled_cb, NULL);
   etk_signal_unblock("toggled",ETK_OBJECT(UI_PartEventsRepeatCheck),
                      _part_EventsRepeatCheck_toggled_cb, NULL);
   etk_signal_unblock("item-activated", ETK_OBJECT(UI_CliptoComboBox),
                      ETK_CALLBACK(_part_CliptoComboBox_item_activated_cb), NULL);
   etk_signal_unblock("item-activated", ETK_OBJECT(UI_PartSourceComboBox),
                      ETK_CALLBACK(_part_SourceComboBox_item_activated_cb), NULL);
}

char*
part_type_image_get(int part_type)
{
   /* Get the name of the group in edje_editor.edj that
    * correspond to the given EDJE_PART_TYPE.
    * Remember to free the returned string.
    */

   static char buf[20];
   char *ret;

   switch (part_type)
   {
      case EDJE_PART_TYPE_IMAGE:     strcpy(buf, "IMAGE.PNG"); break;
      case EDJE_PART_TYPE_GRADIENT:  strcpy(buf, "GRAD.PNG"); break;
      case EDJE_PART_TYPE_TEXT:      strcpy(buf, "TEXT.PNG"); break;
      case EDJE_PART_TYPE_RECTANGLE: strcpy(buf, "RECT.PNG"); break;
      case EDJE_PART_TYPE_SWALLOW:   strcpy(buf, "SWAL.PNG"); break;
      case EDJE_PART_TYPE_GROUP:     strcpy(buf, "GROUP.PNG"); break;
      default:                       strcpy(buf, "NONE.PNG"); break;
   }
   ret = strdup(buf);
   printf("IMAGE: %s\n",ret);
   return ret;
}
/***   Callbacks   ***/
Etk_Bool
_part_EventsCheck_toggled_cb(Etk_Object *object, void *data)
{
   printf("Toggled Signal on EventsCheck EMITTED\n");
   if (etk_string_length_get(Cur.part))
   {
      edje_edit_part_mouse_events_set(edje_o, Cur.part->string,
                     etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(object)));
   }
   return ETK_TRUE;
}

Etk_Bool
_part_EventsRepeatCheck_toggled_cb(Etk_Object *object, void *data)
{
   printf("Toggled Signal on EventsRepeatCheck EMITTED\n");
   if (etk_string_length_get(Cur.part))
   {
      edje_edit_part_repeat_events_set(edje_o, Cur.part->string,
                     etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(object)));
   }
   return ETK_TRUE;
}

Etk_Bool
_part_CliptoComboBox_item_activated_cb(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
{
   char *to;
   printf("Item Activated Signal on CliptoCombobox EMITTED\n");
   
   to = etk_combobox_item_field_get(item, 1);
   if (strcmp(to, "None"))
      edje_edit_part_clip_to_set(edje_o, Cur.part->string, to);
   else
      edje_edit_part_clip_to_set(edje_o, Cur.part->string, NULL);

   return ETK_TRUE;
}

Etk_Bool
_part_SourceComboBox_item_activated_cb(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
{
   char *gr;
   printf("Item Activated Signal on PartSourceCombobox EMITTED\n");

   gr = etk_combobox_item_field_get(item,0);

   if (!strcmp(gr, Cur.group->string))
   {
      dialog_alert_show("A group can't contain itself");
      return ETK_TRUE;
   }

   if (strcmp(gr, "None"))
      edje_edit_part_source_set(edje_o, Cur.part->string, gr);
   else
      edje_edit_part_source_set(edje_o, Cur.part->string, NULL);

   reload_edje();
   return ETK_TRUE;
}

Etk_Bool
_part_NameEntry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   printf("PRESSED %s\n", event->keyname);

   if (!strcmp(event->keyname, "Return"))
      _part_NameEntryImage_clicked_cb(
                                 ETK_OBJECT(ETK_ENTRY(object)->secondary_image),
                                 NULL);
   return ETK_TRUE;
}

Etk_Bool
_part_NameEntryImage_clicked_cb(Etk_Object *object, void *data)
{
   const char *name;
   char *image_name;
   Etk_Tree_Row *row;
   Etk_Tree_Row *child;

   printf("Mouse Click Signal on PartNameEntryImage Emitted\n");

   name = etk_entry_text_get(ETK_ENTRY(UI_PartNameEntry));

   if (!name || !etk_string_length_get(Cur.part)) return ETK_TRUE;

   if (!strcmp(name, Cur.part->string))
   {
      etk_widget_hide(ETK_WIDGET(UI_PartNameEntryImage));
      return ETK_TRUE;
   }

   /* change the name in edje */
   if (!edje_edit_part_name_set(edje_o, Cur.part->string, name))
   {
      dialog_alert_show("Can't set part name.<br>Another name with this name exist? ");
      return ETK_TRUE;
   }

   /* Set new Current name */
   Cur.part = etk_string_set(Cur.part, name);

   //Update PartTree
   row = etk_tree_selected_row_get(ETK_TREE(UI_PartsTree));
   image_name = part_type_image_get(edje_edit_part_type_get(edje_o, Cur.part->string));
   etk_tree_row_fields_set(row,TRUE,
                           COL_NAME, EdjeFile, image_name, name, NULL);
   free(image_name);


   /* Update hidden colon on every child */
   child = etk_tree_row_first_child_get(row);
   etk_tree_row_fields_set(child, TRUE, COL_PARENT, name, NULL);
   while ((child = etk_tree_row_next_get(child)))
      etk_tree_row_fields_set(child, TRUE, COL_PARENT, name, NULL);

   /* Update Parts_Hash */
   Parts_Hash = evas_hash_del(Parts_Hash, Cur.part->string, NULL);
   Parts_Hash = evas_hash_add(Parts_Hash, name, row);

   /* Recreate rel combobox */
   position_comboboxes_populate();
   program_source_combo_populate();

   /* Hide the image */
   etk_widget_hide(ETK_WIDGET(UI_PartNameEntryImage));

   return ETK_TRUE;
}

