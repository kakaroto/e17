#include <string.h>
#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"

/***   Implementation   ***/
Etk_Widget*
position_frame_create(void)
{
   Etk_Widget *vbox;
   Etk_Widget *hbox;
   Etk_Widget *label;

/*    //Position Notebook
   notebook = etk_notebook_new();
   etk_container_add(ETK_CONTAINER(UI_PositionFrame), notebook);
 */
/*          //Simple TAB
         vbox = etk_vbox_new(ETK_FALSE, 0);
         etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Simple", vbox);

            //hbox
            hbox = etk_hbox_new(ETK_FALSE, 0);
            etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

               label = etk_label_new("Container");
               etk_object_properties_set(ETK_OBJECT(label), "xalign",0.5,NULL);
               etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

               //SimpleParentCombobox
               SimpleParentComboBox = etk_combobox_new();
               etk_combobox_column_add(ETK_COMBOBOX(SimpleParentComboBox), ETK_COMBOBOX_LABEL, 75, ETK_TRUE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
               etk_combobox_build(ETK_COMBOBOX(SimpleParentComboBox));
               etk_box_append(ETK_BOX(hbox), SimpleParentComboBox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
 */
   //ADVANCED TAB
   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 0);
   // etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Advanced", vbox);

   label = etk_label_new("<color=#FF0000><b>First_Point</b></>");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 0.5, NULL);
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("<color=#FF0000><b>X</b></>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Rel1XSpinner
   UI_Rel1XSpinner = etk_spinner_new(-100.0, 100.0, 0.0, 0.01, 0.1);
   etk_spinner_digits_set(ETK_SPINNER(UI_Rel1XSpinner), 2);
   etk_widget_size_request_set(UI_Rel1XSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel1XSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#FF0000>+</>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //Rel1XOffsetEntry
   UI_Rel1XOffsetSpinner =  etk_spinner_new(-2000, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_Rel1XOffsetSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel1XOffsetSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#FF0000>to</>");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 1.0, NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Rel1ToXCombobox
   UI_Rel1ToXComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_Rel1ToXComboBox),
      ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_Rel1ToXComboBox),
      ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_Rel1ToXComboBox));
   etk_box_append(ETK_BOX(hbox), UI_Rel1ToXComboBox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("<color=#FF0000><b>Y</b></>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Rel1YSpinner
   UI_Rel1YSpinner = etk_spinner_new(-100.0, 100.0, 0.0, 0.01, 0.1);
   etk_spinner_digits_set(ETK_SPINNER(UI_Rel1YSpinner), 2);
   etk_widget_size_request_set(UI_Rel1YSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel1YSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#FF0000>+</>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //Rel1YOffsetSpinner
   UI_Rel1YOffsetSpinner =  etk_spinner_new(-2000, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_Rel1YOffsetSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel1YOffsetSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#FF0000>to</> ");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 1.0, NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Rel1ToYCombobox
   UI_Rel1ToYComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_Rel1ToYComboBox),
      ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_Rel1ToYComboBox),
      ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_Rel1ToYComboBox));
   etk_box_append(ETK_BOX(hbox), UI_Rel1ToYComboBox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("<color=#0000FF><b>Second_Point</b></>");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 0.5, NULL);
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("<color=#0000FF><b>X</b></>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Rel2XSpinner
   UI_Rel2XSpinner = etk_spinner_new(-100.0, 100.0, 0.0, 0.01, 0.1);
   etk_spinner_digits_set(ETK_SPINNER(UI_Rel2XSpinner), 2);
   etk_widget_size_request_set(UI_Rel2XSpinner,45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel2XSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#0000FF>+</>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //Rel2XOffsetSpinner
   UI_Rel2XOffsetSpinner = etk_spinner_new(-2000, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_Rel2XOffsetSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel2XOffsetSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#0000FF>to</>");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 1.0, NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Rel2ToXCombobox
   UI_Rel2ToXComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_Rel2ToXComboBox),
      ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_Rel2ToXComboBox),
      ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_Rel2ToXComboBox));
   etk_box_append(ETK_BOX(hbox), UI_Rel2ToXComboBox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("<color=#0000FF><b>Y</b></>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Rel2YSpinner
   UI_Rel2YSpinner = etk_spinner_new(-100.0, 100.0, 0.0, 0.01, 0.1);
   etk_spinner_digits_set(ETK_SPINNER(UI_Rel2YSpinner), 2);
   etk_widget_size_request_set(UI_Rel2YSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel2YSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#0000FF>+</>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //Rel2YOffsetSpinner
   UI_Rel2YOffsetSpinner = etk_spinner_new(-2000, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_Rel2YOffsetSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel2YOffsetSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#0000FF>to</> ");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 1.0, NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Rel1ToYCombobox
   UI_Rel2ToYComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_Rel2ToYComboBox),
      ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_Rel2ToYComboBox),
      ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_Rel2ToYComboBox));
   etk_box_append(ETK_BOX(hbox), UI_Rel2ToYComboBox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel1XOffsetSpinner),
                      ETK_CALLBACK(_position_RelOffsetSpinners_value_changed_cb),
                      (void *)REL1X_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel1YOffsetSpinner),
                      ETK_CALLBACK(_position_RelOffsetSpinners_value_changed_cb),
                      (void *)REL1Y_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel2XOffsetSpinner),
                      ETK_CALLBACK(_position_RelOffsetSpinners_value_changed_cb),
                      (void *)REL2X_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel2YOffsetSpinner),
                      ETK_CALLBACK(_position_RelOffsetSpinners_value_changed_cb),
                      (void *)REL2Y_SPINNER);

   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel1XSpinner),
                      ETK_CALLBACK(_position_RelSpinners_value_changed_cb),
                      (void *)REL1X_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel1YSpinner),
                      ETK_CALLBACK(_position_RelSpinners_value_changed_cb),
                      (void *)REL1Y_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel2XSpinner),
                      ETK_CALLBACK(_position_RelSpinners_value_changed_cb),
                      (void *)REL2X_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel2YSpinner),
                      ETK_CALLBACK(_position_RelSpinners_value_changed_cb),
                      (void *)REL2Y_SPINNER);

   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_Rel1ToXComboBox),
                      ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                      (void *)REL1X_SPINNER);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_Rel1ToYComboBox),
                      ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                      (void *)REL1Y_SPINNER);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_Rel2ToXComboBox),
                      ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                      (void *)REL2X_SPINNER);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_Rel2ToYComboBox),
                      ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                      (void *)REL2Y_SPINNER);

   return vbox;
}


void
position_frame_update(void)
{
   //printf("Update Position: %s (offset: %d)\n",Cur.eps->name,Cur.eps->rel1.offset.x);
   //Stop signal propagation
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel1XSpinner),
                    ETK_CALLBACK(_position_RelSpinners_value_changed_cb),
                    (void*)REL1X_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel1YSpinner),
                    ETK_CALLBACK(_position_RelSpinners_value_changed_cb),
                    (void*)REL1Y_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel2XSpinner),
                    ETK_CALLBACK(_position_RelSpinners_value_changed_cb),
                    (void*)REL2X_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel2YSpinner),
                    ETK_CALLBACK(_position_RelSpinners_value_changed_cb),
                    (void*)REL2Y_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel1XOffsetSpinner),
                    ETK_CALLBACK(_position_RelOffsetSpinners_value_changed_cb),
                    (void*)REL1X_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel1YOffsetSpinner),
                    ETK_CALLBACK(_position_RelOffsetSpinners_value_changed_cb),
                    (void*)REL1Y_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel2XOffsetSpinner),
                    ETK_CALLBACK(_position_RelOffsetSpinners_value_changed_cb),
                    (void*)REL2X_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel2YOffsetSpinner),
                    ETK_CALLBACK(_position_RelOffsetSpinners_value_changed_cb),
                    (void*)REL2Y_SPINNER);

   if (!etk_string_length_get(Cur.state)) return;
   if (!etk_string_length_get(Cur.part)) return;
    //Set relative position spinners
   etk_range_value_set(ETK_RANGE(UI_Rel1XSpinner),
      edje_edit_state_rel1_relative_x_get(edje_o, Cur.part->string,Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_Rel1YSpinner),
      edje_edit_state_rel1_relative_y_get(edje_o, Cur.part->string,Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_Rel2XSpinner),
      edje_edit_state_rel2_relative_x_get(edje_o, Cur.part->string,Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_Rel2YSpinner),
      edje_edit_state_rel2_relative_y_get(edje_o, Cur.part->string,Cur.state->string));

   etk_range_value_set(ETK_RANGE(UI_Rel1XOffsetSpinner),
         edje_edit_state_rel1_offset_x_get(edje_o, Cur.part->string,Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_Rel1YOffsetSpinner),
         edje_edit_state_rel1_offset_y_get(edje_o, Cur.part->string,Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_Rel2XOffsetSpinner),
         edje_edit_state_rel2_offset_x_get(edje_o, Cur.part->string,Cur.state->string));
   etk_range_value_set(ETK_RANGE(UI_Rel2YOffsetSpinner),
         edje_edit_state_rel2_offset_y_get(edje_o, Cur.part->string,Cur.state->string));

   //Reenable signal propagation
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel1XSpinner),
                      ETK_CALLBACK(_position_RelSpinners_value_changed_cb),
                      (void*)REL1X_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel1YSpinner),
                      ETK_CALLBACK(_position_RelSpinners_value_changed_cb),
                      (void*)REL1Y_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel2XSpinner),
                      ETK_CALLBACK(_position_RelSpinners_value_changed_cb),
                      (void*)REL2X_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel2YSpinner),
                      ETK_CALLBACK(_position_RelSpinners_value_changed_cb),
                      (void*)REL2Y_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel1XOffsetSpinner),
                      ETK_CALLBACK(_position_RelOffsetSpinners_value_changed_cb),
                      (void*)REL1X_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel1YOffsetSpinner),
                      ETK_CALLBACK(_position_RelOffsetSpinners_value_changed_cb),
                      (void*)REL1Y_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel2XOffsetSpinner),
                      ETK_CALLBACK(_position_RelOffsetSpinners_value_changed_cb),
                      (void*)REL2X_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel2YOffsetSpinner),
                      ETK_CALLBACK(_position_RelOffsetSpinners_value_changed_cb),
                      (void*)REL2Y_SPINNER);
}

void
position_comboboxes_populate(void)
{
   Evas_List *l;
   char *image_name;

   printf("Populate 4 Rel Comboboxs\n");
   //Stop signal propagation
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_Rel1ToXComboBox),
                    ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                    (void *)REL1X_SPINNER);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_Rel1ToYComboBox),
                    ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                    (void *)REL1Y_SPINNER);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_Rel2ToXComboBox),
                    ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                    (void *)REL2X_SPINNER);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_Rel2ToYComboBox),
                    ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                    (void *)REL2Y_SPINNER);
   etk_signal_block("item-activated", ETK_OBJECT(UI_CliptoComboBox),
                    ETK_CALLBACK(_part_CliptoComboBox_item_activated_cb), NULL);

   etk_combobox_clear(ETK_COMBOBOX(UI_Rel1ToXComboBox));
   etk_combobox_clear(ETK_COMBOBOX(UI_Rel1ToYComboBox));
   etk_combobox_clear(ETK_COMBOBOX(UI_Rel2ToXComboBox));
   etk_combobox_clear(ETK_COMBOBOX(UI_Rel2ToYComboBox));
   etk_combobox_clear(ETK_COMBOBOX(UI_CliptoComboBox));

   if (etk_string_length_get(Cur.group))
   {
      // Add first element 'Interface' to all the 4 combobox
      etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToXComboBox),
                               etk_image_new_from_edje(EdjeFile,"NONE.PNG"),
                               "Interface");
      etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToYComboBox),
                               etk_image_new_from_edje(EdjeFile,"NONE.PNG"),
                               "Interface");
      etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToXComboBox),
                               etk_image_new_from_edje(EdjeFile,"NONE.PNG"),
                               "Interface");
      etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToYComboBox),
                               etk_image_new_from_edje(EdjeFile,"NONE.PNG"),
                               "Interface");
      etk_combobox_item_append(ETK_COMBOBOX(UI_CliptoComboBox), 
                               etk_image_new_from_edje(EdjeFile,"NONE.PNG"),
                               "None");

      // Add all the part to all the 4 combobox
      Evas_List *parts;
      int type;

      parts = l = edje_edit_parts_list_get(edje_o);
      while (l)
      {
         //printf("-- %s\n", (char *)l->data);
         type = edje_edit_part_type_get(edje_o,(char *)l->data);
         image_name = part_type_image_get(type);

         etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToXComboBox),
                                  etk_image_new_from_edje(EdjeFile, image_name),
                                  (char *)l->data);
         etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToYComboBox),
                                  etk_image_new_from_edje(EdjeFile, image_name),
                                  (char *)l->data);
         etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToXComboBox),
                                  etk_image_new_from_edje(EdjeFile, image_name),
                                  (char *)l->data);
         etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToYComboBox),
                                  etk_image_new_from_edje(EdjeFile, image_name),
                                  (char *)l->data);
         etk_combobox_item_append(ETK_COMBOBOX(UI_CliptoComboBox),
                                  etk_image_new_from_edje(EdjeFile, image_name),
                                  (char *)l->data);
         free(image_name);
         l = l->next;
      }
      edje_edit_string_list_free(parts);
   }

   //Reenable signal propagation
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_Rel1ToXComboBox),
                      ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                      (void *)REL1X_SPINNER);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_Rel1ToYComboBox),
                      ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                      (void *)REL1Y_SPINNER);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_Rel2ToXComboBox),
                      ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                      (void *)REL2X_SPINNER);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_Rel2ToYComboBox),
                      ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                      (void *)REL2Y_SPINNER);
   etk_signal_unblock("item-activated", ETK_OBJECT(UI_CliptoComboBox),
                      ETK_CALLBACK(_part_CliptoComboBox_item_activated_cb), NULL);
}


void
position_comboboxes_update(void)
{
   int i=0;
   Etk_Combobox_Item *item = NULL;

   printf("SETTING COMBOS\n");

   //Stop signal propagation
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_Rel1ToXComboBox),
                    ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                    (void*)REL1X_SPINNER);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_Rel1ToYComboBox),
                    ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                    (void*)REL1Y_SPINNER);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_Rel2ToXComboBox),
                    ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                    (void*)REL2X_SPINNER);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_Rel2ToYComboBox),
                    ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                    (void*)REL2Y_SPINNER);

   if (!etk_string_length_get(Cur.part)) return;
   if (!etk_string_length_get(Cur.state)) return;
   const char *rel;
   char *p;

   //If rel1_to_x is know set the combobox
   if ((rel = edje_edit_state_rel1_to_x_get(edje_o, Cur.part->string, Cur.state->string)))
   {
      //Loop for all the item in the Combobox
      i=0;
      while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel1ToXComboBox),i)))
      {
         p = etk_combobox_item_field_get(item, 1);
         if (strcmp(p,rel) == 0)
            etk_combobox_active_item_set(ETK_COMBOBOX(UI_Rel1ToXComboBox),item);
         i++;
      }
      edje_edit_string_free(rel);
   }
   else
      etk_combobox_active_item_set(ETK_COMBOBOX(UI_Rel1ToXComboBox), 
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel1ToXComboBox),0));
   
   //If rel1_to_y is know set the combobox
   if ((rel = edje_edit_state_rel1_to_y_get(edje_o, Cur.part->string, Cur.state->string)))
   {
      //Loop for all the item in the Combobox
      i=0;
      while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel1ToYComboBox),i)))
      {
         p = etk_combobox_item_field_get(item, 1);
         if (strcmp(p,rel) == 0)
            etk_combobox_active_item_set(ETK_COMBOBOX(UI_Rel1ToYComboBox),item);
         i++;
      }
      edje_edit_string_free(rel);
   }
   else
      etk_combobox_active_item_set(ETK_COMBOBOX(UI_Rel1ToYComboBox), 
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel1ToYComboBox),0));

   //If rel2_to_x is know set the combobox
   if ((rel = edje_edit_state_rel2_to_x_get(edje_o, Cur.part->string, Cur.state->string)))
   {
      //Loop for all the item in the Combobox
      i=0;
      while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToXComboBox),i)))
      {
         p = etk_combobox_item_field_get(item, 1);
         if (strcmp(p,rel) == 0)
            etk_combobox_active_item_set(ETK_COMBOBOX(UI_Rel2ToXComboBox),item);
         i++;
      }
      edje_edit_string_free(rel);
   }
   else
      etk_combobox_active_item_set(ETK_COMBOBOX(UI_Rel2ToXComboBox), 
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToXComboBox),0));

   //If rel2_to_y is know set the combobox
   if ((rel = edje_edit_state_rel2_to_y_get(edje_o, Cur.part->string, Cur.state->string)))
   {
      //Loop for all the item in the Combobox
      i=0;
      while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToYComboBox),i)))
      {
         p = etk_combobox_item_field_get(item, 1);
         if (strcmp(p,rel) == 0)
            etk_combobox_active_item_set(ETK_COMBOBOX(UI_Rel2ToYComboBox),item);
         i++;
      }
      edje_edit_string_free(rel);
   }
   else
      etk_combobox_active_item_set(ETK_COMBOBOX(UI_Rel2ToYComboBox), 
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToYComboBox),0));

   //Reenable signal propagation
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_Rel1ToXComboBox),
                      ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                      (void*)REL1X_SPINNER);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_Rel1ToYComboBox),
                      ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                      (void*)REL1Y_SPINNER);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_Rel2ToXComboBox),
                      ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                      (void*)REL2X_SPINNER);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_Rel2ToYComboBox),
                      ETK_CALLBACK(_position_RelToComboBoxes_changed_cb),
                      (void*)REL2Y_SPINNER);
}

/***   Callbacks   ***/
Etk_Bool
_position_RelOffsetSpinners_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on Offset Spinner EMITTED\n");

   if (etk_string_length_get(Cur.state) && etk_string_length_get(Cur.part))
   {
      switch ((int)(long)data)
      {
         case REL1X_SPINNER:
            edje_edit_state_rel1_offset_x_set(edje_o, 
                                    Cur.part->string, Cur.state->string,
                                    etk_range_value_get(range));
            break;
         case REL1Y_SPINNER:
            edje_edit_state_rel1_offset_y_set(edje_o, 
                                    Cur.part->string, Cur.state->string,
                                    etk_range_value_get(range));
            break;
         case REL2X_SPINNER:
            edje_edit_state_rel2_offset_x_set(edje_o, 
                                    Cur.part->string, Cur.state->string,
                                    etk_range_value_get(range));
            break;
         case REL2Y_SPINNER:
            edje_edit_state_rel2_offset_y_set(edje_o, 
                                    Cur.part->string, Cur.state->string,
                                    etk_range_value_get(range));
            break;
      }
      canvas_redraw();
   }
   return ETK_TRUE;
}

Etk_Bool
_position_RelSpinners_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on RelSpinner EMITTED (value: %f)\n",etk_range_value_get(range));

   if (etk_string_length_get(Cur.state) && etk_string_length_get(Cur.part))
   {
      switch ((int)(long)data)
      {
         case REL1X_SPINNER:
            edje_edit_state_rel1_relative_x_set(edje_o, 
                                    Cur.part->string, Cur.state->string,
                                    etk_range_value_get(range));
            break;
         case REL1Y_SPINNER:
            edje_edit_state_rel1_relative_y_set(edje_o, 
                                    Cur.part->string, Cur.state->string,
                                    etk_range_value_get(range));
            break;
         case REL2X_SPINNER:
            edje_edit_state_rel2_relative_x_set(edje_o, 
                                    Cur.part->string, Cur.state->string,
                                    etk_range_value_get(range));
            break;
         case REL2Y_SPINNER:
            edje_edit_state_rel2_relative_y_set(edje_o, 
                                    Cur.part->string, Cur.state->string,
                                    etk_range_value_get(range));
            break;
      }
      canvas_redraw();
   }
   return ETK_TRUE;
}

Etk_Bool
_position_RelToComboBoxes_changed_cb(Etk_Combobox *combobox, void *data)
{
   char *parent;
   parent = etk_combobox_item_field_get(etk_combobox_active_item_get(combobox), 1);

   if (strcmp(parent,"Interface") == 0)
        parent = NULL;

   if (parent && (strcmp(parent,Cur.part->string) == 0))
   {
      dialog_alert_show("A state can't rel to itself.");
      return ETK_TRUE;
   }

   switch ((int)(long)data)
   {
      case REL1X_SPINNER:
         edje_edit_state_rel1_to_x_set(edje_o, Cur.part->string,
                                       Cur.state->string, parent);
         break;
      case REL1Y_SPINNER:
         edje_edit_state_rel1_to_y_set(edje_o, Cur.part->string,
                                       Cur.state->string, parent);
         break;
      case REL2X_SPINNER:
         edje_edit_state_rel2_to_x_set(edje_o, Cur.part->string,
                                       Cur.state->string, parent);
         break;
      case REL2Y_SPINNER:
        edje_edit_state_rel2_to_y_set(edje_o, Cur.part->string,
                                      Cur.state->string, parent);
         break;
   }

   edje_edit_part_selected_state_set(edje_o, Cur.part->string, Cur.state->string);  //this make edje redraw (need to update in lib)
   canvas_redraw();
   return ETK_TRUE;
}
