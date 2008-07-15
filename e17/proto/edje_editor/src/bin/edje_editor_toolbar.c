#include <string.h>
#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


/***   Implementation   ***/
Etk_Widget*
toolbar_create(Etk_Toolbar_Orientation o)
{
   Etk_Widget *button;
   Etk_Widget *sep;
   Etk_Widget *menu_item;
   Etk_Widget *image;

   //ToolBar
   UI_Toolbar = etk_toolbar_new();
   etk_toolbar_orientation_set(ETK_TOOLBAR(UI_Toolbar), o);
   //etk_object_properties_set(ETK_OBJECT(UI_Toolbar),"theme-group","etk/iconbox/label");
   //etk_theme_edje_object_set(Evas_Object *object, const char *file, const char *group, const char *parent_group)

   //NewButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_NEW);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_NEW);

   //OpenButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_OPEN);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_OPEN);

   //SaveButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Save", NULL);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_SAVE);

   //SaveEDJButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE_AS);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Save as", NULL);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_SAVE_EDJ);

   //SaveEDCButton
 /*  button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE_AS);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Export edc", NULL);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                     ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_SAVE_EDC);
   */

   sep = etk_vseparator_new();
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), sep, ETK_BOX_START);

   //AddButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_LIST_ADD);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_ADD);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);

   //AddMenu
   UI_AddMenu = etk_menu_new();

   //New Rectangle
   menu_item = etk_menu_item_image_new_with_label("Rectangle");
   image = etk_image_new_from_edje(EdjeFile,"RECT.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_toolbar_add_menu_item_activated_cb),
                      (void*)NEW_RECT);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //New Image
   menu_item = etk_menu_item_image_new_with_label("Image");
   image = etk_image_new_from_edje(EdjeFile,"IMAGE.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_toolbar_add_menu_item_activated_cb),
                      (void*) NEW_IMAGE);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //New Gradient
   menu_item = etk_menu_item_image_new_with_label("Gradient");
   image = etk_image_new_from_edje(EdjeFile,"GRAD_LINEAR.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_toolbar_add_menu_item_activated_cb),
                      (void*) NEW_GRADIENT);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //New Text
   menu_item = etk_menu_item_image_new_with_label("Text");
   image = etk_image_new_from_edje(EdjeFile,"TEXT.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_toolbar_add_menu_item_activated_cb),
                      (void*)NEW_TEXT);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //New Swallow
   menu_item = etk_menu_item_image_new_with_label("Swallow");
   image = etk_image_new_from_edje(EdjeFile,"SWAL.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_toolbar_add_menu_item_activated_cb),
                      (void*)NEW_SWAL);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //New GroupSwallow
   menu_item = etk_menu_item_image_new_with_label("Group swallow");
   image = etk_image_new_from_edje(EdjeFile,"GROUP.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_toolbar_add_menu_item_activated_cb),
                      (void*)NEW_GROUPSWAL);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //New Program
   menu_item = etk_menu_item_image_new_with_label("Program");
   image = etk_image_new_from_edje(EdjeFile,"PROG.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_toolbar_add_menu_item_activated_cb),
                      (void*)NEW_PROG);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //Separator
   menu_item = etk_menu_item_separator_new();
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //New Description
   UI_AddStateButton = etk_menu_item_image_new_with_label("A new state to part");
   image = etk_image_new_from_edje(EdjeFile,"DESC.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(UI_AddStateButton), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(UI_AddStateButton),
                      ETK_CALLBACK(_toolbar_add_menu_item_activated_cb),
                      (void*)NEW_DESC);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(UI_AddStateButton));

   //New Group
   menu_item = etk_menu_item_image_new_with_label("A new group to edj");
   image = etk_image_new_from_edje(EdjeFile,"NONE.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_toolbar_add_menu_item_activated_cb),
                      (void*)NEW_GROUP);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //RemoveButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_LIST_REMOVE);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_REMOVE);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);

   //RemoveMenu
   UI_RemoveMenu = etk_menu_new();

   //description
   UI_RemoveStateButton = etk_menu_item_image_new_with_label("Selected State");
   image = etk_image_new_from_edje(EdjeFile,"DESC.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(UI_RemoveStateButton), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(UI_RemoveStateButton),
                      ETK_CALLBACK(_toolbar_remove_menu_item_activated_cb),
                      (void*)REMOVE_DESCRIPTION);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_RemoveMenu), ETK_MENU_ITEM(UI_RemoveStateButton));

   //part
   UI_RemovePartButton = etk_menu_item_image_new_with_label("Selected Part");
   etk_signal_connect("activated", ETK_OBJECT(UI_RemovePartButton),
                      ETK_CALLBACK(_toolbar_remove_menu_item_activated_cb),
                      (void*)REMOVE_PART);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_RemoveMenu), ETK_MENU_ITEM(UI_RemovePartButton));

   //program 
   UI_RemoveProgramButton = etk_menu_item_image_new_with_label("Selected Program");
   image = etk_image_new_from_edje(EdjeFile,"PROG.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(UI_RemoveProgramButton), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(UI_RemoveProgramButton),
                      ETK_CALLBACK(_toolbar_remove_menu_item_activated_cb),
                      (void*)REMOVE_PROG);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_RemoveMenu), ETK_MENU_ITEM(UI_RemoveProgramButton));
    
   sep = etk_vseparator_new();
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), sep, ETK_BOX_START);

   //group
   menu_item = etk_menu_item_image_new_with_label("Current Group");
   image = etk_image_new_from_edje(EdjeFile, "NONE.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_toolbar_remove_menu_item_activated_cb),
                      (void*)REMOVE_GROUP);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_RemoveMenu), ETK_MENU_ITEM(menu_item));

   //MoveUp Button
   button = etk_tool_button_new_from_stock( ETK_STOCK_GO_UP);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Lower", NULL);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_MOVE_UP);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);

   //MoveDown Button
   button = etk_tool_button_new_from_stock( ETK_STOCK_GO_DOWN);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Raise", NULL);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_MOVE_DOWN);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);

   sep = etk_vseparator_new();
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), sep, ETK_BOX_START);

   //Images Browser Button
   button = etk_tool_button_new_from_stock( ETK_STOCK_IMAGE_X_GENERIC);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Images", NULL);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_IMAGE_BROWSER);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);

   //Spectrum Button
   button = etk_tool_button_new_with_label("Spectrum");
   image = etk_image_new_from_edje(EdjeFile, "SPECTRA.PNG");
   etk_widget_size_request_set(image, 22, 22);
   etk_button_image_set(ETK_BUTTON(button), ETK_IMAGE(image));
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_SPECTRUM);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);

#if DEBUG_MODE
   //Font Browser Button
   button = etk_tool_button_new_from_stock( ETK_STOCK_PREFERENCES_DESKTOP_FONT);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Fonts", NULL);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_FONT_BROWSER);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
#endif

   sep = etk_vseparator_new();
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), sep, ETK_BOX_START);

   //OptionsButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_PREFERENCES_SYSTEM);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Options", NULL);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_OPTIONS);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);

   //OptionsMenu
   UI_OptionsMenu = etk_menu_new();

   //set bg1
   menu_item = etk_menu_item_image_new_with_label("White squared background");
   image = etk_image_new_from_edje(EdjeFile,"BG1_I.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_OPTION_BG1);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_OptionsMenu), ETK_MENU_ITEM(menu_item));

   //set bg2
   menu_item = etk_menu_item_image_new_with_label("Gray squared background");
   image = etk_image_new_from_edje(EdjeFile,"BG2_I.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_OPTION_BG2);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_OptionsMenu), ETK_MENU_ITEM(menu_item));

   //set bg3
   menu_item = etk_menu_item_image_new_with_label("White background");
   image = etk_image_new_from_edje(EdjeFile,"BG3_I.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_OPTION_BG3);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_OptionsMenu), ETK_MENU_ITEM(menu_item));

   //set bg4
   menu_item = etk_menu_item_image_new_with_label("Black background");
   image = etk_image_new_from_edje(EdjeFile,"BG4_I.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_OPTION_BG4);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_OptionsMenu), ETK_MENU_ITEM(menu_item));
   
   //Separator
   menu_item = etk_menu_item_separator_new();
   etk_menu_shell_append(ETK_MENU_SHELL(UI_OptionsMenu), ETK_MENU_ITEM(menu_item));

   //Fullscreen
   menu_item = etk_menu_item_image_new_with_label("Toggle fullscreen");
   image = etk_image_new_from_stock(ETK_STOCK_VIDEO_DISPLAY, ETK_STOCK_SMALL);
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_OPTION_FULLSCREEN);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_OptionsMenu), ETK_MENU_ITEM(menu_item));

   sep = etk_vseparator_new();
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), sep, ETK_BOX_START);

   //UI_PlayButton
   UI_PlayImage = etk_image_new_from_stock(ETK_STOCK_MEDIA_PLAYBACK_START, ETK_STOCK_MEDIUM);
   UI_PauseImage = etk_image_new_from_stock(ETK_STOCK_MEDIA_PLAYBACK_PAUSE, ETK_STOCK_MEDIUM);
   UI_PlayButton = etk_tool_button_new();
   etk_button_image_set (ETK_BUTTON(UI_PlayButton), ETK_IMAGE(UI_PauseImage));
   etk_object_properties_set(ETK_OBJECT(UI_PlayButton), "label", "Pause Edje", NULL);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), UI_PlayButton, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(UI_PlayButton),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_PLAY);

#if DEBUG_MODE
   //Separator
   sep = etk_vseparator_new();
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), sep, ETK_BOX_START);

   //DebugButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_PROPERTIES);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_DEBUG);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Debug", NULL);
#endif

   //QuitButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_SYSTEM_LOG_OUT);
   etk_object_properties_set(ETK_OBJECT(button), "label", "Quit", NULL);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                      ETK_CALLBACK(_window_all_button_click_cb),
                      (void*)TOOLBAR_QUIT);

   return UI_Toolbar;
}

void
toolbar_play_button_toggle(int set)
{
   /* set    -1 = toggle   0 = pause   1 = play   */
   if (set == -1)
      set = !edje_object_play_get(edje_o);

   if (set == 0)
   {
      edje_object_play_set(edje_o, 0);
      etk_button_image_set(ETK_BUTTON(UI_PlayButton), ETK_IMAGE(UI_PlayImage));
      etk_object_properties_set(ETK_OBJECT(UI_PlayButton), "label", "Play Edje", NULL);
   }
   else if (set == 1)
   {
      edje_object_play_set(edje_o, 1);
      etk_button_image_set(ETK_BUTTON(UI_PlayButton), ETK_IMAGE(UI_PauseImage));
      etk_object_properties_set(ETK_OBJECT(UI_PlayButton), "label", "Pause Edje", NULL);
   }
}


/* Add/Remove Buttons Callbacks */
Etk_Bool
_toolbar_add_menu_item_activated_cb(Etk_Object *object, void *data)
{
   printf("Item Activated Signal on AddMenu EMITTED\n");
   Etk_Tree_Row *row;
   Etk_Combobox_Item *item;
   int i;
   char name[32];
   char name2[32];

   switch ((int)(long)data)
   {
      case NEW_RECT:
         if (!etk_string_length_get(Cur.group))
         {
            dialog_alert_show("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New rectangle");
         i = 2;
         while (edje_edit_part_exist(edje_o, name))
            snprintf(name, sizeof(name), "New rectangle %d", i++);
         
         if (!edje_edit_part_add(edje_o, name, EDJE_PART_TYPE_RECTANGLE))
         {
            dialog_alert_show("Can't create part.");
            break;
         }

         row = tree_part_add(name, NULL);
         etk_tree_row_select(row);
         etk_tree_row_unfold(row);
         position_comboboxes_populate();
         program_source_combo_populate();
         break;

      case NEW_IMAGE:
         if (!etk_string_length_get(Cur.group))
         {
            dialog_alert_show("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New image");
         i = 2;
         while (edje_edit_part_exist(edje_o, name))
            snprintf(name, sizeof(name), "New image %d", i++);

         if (!edje_edit_part_add(edje_o, name, EDJE_PART_TYPE_IMAGE))
         {
            dialog_alert_show("Can't create part.");
            break;
         }
         row = tree_part_add(name, NULL);

         Evas_List *images;
         images = edje_edit_images_list_get(edje_o);
         if (images)
         {
            if (images->data)
               edje_edit_state_image_set(edje_o, name,
                                         "default 0.00", images->data);
         }

         etk_tree_row_select(row);
         etk_tree_row_unfold(row);
         position_comboboxes_populate();
         program_source_combo_populate();
         break;
      case NEW_GRADIENT:
         if (!etk_string_length_get(Cur.group))
         {
            dialog_alert_show("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New gradient");
         i = 2;
         while (edje_edit_part_exist(edje_o, name))
            snprintf(name, sizeof(name), "New gradient %d", i++);

         if (!edje_edit_part_add(edje_o, name, EDJE_PART_TYPE_GRADIENT))
         {
            dialog_alert_show("Can't create gradient.");
            break;
         }
         edje_edit_state_gradient_type_set(edje_o, name, "default 0.00", "linear");
         row = tree_part_add(name, NULL);

         etk_tree_row_select(row);
         etk_tree_row_unfold(row);
         position_comboboxes_populate();
         program_source_combo_populate();
         break;
      case NEW_TEXT:
         if (!etk_string_length_get(Cur.group))
         {
            dialog_alert_show("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New text");
         i = 2;
         while (edje_edit_part_exist(edje_o, name))
            snprintf(name, sizeof(name), "New text %d", i++);

         if (!edje_edit_part_add(edje_o, name, EDJE_PART_TYPE_TEXT))
         {
            dialog_alert_show("Can't create part.");
            break;
         }
         row = tree_part_add(name, NULL);

         char *font;
         item = etk_combobox_first_item_get(ETK_COMBOBOX(UI_FontComboBox));
         if (item)
         {
            font = etk_combobox_item_field_get(item, 1);
            if (font)
               edje_edit_state_font_set(edje_o, name, "default 0.00", font);
         }
         edje_edit_state_text_size_set(edje_o, name, "default 0.00", 16);
         edje_edit_state_text_set(edje_o, name, "default 0.00",
                                  "Something to say !");
         edje_edit_part_effect_set(edje_o, name, EDJE_TEXT_EFFECT_GLOW);

         etk_tree_row_select(row);
         etk_tree_row_unfold(row);
         position_comboboxes_populate();
         program_source_combo_populate();
         break;
      case NEW_SWAL:
         if (!etk_string_length_get(Cur.group))
         {
            dialog_alert_show("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New swallow");
         i = 2;
         while (edje_edit_part_exist(edje_o, name))
            snprintf(name, sizeof(name), "New swallow %d", i++);

         if (!edje_edit_part_add(edje_o, name, EDJE_PART_TYPE_SWALLOW))
         {
            dialog_alert_show("Can't create part.");
            break;
         }
         row = tree_part_add(name, NULL);
         etk_tree_row_select(row);
         etk_tree_row_unfold(row);
         position_comboboxes_populate();
         program_source_combo_populate();
         break;
      case NEW_GROUPSWAL:
         if (!etk_string_length_get(Cur.group))
         {
            dialog_alert_show("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New group swallow");
         i = 2;
         while (edje_edit_part_exist(edje_o, name))
            snprintf(name, sizeof(name), "New group swallow %d", i++);

         if (!edje_edit_part_add(edje_o, name, EDJE_PART_TYPE_GROUP))
         {
            dialog_alert_show("Can't create part.");
            break;
         }
         row = tree_part_add(name, NULL);
         etk_tree_row_select(row);
         etk_tree_row_unfold(row);
         position_comboboxes_populate();
         program_source_combo_populate();

         break;
      case NEW_DESC:
         if (!etk_string_length_get(Cur.part))
         {
            dialog_alert_show("You must first select a part.");
            break;
         }

         //Generate a unique name
         snprintf(name, sizeof(name), "state");
         snprintf(name2, sizeof(name2), "state 0.00");
         i = 2;
         while (edje_edit_state_exist(edje_o, Cur.part->string, name2))
         {
            snprintf(name, sizeof(name), "state%d", i++);
            snprintf(name2, sizeof(name2), "%s 0.00", name);
         }

         //Create state
         edje_edit_state_add(edje_o, Cur.part->string, name);

         edje_edit_state_rel1_relative_x_set(edje_o, Cur.part->string,name2, 0.1);
         edje_edit_state_rel1_relative_y_set(edje_o, Cur.part->string,name2, 0.1);
         edje_edit_state_rel2_relative_x_set(edje_o, Cur.part->string,name2, 0.9);
         edje_edit_state_rel2_relative_y_set(edje_o, Cur.part->string,name2, 0.9);
         edje_edit_state_text_size_set(edje_o, Cur.part->string, name2, 16);

         if (edje_edit_part_type_get(edje_o, Cur.part->string) == EDJE_PART_TYPE_GRADIENT)
            edje_edit_state_gradient_type_set(edje_o, Cur.part->string, name2, "linear");

         //Add state to tree
         row = tree_state_add(Cur.part->string, name2);
         etk_tree_row_select(row);
         etk_tree_row_unfold(evas_hash_find(Parts_Hash,Cur.part->string));
         break;

      case NEW_PROG:
         if (!etk_string_length_get(Cur.group))
         {
            dialog_alert_show("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New program");
         i = 2;
         while (edje_edit_program_exist(edje_o, name))
            snprintf(name, sizeof(name), "New program %d", i++);

         if (!edje_edit_program_add(edje_o, name))
         {
            dialog_alert_show("ERROR: can't add program");
            break;
         }
         row = tree_program_add(name);
         etk_tree_row_select(row);
         etk_tree_row_scroll_to(row, ETK_FALSE);
         break;

      case NEW_GROUP:
         //generate a unique new name
         snprintf(name, sizeof(name), "New group");
         i = 2;
         while (edje_edit_group_exist(edje_o, name))
            snprintf(name, sizeof(name), "New group %d", i++);

         if (edje_edit_group_add(edje_o, name))
         {
            tree_combobox_populate();
            etk_combobox_active_item_set(ETK_COMBOBOX(UI_GroupsComboBox),
               etk_combobox_last_item_get(ETK_COMBOBOX(UI_GroupsComboBox)));
         }
         else
         {
            dialog_alert_show("Can't create group.");
         }
         break;
   }
   canvas_redraw();
   return ETK_TRUE;
}

Etk_Bool
_toolbar_remove_menu_item_activated_cb(Etk_Object *object, void *data)
{
   Etk_Tree_Row *row, *next;
   printf("Item Activated Signal on RemoveMenu EMITTED\n");

   switch ((int)(long)data)
   {
      case REMOVE_DESCRIPTION:
         if (!etk_string_length_get(Cur.state))
         {
            dialog_alert_show("No part state selected");
            break;
         }
         if (!strcmp(Cur.state->string,"default 0.00"))
         {
            dialog_alert_show("You can't remove default 0.0");
            break;
         }
         edje_edit_state_del(edje_o, Cur.part->string, Cur.state->string);

         // Select next row (if no exist select prev); and delete current.
         row = etk_tree_selected_row_get(ETK_TREE(UI_PartsTree));
         next = etk_tree_row_next_get(row);
         if (!next) 
            next = etk_tree_row_prev_get(row);
         etk_tree_row_select(next);
         etk_tree_row_delete(row);

         break;
      case REMOVE_PART:
         if (!etk_string_length_get(Cur.part))
         {
            dialog_alert_show("No part selected");
            break;
         }
         if (!edje_edit_part_del(edje_o, Cur.part->string))
         {
            dialog_alert_show("Can't delete part");
            break;
         }

         row = evas_hash_find(Parts_Hash, Cur.part->string);
         Parts_Hash = evas_hash_del(Parts_Hash, Cur.part->string, NULL);

         next = etk_tree_row_next_get(row);
         if (!next) 
            next = etk_tree_row_prev_get(row);
         etk_tree_row_delete(row);
         if (next)
            etk_tree_row_select(next);
         else
         {
            Cur.part = etk_string_clear(Cur.part);
            Cur.state = etk_string_clear(Cur.state);
         }

         position_comboboxes_populate();
         program_source_combo_populate();
         //ev_redraw();
         break;

      case REMOVE_GROUP:
         if (!edje_edit_group_del(edje_o))
         {
            dialog_alert_show("Can't delete group");
            break;
         }
         Etk_Combobox_Item *item, *nitem;
         item = etk_combobox_active_item_get(ETK_COMBOBOX(UI_GroupsComboBox));

         nitem = etk_combobox_item_next_get(item);
         if (!nitem)
            nitem = etk_combobox_item_prev_get(item);

         etk_combobox_active_item_set(ETK_COMBOBOX(UI_GroupsComboBox), nitem);
         etk_combobox_item_remove(item);
         break;

      case REMOVE_PROG:
         if (!etk_string_length_get(Cur.prog))
         {
            dialog_alert_show("You must first select a program");
         }
         if (!edje_edit_program_del(edje_o, Cur.prog->string))
         {
            dialog_alert_show("Can't delete program");
            break;
         }
         row = etk_tree_selected_row_get(ETK_TREE(UI_PartsTree));
         next = etk_tree_row_next_get(row);
         if (!next) 
            next = etk_tree_row_prev_get(row);
         etk_tree_row_delete(row);
         if (next)
            etk_tree_row_select(next);
      
         break;
   }
   return ETK_TRUE;
}
