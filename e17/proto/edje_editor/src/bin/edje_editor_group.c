#include <string.h>
#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


/***   Implementation   ***/
Etk_Widget*
group_frame_create(void)
{
   Etk_Widget *label;
   Etk_Widget *hbox, *vbox;

   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);

   //GroupNameEntry
   label = etk_label_new("Name");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   UI_GroupNameEntry = etk_entry_new();
   UI_GroupNameEntryImage = etk_image_new_from_stock(ETK_STOCK_DIALOG_OK,
                                                     ETK_STOCK_SMALL);
   etk_entry_image_set(ETK_ENTRY(UI_GroupNameEntry), ETK_ENTRY_IMAGE_SECONDARY,
                       ETK_IMAGE(UI_GroupNameEntryImage));
   etk_box_append(ETK_BOX(hbox), UI_GroupNameEntry, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //CurrentSizeLabel
   UI_CurrentGroupSizeLabel = etk_label_new("Current size: 0 x 0");
   etk_box_append(ETK_BOX(vbox), UI_CurrentGroupSizeLabel, ETK_BOX_START, ETK_BOX_NONE, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("min");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_GroupMinWSpinner
   UI_GroupMinWSpinner = etk_spinner_new(0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_GroupMinWSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_GroupMinWSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("x");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 0.5, NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_GroupMinHSpinner
   UI_GroupMinHSpinner = etk_spinner_new(0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_GroupMinHSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_GroupMinHSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("max");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 1.0, NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_GroupMaxWSpinner
   UI_GroupMaxWSpinner =  etk_spinner_new(0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_GroupMaxWSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_GroupMaxWSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("x");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 0.5, NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_GroupMaxHspinner
   UI_GroupMaxHSpinner = etk_spinner_new(0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_GroupMaxHSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_GroupMaxHSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   
   etk_signal_connect("text-changed", ETK_OBJECT(UI_GroupNameEntry),
         ETK_CALLBACK(_group_NamesEntry_text_changed_cb), NULL);   
   etk_signal_connect("key-down", ETK_OBJECT(UI_GroupNameEntry),
         ETK_CALLBACK(_group_NameEntry_key_down_cb), NULL);
   etk_signal_connect("mouse-click", ETK_OBJECT(UI_GroupNameEntryImage),
                      ETK_CALLBACK(_group_NameEntryImage_clicked_cb), NULL);

   etk_signal_connect("value-changed", ETK_OBJECT(UI_GroupMinWSpinner),
                      ETK_CALLBACK(_group_spinners_value_changed_cb),
                      (void *)MINW_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_GroupMinHSpinner),
                      ETK_CALLBACK(_group_spinners_value_changed_cb),
                      (void *)MINH_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_GroupMaxWSpinner),
                      ETK_CALLBACK(_group_spinners_value_changed_cb),
                      (void *)MAXW_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_GroupMaxHSpinner),
                      ETK_CALLBACK(_group_spinners_value_changed_cb),
                     (void *)MAXH_SPINNER);

   return vbox;
}

void
group_frame_update(void)
{
   //Stop signal propagation
   etk_signal_block("text-changed", ETK_OBJECT(UI_GroupNameEntry),
                    _group_NamesEntry_text_changed_cb, NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GroupMinWSpinner),
                    ETK_CALLBACK(_group_spinners_value_changed_cb),
                    (void *)MINW_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GroupMinHSpinner),
                    ETK_CALLBACK(_group_spinners_value_changed_cb),
                    (void *)MINH_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GroupMaxWSpinner),
                    ETK_CALLBACK(_group_spinners_value_changed_cb),
                    (void *)MAXW_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GroupMaxHSpinner),
                    ETK_CALLBACK(_group_spinners_value_changed_cb),
                    (void *)MAXH_SPINNER);

   if (etk_string_length_get(Cur.group))
   {
      //Update name
      etk_entry_text_set(ETK_ENTRY(UI_GroupNameEntry),Cur.group->string);
      etk_widget_hide(ETK_WIDGET(UI_GroupNameEntryImage));

      //Update min e max spinners
      etk_range_value_set(ETK_RANGE(UI_GroupMinWSpinner),
                          (float)edje_edit_group_min_w_get(edje_o));
      etk_range_value_set(ETK_RANGE(UI_GroupMinHSpinner),
                          (float)edje_edit_group_min_h_get(edje_o));
      etk_range_value_set(ETK_RANGE(UI_GroupMaxWSpinner),
                          (float)edje_edit_group_max_w_get(edje_o));
      etk_range_value_set(ETK_RANGE(UI_GroupMaxHSpinner),
                          (float)edje_edit_group_max_h_get(edje_o));
   }

   //ReEnable Signal Propagation
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_GroupNameEntry),
                      _group_NamesEntry_text_changed_cb, NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GroupMinWSpinner),
                      ETK_CALLBACK(_group_spinners_value_changed_cb),
                      (void *)MINW_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GroupMinHSpinner),
                      ETK_CALLBACK(_group_spinners_value_changed_cb),
                      (void *)MINH_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GroupMaxWSpinner),
                      ETK_CALLBACK(_group_spinners_value_changed_cb),
                      (void *)MAXW_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GroupMaxHSpinner),
                      ETK_CALLBACK(_group_spinners_value_changed_cb),
                      (void *)MAXH_SPINNER);
}


/***   Callbacks   ***/
Etk_Bool
_group_NamesEntry_text_changed_cb(Etk_Object *object, void *data)
{
   //printf("Text Changed Signal on one of the Names Entry Emitted\n");
   etk_widget_show(ETK_WIDGET(ETK_ENTRY(object)->secondary_image));
   return ETK_TRUE;
}

Etk_Bool
_group_spinners_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("Group Spinners value changed signal EMIT\n");
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   switch ((int)(long)data)
   {
      case MINW_SPINNER:
         edje_edit_group_min_w_set(edje_o,
            (int)etk_range_value_get(ETK_RANGE(UI_GroupMinWSpinner)));
         break;
      case MINH_SPINNER:
         edje_edit_group_min_h_set(edje_o,
            (int)etk_range_value_get(ETK_RANGE(UI_GroupMinHSpinner)));
         break;
      case MAXW_SPINNER:
         edje_edit_group_max_w_set(edje_o,
            (int)etk_range_value_get(ETK_RANGE(UI_GroupMaxWSpinner)));
         break;
      case MAXH_SPINNER:
         edje_edit_group_max_h_set(edje_o,
            (int)etk_range_value_get(ETK_RANGE(UI_GroupMaxHSpinner)));
         break;
   }
   return ETK_TRUE;
}

Etk_Bool
_group_NameEntry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   //printf("PRESSED %s\n", event->keyname);

   if (!strcmp(event->keyname, "Return"))
      _group_NameEntryImage_clicked_cb(
                                 ETK_OBJECT(ETK_ENTRY(object)->secondary_image),
                                 NULL);
   return ETK_TRUE;
}

Etk_Bool
_group_NameEntryImage_clicked_cb(Etk_Object *object, void *data)
{
   const char *name;

   printf("Mouse Click Signal on GroupNameEntryImage Emitted\n");

   name = etk_entry_text_get(ETK_ENTRY(UI_GroupNameEntry));
   if (!name || !etk_string_length_get(Cur.group)) return ETK_TRUE;

   if (!strcmp(name, Cur.group->string))
   {
      etk_widget_hide(ETK_WIDGET(UI_GroupNameEntryImage));
      return ETK_TRUE;
   }

   if (!edje_edit_group_name_set(edje_o, name))
   {
      dialog_alert_show("Can't rename group.<br>Another group with this name exist?");
      return ETK_TRUE;
   }

   //Update Group Combobox
   Etk_Combobox_Entry_Item *item;
   item = etk_combobox_entry_active_item_get(ETK_COMBOBOX_ENTRY(UI_GroupsComboBox));
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_GroupsComboBox),
                    ETK_CALLBACK(_tree_combobox_active_item_changed_cb), NULL);
   etk_combobox_entry_autosearch_set(ETK_COMBOBOX_ENTRY(UI_GroupsComboBox), -1, NULL);

   etk_combobox_entry_item_fields_set(item, name);
   etk_entry_text_set(ETK_ENTRY(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_GroupsComboBox))), name);

   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_GroupsComboBox),
                      ETK_CALLBACK(_tree_combobox_active_item_changed_cb), NULL);
   etk_combobox_entry_autosearch_set(ETK_COMBOBOX_ENTRY(UI_GroupsComboBox),
                                     GROUP_COMBO_AUTOSEARCH_COL, NULL);
   //Update FakeWin title
   edje_object_part_text_set(EV_fakewin, "title", name);

   /* Hide the entry image */
   etk_widget_hide(ETK_WIDGET(UI_GroupNameEntryImage));

   return ETK_TRUE;
}
