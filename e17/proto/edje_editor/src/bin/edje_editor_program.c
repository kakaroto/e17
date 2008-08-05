#include <string.h>
#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


/***   Implementation   ***/
Etk_Widget*
program_frame_create(void)
{
   Etk_Widget *table;
   Etk_Widget *label;

   //table
   table = etk_table_new(4, 10, ETK_TABLE_HHOMOGENEOUS);

   //UI_ProgramEntry + image
   label = etk_label_new("<b>Name</b>");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0,ETK_TABLE_NONE,0,0);
   UI_ProgramEntry = etk_entry_new();
   etk_tooltips_tip_set(UI_ProgramEntry, "Symbolic <b>name</b> of "
                                         "program as a unique identifier.");
   UI_ProgramEntryImage = etk_image_new_from_stock(ETK_STOCK_DIALOG_OK,
                                                   ETK_STOCK_SMALL);
   etk_entry_image_set(ETK_ENTRY(UI_ProgramEntry), ETK_ENTRY_IMAGE_SECONDARY,
                       ETK_IMAGE(UI_ProgramEntryImage));
   etk_table_attach_default(ETK_TABLE(table),UI_ProgramEntry, 1, 2, 0, 0);

   //UI_RunProgButton
   UI_RunProgButton = etk_button_new_from_stock(ETK_STOCK_MEDIA_PLAYBACK_START);
   etk_object_properties_set(ETK_OBJECT(UI_RunProgButton),
                              "label","Run",NULL);
   etk_table_attach_default(ETK_TABLE(table),UI_RunProgButton, 3, 3, 0, 0);

   etk_signal_connect("clicked", ETK_OBJECT(UI_RunProgButton),
            ETK_CALLBACK(_window_all_button_click_cb), (void*)RUN_PROG);

   //UI_SourceEntry
   label = etk_label_new("<b>Source</b>");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1,ETK_TABLE_NONE,0,0);
   //etk_tooltips_tip_set(UI_SourceEntry, "<b>Source(s)</b> of the signal.<br>The Part or Program that emit the signal<br>Wildcards can be used to widen the scope, ex: \"button-*\"");
   UI_SourceEntry = etk_combobox_entry_new();
   etk_combobox_entry_column_add(ETK_COMBOBOX_ENTRY(UI_SourceEntry),
                  ETK_COMBOBOX_ENTRY_IMAGE, 24, ETK_COMBOBOX_ENTRY_NONE, 0.0);
   etk_combobox_entry_column_add(ETK_COMBOBOX_ENTRY(UI_SourceEntry),
                  ETK_COMBOBOX_ENTRY_LABEL, 75, ETK_COMBOBOX_ENTRY_EXPAND, 0.0);
   etk_combobox_entry_build(ETK_COMBOBOX_ENTRY(UI_SourceEntry));
   etk_table_attach_default(ETK_TABLE(table), UI_SourceEntry, 1, 3, 1, 1);

   //UI_SignalEntry
   label = etk_label_new("<b>Signal</b>");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 2, 2,ETK_TABLE_NONE,0,0);

   UI_SignalEntry = etk_combobox_entry_new();
   etk_combobox_entry_column_add(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
                  ETK_COMBOBOX_ENTRY_IMAGE, 24, ETK_COMBOBOX_ENTRY_NONE, 0.0);
   etk_combobox_entry_column_add(ETK_COMBOBOX_ENTRY(UI_SignalEntry),
                  ETK_COMBOBOX_ENTRY_LABEL, 75, ETK_COMBOBOX_ENTRY_EXPAND, 0.0);
   etk_combobox_entry_build(ETK_COMBOBOX_ENTRY(UI_SignalEntry));
   etk_tooltips_tip_set(UI_SignalEntry, "The name of the <b>signal</b> that will trigger the program.<br>Wildcards can be used to widen the scope, ex: \"mouse,down,*\"<br>Can be a mouse signal ( mouse,in ; mouse,up,1 )<br>Or a user defined signal (emitted by the application)");
   etk_table_attach_default(ETK_TABLE(table),UI_SignalEntry, 1, 3, 2, 2);

   //UI_DelayFromSpinner
   label = etk_label_new("<b>Delay</b>");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 3, 3,ETK_TABLE_NONE,0,0);
   UI_DelayFromSpinner = etk_spinner_new(0.0, 999.0, 0.0, 0.1, 1.0);
   etk_tooltips_tip_set(UI_DelayFromSpinner, "The number of seconds to wait before running the program");
   etk_spinner_digits_set(ETK_SPINNER(UI_DelayFromSpinner), 3);
   etk_widget_size_request_set(UI_DelayFromSpinner,45, 20);
   etk_table_attach_default(ETK_TABLE(table),UI_DelayFromSpinner, 1, 1, 3, 3);

   //UI_DelayRangeSpinner
   label = etk_label_new("<b>+random</b>");
   etk_table_attach(ETK_TABLE(table), label, 2, 2, 3, 3,ETK_TABLE_NONE,0,0);
   UI_DelayRangeSpinner = etk_spinner_new(0.0, 999.0, 0.0, 0.1, 1.0);
   etk_tooltips_tip_set(UI_DelayRangeSpinner, "The maximum <b>random</b> seconds which is added to <b>Delay</b>");
   etk_spinner_digits_set(ETK_SPINNER(UI_DelayRangeSpinner), 3);
   etk_widget_size_request_set(UI_DelayRangeSpinner,45, 20);
   etk_table_attach_default(ETK_TABLE(table),UI_DelayRangeSpinner, 3, 3, 3, 3);

   //UI_AfterEntry
   label = etk_label_new("<b>After(s)</b>");
   etk_table_attach (ETK_TABLE(table), label, 0, 0, 4, 4,ETK_TABLE_NONE,0,0);
   UI_AfterEntry = etk_entry_new();
   etk_tooltips_tip_set(UI_AfterEntry, "Specifies program(s) to run after the current program completes.<br>The <i>source</i> and <i>signal</i> parameters of a program run as an <i>after</j> are ignored.<br>Multiple programs can be specified separated by ',' (comma).");
   etk_table_attach_default(ETK_TABLE(table),UI_AfterEntry, 1, 3, 4, 4);

   //UI_ActionComboBox
   Etk_Combobox_Item *item = NULL;
   label = etk_label_new("<b>Action</b>");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 5, 5, ETK_TABLE_NONE, 0, 0);
   UI_ActionComboBox = etk_combobox_new();
   //etk_tooltips_tip_set(UI_ActionComboBox, "<b>Action</b> to be performed by the program.<br>STATE_SET is used to change the state of one or more targets parts<br>, ACTION_STOP and SIGNAL_EMIT.<br>");
   etk_combobox_column_add(ETK_COMBOBOX(UI_ActionComboBox),
      ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_ActionComboBox),
      ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_ActionComboBox));
   etk_table_attach_default(ETK_TABLE(table),UI_ActionComboBox, 1, 3, 5, 5);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_ActionComboBox),
                  etk_image_new_from_edje(EdjeFile,"DESC.PNG"), "None");
   etk_combobox_item_data_set(item, (void*)EDJE_ACTION_TYPE_NONE);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_ActionComboBox),
                  etk_image_new_from_edje(EdjeFile,"DESC.PNG"), "State Set");
   etk_combobox_item_data_set(item, (void*)EDJE_ACTION_TYPE_STATE_SET);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_ActionComboBox),
                  etk_image_new_from_edje(EdjeFile,"DESC.PNG"), "Action Stop");
   etk_combobox_item_data_set(ETK_COMBOBOX_ITEM(item),
                              (void*)EDJE_ACTION_TYPE_ACTION_STOP);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_ActionComboBox),
                  etk_image_new_from_edje(EdjeFile,"DESC.PNG"), "Signal Emit");
   etk_combobox_item_data_set(ETK_COMBOBOX_ITEM(item),
                              (void*)EDJE_ACTION_TYPE_SIGNAL_EMIT);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_ActionComboBox),
                  etk_image_new_from_edje(EdjeFile,"DESC.PNG"), "Drag val set");
   etk_combobox_item_data_set(ETK_COMBOBOX_ITEM(item),
                              (void*)EDJE_ACTION_TYPE_DRAG_VAL_SET);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_ActionComboBox),
                  etk_image_new_from_edje(EdjeFile,"DESC.PNG"), "Drag val step");
   etk_combobox_item_data_set(ETK_COMBOBOX_ITEM(item),
                              (void*)EDJE_ACTION_TYPE_DRAG_VAL_STEP);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_ActionComboBox),
                  etk_image_new_from_edje(EdjeFile,"DESC.PNG"), "Drag val page");
   etk_combobox_item_data_set(ETK_COMBOBOX_ITEM(item),
                              (void*)EDJE_ACTION_TYPE_DRAG_VAL_PAGE);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_ActionComboBox),
                  etk_image_new_from_edje(EdjeFile,"DESC.PNG"), "Embryo script");
   etk_combobox_item_data_set(ETK_COMBOBOX_ITEM(item),
                              (void*)EDJE_ACTION_TYPE_SCRIPT);

   //UI_TargetEntry
   UI_TargetLabel = etk_label_new("<b>Target(s)</b>");
   etk_table_attach(ETK_TABLE(table),UI_TargetLabel,0,0,6,6,ETK_TABLE_NONE,0,0);
   UI_TargetEntry = etk_entry_new();
   etk_tooltips_tip_set(UI_TargetEntry, "Program or part on which the specified action acts.<br>Multiple target keywords may be specified, separated by ','(comma).");
   etk_table_attach_default(ETK_TABLE(table), UI_TargetEntry, 1, 4, 6, 6);

   //UI_Param1Entry
   UI_Param1Label = etk_label_new("<b>State</b>");
   etk_table_attach(ETK_TABLE(table), UI_Param1Label, 0, 0, 7, 7,ETK_TABLE_NONE,0,0);
   UI_Param1Entry = etk_entry_new();
   etk_tooltips_tip_set(UI_Param1Entry, "Description to set in the specified targets");
   etk_table_attach_default(ETK_TABLE(table), UI_Param1Entry, 1, 2, 7, 7);

   //Param1Spinner
   UI_Param1Spinner = etk_spinner_new(-999.0, 999.0, 0.0, 0.1, 1.0);
   etk_tooltips_tip_set(UI_Param1Spinner, "Description to set in the specified targets");
   etk_spinner_digits_set(ETK_SPINNER(UI_Param1Spinner), 2);
   etk_widget_size_request_set(UI_Param1Spinner,45, 20);
   etk_table_attach_default(ETK_TABLE(table),UI_Param1Spinner, 3, 3, 7, 7);

   //UI_TransiComboBox
   UI_TransiLabel = etk_label_new("<b>Transition</b>");
   etk_table_attach(ETK_TABLE(table), UI_TransiLabel, 0, 0, 8, 8,ETK_TABLE_NONE,0,0);
   UI_TransiComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_TransiComboBox),
      ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_TransiComboBox),
      ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_TransiComboBox));
   etk_table_attach_default(ETK_TABLE(table),UI_TransiComboBox, 1, 2, 8, 8);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_TransiComboBox),
                  etk_image_new_from_edje(EdjeFile,"DESC.PNG"), "Linear");
   etk_combobox_item_data_set(item, (void*)EDJE_TWEEN_MODE_LINEAR);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_TransiComboBox),
                  etk_image_new_from_edje(EdjeFile,"DESC.PNG"), "Sinusoidal");
   etk_combobox_item_data_set(item, (void*)EDJE_TWEEN_MODE_SINUSOIDAL);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_TransiComboBox),
                  etk_image_new_from_edje(EdjeFile,"DESC.PNG"), "Accelerate");
   etk_combobox_item_data_set(item, (void*)EDJE_TWEEN_MODE_ACCELERATE);

   item = etk_combobox_item_append(ETK_COMBOBOX(UI_TransiComboBox),
                  etk_image_new_from_edje(EdjeFile,"DESC.PNG"), "Decelerate");
   etk_combobox_item_data_set(item, (void*)EDJE_TWEEN_MODE_DECELERATE);

   //DurationSpinner
   UI_DurationSpinner = etk_spinner_new(0.0, 999.0, 0.0, 0.1, 1.0);
   etk_tooltips_tip_set(UI_DurationSpinner, "The duration of the transition. In seconds.");
   etk_spinner_digits_set(ETK_SPINNER(UI_DurationSpinner), 3);
   etk_widget_size_request_set(UI_DurationSpinner,45, 20);
   etk_table_attach_default(ETK_TABLE(table),UI_DurationSpinner, 3, 3, 8, 8);

   //UI_Param2Entry
   UI_Param2Label = etk_label_new("<b>Source</b>");
   etk_table_attach(ETK_TABLE(table), UI_Param2Label, 0, 0, 9, 9,ETK_TABLE_NONE,0,0);
   UI_Param2Entry = etk_entry_new();
   etk_tooltips_tip_set(UI_Param2Entry, "!!!!!!");
   etk_table_attach_default(ETK_TABLE(table), UI_Param2Entry, 1, 2, 9, 9);

   //Param2Spinner
   UI_Param2Spinner = etk_spinner_new(-999.0, 999.0, 0.0, 0.1, 1.0);
   etk_tooltips_tip_set(UI_Param2Spinner, "Used for drag operation");
   etk_spinner_digits_set(ETK_SPINNER(UI_Param2Spinner), 2);
   etk_widget_size_request_set(UI_Param2Spinner,45, 20);
   etk_table_attach_default(ETK_TABLE(table),UI_Param2Spinner, 3, 3, 9, 9);

   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_ActionComboBox),
         ETK_CALLBACK(_program_ActionComboBox_changed_cb), NULL);
   
   etk_signal_connect("text-changed", ETK_OBJECT(UI_ProgramEntry),
         ETK_CALLBACK(_group_NamesEntry_text_changed_cb), NULL);   
   etk_signal_connect("key-down", ETK_OBJECT(UI_ProgramEntry),
         ETK_CALLBACK(_program_Entry_key_down_cb), NULL);
   etk_signal_connect("mouse-click", ETK_OBJECT(UI_ProgramEntryImage),
                      ETK_CALLBACK(_program_EntryImage_clicked_cb), NULL);

   etk_signal_connect("text-changed", ETK_OBJECT(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SourceEntry))),
         ETK_CALLBACK(_program_SourceEntry_text_changed_cb), UI_SourceEntry);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_SourceEntry),
         ETK_CALLBACK(_program_SourceEntry_item_changed_cb), NULL);

   etk_signal_connect("text-changed", ETK_OBJECT(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SignalEntry))),
         ETK_CALLBACK(_program_SignalEntry_text_changed_cb), UI_SignalEntry);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_SignalEntry),
         ETK_CALLBACK(_program_SignalEntry_item_changed_cb), NULL);

   etk_signal_connect("value-changed", ETK_OBJECT(UI_DelayFromSpinner),
         ETK_CALLBACK(_program_DelaySpinners_value_changed_cb),NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_DelayRangeSpinner),
         ETK_CALLBACK(_program_DelaySpinners_value_changed_cb),NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_TargetEntry),
         ETK_CALLBACK(_program_TargetEntry_text_changed_cb), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_Param1Entry),
         ETK_CALLBACK(_program_Param1Entry_text_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Param1Spinner),
         ETK_CALLBACK(_program_Param1Spinner_value_changed_cb), NULL);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_TransiComboBox),
         ETK_CALLBACK(_program_TransitionComboBox_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_DurationSpinner),
         ETK_CALLBACK(_program_DurationSpinner_value_changed_cb), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_AfterEntry),
         ETK_CALLBACK(_program_AfterEntry_text_changed_cb), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_Param2Entry),
         ETK_CALLBACK(_program_Param2Entry_text_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Param2Spinner),
         ETK_CALLBACK(_program_Param2Spinner_value_changed_cb), NULL);

   return table;
}

void
program_frame_update(void)
{
   const char *s;
   Evas_List *l;

   if (!etk_string_length_get(Cur.prog)) return;

   //Stop signal propagation
   etk_signal_block("text-changed", ETK_OBJECT(UI_ProgramEntry),
         ETK_CALLBACK(_group_NamesEntry_text_changed_cb), NULL);
   etk_signal_block("text-changed",
         ETK_OBJECT(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SourceEntry))),
         ETK_CALLBACK(_program_SourceEntry_text_changed_cb), UI_SourceEntry);
   etk_signal_block("text-changed",
         ETK_OBJECT(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SignalEntry))),
         ETK_CALLBACK(_program_SignalEntry_text_changed_cb), UI_SignalEntry);
   etk_signal_block("value-changed", ETK_OBJECT(UI_DelayFromSpinner),
         ETK_CALLBACK(_program_DelaySpinners_value_changed_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_DelayRangeSpinner),
         ETK_CALLBACK(_program_DelaySpinners_value_changed_cb), NULL);
   etk_signal_block("text-changed", ETK_OBJECT(UI_TargetEntry),
         ETK_CALLBACK(_program_TargetEntry_text_changed_cb), NULL);
   etk_signal_block("text-changed", ETK_OBJECT(UI_Param1Entry),
         ETK_CALLBACK(_program_Param1Entry_text_changed_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Param1Spinner),
         ETK_CALLBACK(_program_Param1Spinner_value_changed_cb), NULL);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_TransiComboBox),
         ETK_CALLBACK(_program_TransitionComboBox_changed_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_DurationSpinner),
         ETK_CALLBACK(_program_DurationSpinner_value_changed_cb), NULL);
   etk_signal_block("text-changed", ETK_OBJECT(UI_AfterEntry),
         ETK_CALLBACK(_program_AfterEntry_text_changed_cb), NULL);
   etk_signal_block("text-changed", ETK_OBJECT(UI_Param2Entry),
         ETK_CALLBACK(_program_Param2Entry_text_changed_cb), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Param2Spinner),
         ETK_CALLBACK(_program_Param2Spinner_value_changed_cb), NULL);

   //Update Program Entry (name)
   etk_entry_text_set(ETK_ENTRY(UI_ProgramEntry), Cur.prog->string);
   etk_widget_hide(ETK_WIDGET(UI_ProgramEntryImage));

   //Update Source
   s = edje_edit_program_source_get(edje_o, Cur.prog->string);
   etk_entry_text_set(ETK_ENTRY(etk_combobox_entry_entry_get(
                      ETK_COMBOBOX_ENTRY(UI_SourceEntry))),s);
   edje_edit_string_free(s);

   //Update Signal
   s = edje_edit_program_signal_get(edje_o, Cur.prog->string);
   etk_entry_text_set(ETK_ENTRY(etk_combobox_entry_entry_get(
                      ETK_COMBOBOX_ENTRY(UI_SignalEntry))),s);
   edje_edit_string_free(s);

   //Update Delay
   etk_range_value_set(ETK_RANGE(UI_DelayFromSpinner),
                       edje_edit_program_in_from_get(edje_o, Cur.prog->string));
   etk_range_value_set(ETK_RANGE(UI_DelayRangeSpinner),
                      edje_edit_program_in_range_get(edje_o, Cur.prog->string));

   //Update Action
   int action;
   action = edje_edit_program_action_get(edje_o, Cur.prog->string);
   etk_combobox_active_item_set(ETK_COMBOBOX(UI_ActionComboBox),
         etk_combobox_nth_item_get(ETK_COMBOBOX(UI_ActionComboBox),action));


   //Update Target(s)
   Etk_String *str = etk_string_new("");
   l = edje_edit_program_targets_get(edje_o, Cur.prog->string);
   for (; l; l = l->next)
   {
      str = etk_string_append(str, l->data);
      str = etk_string_append(str, "|");
   }
   if (str->length > 1)
      str = etk_string_truncate(str, str->length - 1);
   etk_entry_text_set(ETK_ENTRY(UI_TargetEntry),str->string);

   //Update Afters(s)
   str = etk_string_clear(str);
   l = edje_edit_program_afters_get(edje_o, Cur.prog->string);
   for (; l; l = l->next)
   {
      str = etk_string_append(str,l->data);
      str = etk_string_append(str,"|");
   }
   if (str->length > 1)
      str = etk_string_truncate(str, str->length - 1);
   etk_entry_text_set(ETK_ENTRY(UI_AfterEntry),str->string);

   //Update state & state value
   s = edje_edit_program_state_get(edje_o, Cur.prog->string);
   etk_entry_text_set(ETK_ENTRY(UI_Param1Entry),s);
   etk_range_value_set(ETK_RANGE(UI_Param1Spinner),
                       edje_edit_program_value_get(edje_o, Cur.prog->string));
   edje_edit_string_free(s);

   s = edje_edit_program_state2_get(edje_o, Cur.prog->string);
   etk_entry_text_set(ETK_ENTRY(UI_Param2Entry),s);
   etk_range_value_set(ETK_RANGE(UI_Param2Spinner),
                        edje_edit_program_value2_get(edje_o, Cur.prog->string));
   edje_edit_string_free(s);

   //Update Transition
   etk_combobox_active_item_set(ETK_COMBOBOX(UI_TransiComboBox),
      etk_combobox_nth_item_get(ETK_COMBOBOX(UI_TransiComboBox),
         edje_edit_program_transition_get(edje_o, Cur.prog->string)-1));

   //Update Transition Duration
   etk_range_value_set(ETK_RANGE(UI_DurationSpinner),
               edje_edit_program_transition_time_get(edje_o, Cur.prog->string));

   //Reenable signal propagation
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_ProgramEntry),
         ETK_CALLBACK(_group_NamesEntry_text_changed_cb), NULL);
   etk_signal_unblock("text-changed",
         ETK_OBJECT(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SourceEntry))),
         ETK_CALLBACK(_program_SourceEntry_text_changed_cb), UI_SourceEntry);
   etk_signal_unblock("text-changed",
         ETK_OBJECT(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SignalEntry))),
         ETK_CALLBACK(_program_SignalEntry_text_changed_cb), UI_SignalEntry);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_DelayFromSpinner),
         ETK_CALLBACK(_program_DelaySpinners_value_changed_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_DelayRangeSpinner),
         ETK_CALLBACK(_program_DelaySpinners_value_changed_cb), NULL);
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_TargetEntry),
         ETK_CALLBACK(_program_TargetEntry_text_changed_cb), NULL);
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_Param1Entry),
         ETK_CALLBACK(_program_Param1Entry_text_changed_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Param1Spinner),
         ETK_CALLBACK(_program_Param1Spinner_value_changed_cb), NULL);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_TransiComboBox),
         ETK_CALLBACK(_program_TransitionComboBox_changed_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_DurationSpinner),
         ETK_CALLBACK(_program_DurationSpinner_value_changed_cb), NULL);
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_AfterEntry),
         ETK_CALLBACK(_program_AfterEntry_text_changed_cb), NULL);
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_Param2Entry),
         ETK_CALLBACK(_program_Param2Entry_text_changed_cb), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Param2Spinner),
         ETK_CALLBACK(_program_Param2Spinner_value_changed_cb), NULL);
   etk_object_destroy(ETK_OBJECT(str));
}

void
program_source_combo_populate(void)
{
   Evas_List *l;
   char *image_name;
   printf("Populate Program Source ComboEntry\n");

   //Stop signal propagation
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_SourceEntry),
                  ETK_CALLBACK(_program_SourceEntry_item_changed_cb), NULL);

   etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(UI_SourceEntry));

   l = edje_edit_parts_list_get(edje_o);
   while (l)
   {
      image_name = part_type_image_get((char*)l->data);
      etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SourceEntry),
                  etk_image_new_from_edje(EdjeFile, image_name),
                  (char *)l->data);
      free(image_name);

      l = l->next;
   }
   edje_edit_string_list_free(l);

   //Renable  signal propagation
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_SourceEntry),
                     ETK_CALLBACK(_program_SourceEntry_item_changed_cb), NULL);
}

void
program_signal_combo_populate(void)
{
   printf("Populate Program Signal ComboEntry\n");

   //Stop signal propagation
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_SignalEntry),
                  ETK_CALLBACK(_program_SignalEntry_item_changed_cb), NULL);

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
                     ETK_CALLBACK(_program_SignalEntry_item_changed_cb), NULL);
}


/***   Callbacks   ***/
Etk_Bool
_program_ActionComboBox_changed_cb(Etk_Combobox *combobox, void *data)
{
   int action;
   printf("Changed Signal on ActionComboBox EMITTED\n");

   if (!etk_string_length_get(Cur.prog)) return ETK_TRUE;

   //Get the new action from the combo data
   action = (int)(long)etk_combobox_item_data_get(
               etk_combobox_active_item_get(combobox));

   edje_edit_program_action_set(edje_o, Cur.prog->string, action);
   
   if (action == EDJE_ACTION_TYPE_SIGNAL_EMIT)
   {
      etk_widget_hide(UI_TargetEntry);
      etk_widget_hide(UI_TargetLabel);
      etk_widget_hide(UI_TransiComboBox);
      etk_widget_hide(UI_TransiLabel);
      etk_widget_hide(UI_DurationSpinner);
      etk_widget_show(UI_Param1Entry);
      etk_widget_show(UI_Param1Label);
      etk_label_set(ETK_LABEL(UI_Param1Label), "<b>Signal</b>");
      etk_label_set(ETK_LABEL(UI_Param2Label), "<b>Source</b>");
      etk_widget_hide(UI_Param1Spinner);
      etk_widget_show(UI_Param2Label);
      etk_widget_show(UI_Param2Entry);
   }
   if (action == EDJE_ACTION_TYPE_STATE_SET)
   {
      etk_widget_show(UI_TargetEntry);
      etk_widget_show(UI_TargetLabel);
      etk_widget_show(UI_TransiComboBox);
      etk_widget_show(UI_TransiLabel);
      etk_widget_show(UI_DurationSpinner);
      etk_widget_show(UI_Param1Entry);
      etk_widget_show(UI_Param1Label);
      etk_label_set(ETK_LABEL(UI_Param1Label), "<b>State</b>");
      etk_widget_show(UI_Param1Spinner);
      etk_widget_hide(UI_Param2Label);
      etk_widget_hide(UI_Param2Entry);
      etk_widget_hide(UI_Param2Spinner);
   }
   if (action == EDJE_ACTION_TYPE_ACTION_STOP)
   {
      etk_widget_show(UI_TargetEntry);
      etk_widget_show(UI_TargetLabel);
      etk_widget_hide(UI_TransiComboBox);
      etk_widget_hide(UI_TransiLabel);
      etk_widget_hide(UI_DurationSpinner);
      etk_widget_hide(UI_Param1Entry);
      etk_widget_hide(UI_Param1Label);
      etk_widget_hide(UI_Param1Spinner);
      etk_widget_hide(UI_Param2Label);
      etk_widget_hide(UI_Param2Entry);
      etk_widget_hide(UI_Param2Spinner);
   }
   if (action == EDJE_ACTION_TYPE_NONE ||
       action == EDJE_ACTION_TYPE_SCRIPT)
   {
      etk_widget_hide(UI_TargetEntry);
      etk_widget_hide(UI_TargetLabel);
      etk_widget_hide(UI_TransiComboBox);
      etk_widget_hide(UI_TransiLabel);
      etk_widget_hide(UI_DurationSpinner);
      etk_widget_hide(UI_Param1Entry);
      etk_widget_hide(UI_Param1Label);
      etk_widget_hide(UI_Param1Spinner);
      etk_widget_hide(UI_Param2Label);
      etk_widget_hide(UI_Param2Entry);
      etk_widget_hide(UI_Param2Spinner);
   }
   if (action == EDJE_ACTION_TYPE_DRAG_VAL_SET ||
       action == EDJE_ACTION_TYPE_DRAG_VAL_STEP ||
       action == EDJE_ACTION_TYPE_DRAG_VAL_PAGE)
   {
      etk_widget_hide(UI_TargetEntry);
      etk_widget_hide(UI_TargetLabel);
      etk_widget_hide(UI_TransiComboBox);
      etk_widget_hide(UI_TransiLabel);
      etk_widget_hide(UI_DurationSpinner);
      etk_label_set(ETK_LABEL(UI_Param1Label), "<b>? ? ? ?</b>");
      etk_label_set(ETK_LABEL(UI_Param2Label), "<b>? ? ? ?</b>");
      etk_widget_hide(UI_Param1Entry);
      etk_widget_show(UI_Param1Label);
      etk_widget_show(UI_Param1Spinner);
      etk_widget_show(UI_Param2Label);
      etk_widget_hide(UI_Param2Entry);
      etk_widget_show(UI_Param2Spinner);
   }

   return ETK_TRUE;
}

Etk_Bool
_program_Entry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   printf("PRESSED %s\n", event->keyname);

   if (!strcmp(event->keyname, "Return"))
      _program_EntryImage_clicked_cb(
                                 ETK_OBJECT(ETK_ENTRY(object)->secondary_image),
                                 NULL);
   return ETK_TRUE;
}

Etk_Bool
_program_EntryImage_clicked_cb(Etk_Object *object, void *data)
{
   const char *name;

   printf("Mouse Click Signal on ProgramEntryImage Emitted\n");

   name = etk_entry_text_get(ETK_ENTRY(UI_ProgramEntry));

   if (!name || !etk_string_length_get(Cur.prog)) return ETK_TRUE;

   if (!strcmp(name, Cur.prog->string))
   {
      etk_widget_hide(ETK_WIDGET(UI_ProgramEntryImage));
      return ETK_TRUE;
   }

   if (edje_edit_program_name_set(edje_o, Cur.prog->string, name))
   {
      /* update tree */
      Etk_Tree_Row *row;
      row = etk_tree_selected_row_get(ETK_TREE(UI_PartsTree));
      etk_tree_row_fields_set(row,TRUE,
                              COL_NAME, EdjeFile, "PROG.PNG", name,
                              NULL);
      /* update Cur */
      Cur.prog = etk_string_set(Cur.prog, name);

      /* Hide the image */
      etk_widget_hide(ETK_WIDGET(UI_ProgramEntryImage));
   }
   else
      dialog_alert_show("Can't rename program.<br>Another program with this name just exist?");

   return ETK_TRUE;
}

Etk_Bool
_program_SourceEntry_text_changed_cb(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on SourceEntry Emitted\n");
   const char *str = etk_entry_text_get(ETK_ENTRY(object));
   edje_edit_program_source_set(edje_o, Cur.prog->string, str);
   return ETK_TRUE;
}

Etk_Bool
_program_SourceEntry_item_changed_cb(Etk_Combobox_Entry *combo, void *data)
{
   Etk_Combobox_Entry_Item *active_item = NULL;
   char *pname;

   printf("Item Changed Signal on SourceEntry Emitted\n");

   if (!(active_item = etk_combobox_entry_active_item_get(combo)))
      return ETK_TRUE;

   etk_combobox_entry_item_fields_get(active_item, NULL, &pname, NULL);

   etk_entry_text_set(ETK_ENTRY(etk_combobox_entry_entry_get(
                      ETK_COMBOBOX_ENTRY(UI_SourceEntry))),pname);

   return ETK_TRUE;
}

Etk_Bool
_program_SignalEntry_item_changed_cb(Etk_Combobox_Entry *combo, void *data)
{
   Etk_Combobox_Entry_Item *active_item = NULL;
   char *pname;

   printf("Item Changed Signal on SignalEntry Emitted\n");

   if (!(active_item = etk_combobox_entry_active_item_get(combo)))
      return ETK_TRUE;

   etk_combobox_entry_item_fields_get(active_item, NULL, &pname, NULL);

   etk_entry_text_set(ETK_ENTRY(etk_combobox_entry_entry_get(
                      ETK_COMBOBOX_ENTRY(UI_SignalEntry))),pname);

   return ETK_TRUE;
}

Etk_Bool
_program_SignalEntry_text_changed_cb(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on SignalEntry Emitted\n");
   const char *str = etk_entry_text_get(ETK_ENTRY(object));
   edje_edit_program_signal_set(edje_o, Cur.prog->string, str);
   return ETK_TRUE;
}

Etk_Bool
_program_DelaySpinners_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("value Changed Signal on DelayFromSpinner Emitted\n");
   edje_edit_program_in_from_set(edje_o, Cur.prog->string,
                           etk_range_value_get(ETK_RANGE(UI_DelayFromSpinner)));

   edje_edit_program_in_range_set(edje_o, Cur.prog->string,
                           etk_range_value_get(ETK_RANGE(UI_DelayRangeSpinner)));

   return ETK_TRUE;
}

Etk_Bool
_program_TargetEntry_text_changed_cb(Etk_Object *object, void *data)
{
   char *text = strdup(etk_entry_text_get(ETK_ENTRY(object)));
   char *tok;

   printf("Text Changed Signal on TargetEntry Emitted (text: %s)\n",text);

   //Empty current targets list
   edje_edit_program_targets_clear(edje_o, Cur.prog->string);

   //Spit the string in token and add every targets
   tok = strtok(text,"|");
   while (tok != NULL)
   {
      printf("'%s'\n",tok);
      edje_edit_program_target_add(edje_o, Cur.prog->string, tok);
      tok = strtok(NULL, "|");
   }

   //TODO Check if all the targets exists in the group, otherwise make the text red

   free(text);
   return ETK_TRUE;
}

Etk_Bool
_program_Param1Entry_text_changed_cb(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on Param1Entry Emitted\n");

   edje_edit_program_state_set(edje_o, Cur.prog->string,
                               etk_entry_text_get(ETK_ENTRY(UI_Param1Entry)));

   return ETK_TRUE;
}

Etk_Bool
_program_Param2Entry_text_changed_cb(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on Param2Entry Emitted\n");

   edje_edit_program_state2_set(edje_o, Cur.prog->string,
                               etk_entry_text_get(ETK_ENTRY(UI_Param2Entry)));

   return ETK_TRUE;
}

Etk_Bool
_program_Param1Spinner_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("value Changed Signal on Param1Spinner Emitted\n");
   edje_edit_program_value_set(edje_o, Cur.prog->string,
                              etk_range_value_get(ETK_RANGE(UI_Param1Spinner)));
   return ETK_TRUE;
}

Etk_Bool
_program_Param2Spinner_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("value Changed Signal on Param2Spinner Emitted\n");
   edje_edit_program_value2_set(edje_o, Cur.prog->string,
                              etk_range_value_get(ETK_RANGE(UI_Param2Spinner)));
   return ETK_TRUE;
}

Etk_Bool
_program_TransitionComboBox_changed_cb(Etk_Combobox *combobox, void *data)
{
   int trans;
   printf("Changed Signal on TransitionComboBox Emitted\n");

   //get the transition from the combo data
   trans = (int)(long)etk_combobox_item_data_get(etk_combobox_active_item_get(combobox));
   edje_edit_program_transition_set(edje_o, Cur.prog->string, trans);

   return ETK_TRUE;
}

Etk_Bool
_program_DurationSpinner_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("value Changed Signal on DurationSpinner Emitted\n");
   if (etk_string_length_get(Cur.prog))
      edje_edit_program_transition_time_set(edje_o, Cur.prog->string,
                           etk_range_value_get(ETK_RANGE(UI_DurationSpinner)));

   return ETK_TRUE;
}

Etk_Bool
_program_AfterEntry_text_changed_cb(Etk_Object *object, void *data)
{
   char *text = strdup(etk_entry_text_get(ETK_ENTRY(object)));
   char *tok;

   printf("Text Changed Signal on AfterEntry Emitted (text: %s)\n",text);

   //Empty current afters list
   edje_edit_program_afters_clear(edje_o, Cur.prog->string);

   //Spit the string in token and add every afters
   tok = strtok (text,"|");
   while (tok != NULL)
   {
      printf ("'%s'\n",tok);
      edje_edit_program_after_add(edje_o, Cur.prog->string, tok);
      tok = strtok (NULL, "|");
   }

   //TODO Check if all the after exists in the group, otherwise make the text red

   free(text);
   return ETK_TRUE;
}
