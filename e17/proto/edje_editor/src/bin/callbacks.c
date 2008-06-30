#include <string.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include <Etk.h>
#include <Ecore_Evas.h>
#include "callbacks.h"
#include "interface.h"
#include "inout.h"
#include "main.h"
#include "evas.h"


static int current_color_object;

/* Called when the window is destroyed */
void
ecore_delete_cb(Ecore_Evas *ee)
{
   etk_main_quit();
}

/* Called when the window is resized */
void
ecore_resize_callback(Ecore_Evas *ecore_evas)
{
   Evas_Object *embed_object;
   int win_w, win_h;
   
   //Get window size
   ecore_evas_geometry_get(UI_ecore_MainWin, NULL, NULL, &win_w, &win_h);
   
   //Resize main edje interface
   evas_object_resize(edje_ui, win_w, win_h);
   
   //Resize tree
   embed_object = etk_embed_object_get(ETK_EMBED(UI_PartsTreeEmbed));
   evas_object_move(embed_object, 0, 55);
   evas_object_resize(embed_object, TREE_WIDTH, win_h - 55);
   
   //Resize Consolle
   evas_object_move(Consolle, TREE_WIDTH + 5, win_h - 80);
   evas_object_resize(Consolle, win_w - TREE_WIDTH - 10, 75);
}
/* Catch all the signal from the editing edje object */
void
signal_cb(void *data, Evas_Object *o, const char *sig, const char *src)
{
   char buf[1024];
   consolle_count++;
   snprintf(buf, sizeof(buf), "[%d]  SIGNAL = '%s'     SOURCE = '%s'",
            consolle_count, sig, src);
   ConsolleLog(buf);
}

/* Group combobox callback */
Etk_Bool
on_GroupsComboBox_activated(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
{
   char *gr;
   gr = etk_combobox_item_field_get(item,0);
   //printf("Group combo activated: %s\n",gr);
   ChangeGroup(gr);
   
   return ETK_TRUE;
}

/* All the buttons Callback */
Etk_Bool
on_AllButton_click(Etk_Button *button, void *data)
{
   Etk_String *text;
   const char *tween;
   Etk_Tree_Row *row, *next, *prev;
   Etk_Combobox_Item *item;
   Etk_Iconbox_Icon *icon;

   switch ((int)data)
      {
   case TOOLBAR_NEW:
      system("edje_editor &");
      break;
   case TOOLBAR_OPEN:
      ShowFilechooser(FILECHOOSER_OPEN);
      break;
   case TOOLBAR_SAVE:
      if (!etk_string_length_get(Cur.edj_file_name))
      {
         ShowFilechooser(FILECHOOSER_SAVE_EDJ);
         break;
      }

      edje_edit_save(edje_o);
      if (!ecore_file_cp(Cur.edj_temp_name->string, Cur.edj_file_name->string))
      {
         ShowAlert("<b>ERROR:<\b><br>Can't write file");
      }
      break;
   case TOOLBAR_SAVE_EDC:
      ShowAlert("Not yet reimplemented ;)");
      break;
   case TOOLBAR_SAVE_EDJ:
      ShowFilechooser(FILECHOOSER_SAVE_EDJ);
      break;
   case TOOLBAR_ADD:
      if (!etk_string_length_get(Cur.part))
         etk_widget_disabled_set(UI_AddStateButton, 1);
      else etk_widget_disabled_set(UI_AddStateButton, 0);
      etk_menu_popup(ETK_MENU(UI_AddMenu));
      break;
   case TOOLBAR_REMOVE:
      if (!etk_string_length_get(Cur.state))
         etk_widget_disabled_set(UI_RemoveStateButton, 1);
      else etk_widget_disabled_set(UI_RemoveStateButton, 0);
      if (!etk_string_length_get(Cur.part))
         etk_widget_disabled_set(UI_RemovePartButton, 1);
      else etk_widget_disabled_set(UI_RemovePartButton, 0);
      if (!etk_string_length_get(Cur.prog))
         etk_widget_disabled_set(UI_RemoveProgramButton, 1);
      else etk_widget_disabled_set(UI_RemoveProgramButton, 0);

      etk_menu_popup(ETK_MENU(UI_RemoveMenu));
      break;
   case TOOLBAR_QUIT:
      etk_main_quit();
      break;
   case TOOLBAR_MOVE_UP: //Lower
      if (!etk_string_length_get(Cur.part))
      {
         ShowAlert("You must select a part to lower");
         break;
      }
      if (!edje_edit_part_restack_below(edje_o, Cur.part->string))
         break;
      
      row = evas_hash_find(Parts_Hash, Cur.part->string);
      prev = etk_tree_row_prev_get(row);
      if (!prev) break;
      prev = etk_tree_row_prev_get(prev);
      
      Parts_Hash = evas_hash_del(Parts_Hash, NULL, row);
      etk_tree_row_delete(row);
      
      if (prev)
         row = AddPartToTree(Cur.part->string, prev);
      else
         row = AddPartToTree(Cur.part->string, (void*)1);
      etk_tree_row_select(row);
      //Reload the edje if needed
      if (edje_edit_part_type_get(edje_o, Cur.part->string) == EDJE_PART_TYPE_GROUP)
         ReloadEdje();
      break;
   
   case TOOLBAR_MOVE_DOWN: //Raise
      if (!etk_string_length_get(Cur.part))
      {
         ShowAlert("You must select a part to lower");
         break;
      }
      if (!edje_edit_part_restack_above(edje_o, Cur.part->string))
         break;
      
      row = evas_hash_find(Parts_Hash, Cur.part->string);
      next = etk_tree_row_next_get(row);
      if (!next) break;
      
      Parts_Hash = evas_hash_del(Parts_Hash, NULL, row);
      etk_tree_row_delete(row);
      row = AddPartToTree(Cur.part->string, next);
      etk_tree_row_select(row);
      //Reload the edje if needed
      if (edje_edit_part_type_get(edje_o, Cur.part->string) == EDJE_PART_TYPE_GROUP)
         ReloadEdje();
      break;
   case IMAGE_BROWSER_SHOW:
      ShowImageBrowser(1);
      break;
   case IMAGE_BROWSER_CLOSE:
      etk_widget_hide(ETK_WIDGET(UI_ImageBrowserWin));
      break;
   case TOOLBAR_IMAGE_BROWSER:
      ShowImageBrowser(0);
      break;
   case TOOLBAR_FONT_BROWSER:
      ShowAlert("Font Browser");
      break;
   case TOOLBAR_IMAGE_FILE_ADD:
      ShowFilechooser(FILECHOOSER_IMAGE);
      break;
   case TOOLBAR_FONT_FILE_ADD:
      ShowFilechooser(FILECHOOSER_FONT);
      break;
   case IMAGE_TWEEN_ADD:
      icon = etk_iconbox_icon_get_selected(ETK_ICONBOX(UI_ImageBrowserIconbox));
      tween = etk_iconbox_icon_label_get(icon);
      if (tween)
      {
         if(edje_edit_state_tween_add(edje_o, Cur.part->string, Cur.state->string, tween))
         {
            PopulateTweenList();
            row = etk_tree_last_row_get(ETK_TREE(UI_ImageTweenList));
            etk_tree_row_select(row);
            etk_tree_row_scroll_to(row, ETK_FALSE);
         }
      }
      else
      {
         ShowAlert("You must choose an image to add from the Image Browser");
      }
      break;
   case IMAGE_TWEEN_DELETE:
      //TODO delete the correct tween (not the first with that name)
      if (!etk_string_length_get(Cur.tween)) break;
      printf("REMOVE TWEEN %s\n", Cur.tween->string);
      edje_edit_state_tween_del(edje_o, Cur.part->string, Cur.state->string,
                                   Cur.tween->string);
      row = etk_tree_selected_row_get(ETK_TREE(UI_ImageTweenList));
      next = etk_tree_row_next_get(row);
      if (!next) 
         next = etk_tree_row_prev_get(row);
      if (next)
         etk_tree_row_select(next);
      else
      {
         Cur.tween = etk_string_clear(Cur.tween);
         etk_widget_disabled_set(UI_DeleteTweenButton, TRUE);
         etk_widget_disabled_set(UI_MoveDownTweenButton, TRUE);
         etk_widget_disabled_set(UI_MoveUpTweenButton, TRUE);
      }
      etk_tree_row_delete(row);
      break;
   case TOOLBAR_OPTIONS:
      etk_menu_popup(ETK_MENU(UI_OptionsMenu));
      //etk_menu_popup_at_xy (ETK_MENU(AddMenu), 10, 10);
      break;
   case TOOLBAR_OPTION_BG1:
      edje_object_signal_emit(edje_ui,"set_bg1","edje_editor");
      break;
    case TOOLBAR_OPTION_BG2:
      edje_object_signal_emit(edje_ui,"set_bg2","edje_editor");
      break;
   case TOOLBAR_OPTION_BG3:
      edje_object_signal_emit(edje_ui,"set_bg3","edje_editor");
      break;
   case TOOLBAR_OPTION_BG4:
      edje_object_signal_emit(edje_ui,"set_bg4","edje_editor");
      break;
   case TOOLBAR_OPTION_FULLSCREEN:
      Cur.fullscreen = !Cur.fullscreen;
      ecore_evas_fullscreen_set(UI_ecore_MainWin, Cur.fullscreen);
      break;
   case TOOLBAR_PLAY:
      TogglePlayButton(-1);
      break;
   case TOOLBAR_DEBUG:
      //DebugInfo(FALSE);
      //ChangeGroup("New group");
      //edje_edit_group_add(edje_o, "dai cazzo");
      //on_AddMenu_item_activated(NULL, NEW_RECT);
      edje_edit_print_internal_status(edje_o);
      break;
   case IMAGE_TWEEN_UP:
      ShowAlert("Up not yet implemented.");
      break;
   case IMAGE_TWEEN_DOWN:
      ShowAlert("Down not yet implemented.");
      break;
   case RUN_PROG:
      if (etk_string_length_get(Cur.prog))
         edje_edit_program_run(edje_o, Cur.prog->string);
      break;
   case SAVE_SCRIPT:
      text = etk_textblock_text_get(ETK_TEXT_VIEW(UI_ScriptBox)->textblock,
                                    ETK_TRUE);
      ShowAlert("Script not yet implemented.");
      etk_object_destroy(ETK_OBJECT(text));
      break;
   default:
      break;
      }

   return ETK_TRUE;
}

void
on_Editing_click(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Object *o2;
   int x, y;
   Evas_Event_Mouse_Down *ev = event_info;
   
  	evas_pointer_output_xy_get(e, &x, &y);
   
   printf("CLIK\n");
   //o2 = evas_object_top_at_pointer_get(e);
   Evas_List *l =	evas_objects_at_xy_get (e, ev->canvas.x, ev->canvas.y, 1, 1);
   printf("CLIK %x [%d %d] num: %d\n", obj, ev->canvas.x, ev->canvas.y, evas_list_count(l));
   
}

void
on_Mainwin_key_press(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;
  
   printf("*** Logo receive key pressed\n");
   printf("   keyname: %s\n", ev->keyname);
   printf("   key: %s\n", ev->key);
   printf("   string: %s\n", ev->string);
   printf("   compose: %s\n", ev->compose);
	
   
   /* NOTE: To add new bindings you must add a keygrab for the key
      you want in create_main_window(). And remember to update the README */
   
   /* quit */
   if (!strcmp(ev->key, "q") &&
       evas_key_modifier_is_set(ev->modifiers, "Control"))
      etk_main_quit();
   
   /* fullscreen */
   else if (!strcmp(ev->key, "f") &&
            evas_key_modifier_is_set(ev->modifiers, "Control"))
   {    
      Cur.fullscreen = !Cur.fullscreen;
      ecore_evas_fullscreen_set(UI_ecore_MainWin, Cur.fullscreen); 	
   }

   /* save (TODO make some sort of feedback for the user)*/
   else if (!strcmp(ev->key, "s") &&
            evas_key_modifier_is_set(ev->modifiers, "Control"))
      on_AllButton_click(NULL, TOOLBAR_SAVE);
   
   
}

/* Image Browser callbacks */
Etk_Bool 
on_ImageBrowserIconbox_selected(Etk_Iconbox *iconbox, Etk_Iconbox_Icon *icon, void *data)
{
   const char *image;
   image = etk_iconbox_icon_label_get(icon);
   
   printf("CLICK ON: %s :)\n", image);
   if (ImageBroserUpdate && etk_string_length_get(Cur.part) && etk_string_length_get(Cur.state))
     {
        edje_edit_state_image_set(edje_o, Cur.part->string, Cur.state->string, image);
     }
   UpdateImageFrame();
}

/* Tree callbacks */
Etk_Bool
on_PartsTree_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   int row_type=0;
   char *name;
   char *parent_name;

   printf("Row Selected Signal on one of the Tree EMITTED \n");

   //get the info from the tree cols of the selected row
   etk_tree_row_fields_get(row,
                           COL_TYPE, &row_type,
                           COL_NAME,NULL, NULL, &name,
                           COL_PARENT, &parent_name,
                           NULL);

   switch (row_type)
   {
      case ROW_PART:
         Cur.part = etk_string_set(Cur.part, name);
         Cur.state = etk_string_clear(Cur.state);
         Cur.tween = etk_string_clear(Cur.tween);
         Cur.prog = etk_string_clear(Cur.prog);
         
         edje_object_signal_emit(edje_ui,"description_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"position_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"rect_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"image_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"text_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"group_frame_show","edje_editor");
         edje_object_signal_emit(edje_ui,"program_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"part_frame_show","edje_editor");
         edje_object_signal_emit(edje_ui,"script_frame_hide","edje_editor");
         
         UpdatePartFrame();
         break;

      case ROW_DESC:
         Cur.state = etk_string_set(Cur.state, name);
         Cur.part = etk_string_set(Cur.part, parent_name);
         Cur.tween = etk_string_clear(Cur.tween);
         Cur.prog = etk_string_clear(Cur.prog);
       
         edje_edit_part_selected_state_set(edje_o, Cur.part->string, Cur.state->string);  
         
         UpdateDescriptionFrame();
         UpdatePositionFrame();
         UpdateComboPositionFrame();
       
         switch(edje_edit_part_type_get(edje_o, Cur.part->string))
         {
            case EDJE_PART_TYPE_RECTANGLE:
               UpdateRectFrame();
               edje_object_signal_emit(edje_ui,"rect_frame_show","edje_editor");
               edje_object_signal_emit(edje_ui,"image_frame_hide","edje_editor");
               edje_object_signal_emit(edje_ui,"text_frame_hide","edje_editor");
               break;
            case EDJE_PART_TYPE_IMAGE:
               UpdateImageFrame();
               edje_object_signal_emit(edje_ui,"image_frame_show","edje_editor");
               edje_object_signal_emit(edje_ui,"rect_frame_hide","edje_editor");
               edje_object_signal_emit(edje_ui,"text_frame_hide","edje_editor");
               break;
            case EDJE_PART_TYPE_TEXT:
               UpdateTextFrame();
               edje_object_signal_emit(edje_ui,"text_frame_show","edje_editor");
               edje_object_signal_emit(edje_ui,"image_frame_hide","edje_editor");
               edje_object_signal_emit(edje_ui,"rect_frame_hide","edje_editor");
               break;
         }
         
         edje_object_signal_emit(edje_ui,"part_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"group_frame_show","edje_editor");
         edje_object_signal_emit(edje_ui,"program_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"script_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"description_frame_show","edje_editor");
         edje_object_signal_emit(edje_ui,"position_frame_show","edje_editor");
         break;
      
      case ROW_PROG:
         Cur.prog = etk_string_set(Cur.prog, name);
         Cur.part = etk_string_clear(Cur.part);
         Cur.state = etk_string_clear(Cur.state);
         Cur.tween = etk_string_clear(Cur.tween);
       
         edje_object_signal_emit(edje_ui,"description_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"position_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"rect_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"image_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"text_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"group_frame_show","edje_editor");
         edje_object_signal_emit(edje_ui,"part_frame_hide","edje_editor");
         
         edje_object_signal_emit(edje_ui,"program_frame_show","edje_editor");
         edje_object_signal_emit(edje_ui,"script_frame_show_small","edje_editor");
      
         UpdateScriptFrame();
         UpdateProgFrame();
        // PopulateSourceComboBox();
         break;
   }

   ev_redraw();
   return ETK_TRUE;
}

/* Group frame callbacks */
Etk_Bool
on_GroupNameEntry_key_down(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   //printf("PRESSED %s\n", event->keyname);
   
   if (!strcmp(event->keyname, "Return"))
      on_GroupNameEntryImage_mouse_clicked(
                                 ETK_OBJECT(ETK_ENTRY(object)->secondary_image),
                                 NULL);
   return ETK_TRUE;
}

Etk_Bool
on_GroupNameEntryImage_mouse_clicked(Etk_Object *object, void *data)
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
      ShowAlert("Can't rename group.<br>Another group with this name exist?");
      return ETK_TRUE;
   }
   
   //Update Group Combobox
   Etk_Combobox_Item *item;
   item = etk_combobox_active_item_get(ETK_COMBOBOX(UI_GroupsComboBox));
   etk_signal_block("item-activated",ETK_OBJECT(UI_GroupsComboBox),
                    on_GroupsComboBox_activated, NULL);
   etk_combobox_item_fields_set(item, name);
   etk_signal_unblock("item-activated",ETK_OBJECT(UI_GroupsComboBox),
                      on_GroupsComboBox_activated, NULL);
    
   //Update FakeWin title
   edje_object_part_text_set(EV_fakewin, "title", name);
   
   /* Hide the entry image */
   etk_widget_hide(ETK_WIDGET(UI_GroupNameEntryImage));
   
   return ETK_TRUE;
}

Etk_Bool
on_GroupSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Group Spinners value changed signal EMIT\n");
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   switch ((int)data)
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

/* Parts & Descriptions Callbacks*/
Etk_Bool
on_PartNameEntry_key_down(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   printf("PRESSED %s\n", event->keyname);
   
   if (!strcmp(event->keyname, "Return"))
      on_PartNameEntryImage_mouse_clicked(
                                 ETK_OBJECT(ETK_ENTRY(object)->secondary_image),
                                 NULL);
   return ETK_TRUE;
}

Etk_Bool
on_PartNameEntryImage_mouse_clicked(Etk_Object *object, void *data)
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
      ShowAlert("Can't set part name.<br>Another name with this name exist? ");
      return ETK_TRUE;
   }
   
   /* Set new Current name */
   Cur.part = etk_string_set(Cur.part, name);
   
   //Update PartTree
   row = etk_tree_selected_row_get(ETK_TREE(UI_PartsTree));
   image_name = GetPartTypeImage(edje_edit_part_type_get(edje_o, Cur.part->string));
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
   PopulateRelComboBoxes();
   PopulateSourceComboEntry();
   
   /* Hide the image */
   etk_widget_hide(ETK_WIDGET(UI_PartNameEntryImage));

   
   return ETK_TRUE;
}

Etk_Bool
on_CliptoComboBox_item_activated(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
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
on_PartSourceComboBox_item_activated(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
{
   char *gr;
   printf("Item Activated Signal on PartSourceCombobox EMITTED\n");
   
   gr = etk_combobox_item_field_get(item,0);
   
   if (!strcmp(gr, Cur.group->string))
   {
      ShowAlert("A group can't contain itself");
      return ETK_TRUE;
   }
   
   if (strcmp(gr, "None"))
      edje_edit_part_source_set(edje_o, Cur.part->string, gr);
   else
      edje_edit_part_source_set(edje_o, Cur.part->string, NULL);
   
   ReloadEdje();
   
   return ETK_TRUE;
}

Etk_Bool
on_PartEventsCheck_toggled(Etk_Object *object, void *data)
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
on_PartEventsRepeatCheck_toggled(Etk_Object *object, void *data)
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
on_StateEntry_key_down(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   printf("PRESSED %s\n", event->keyname);
   if (!strcmp("default 0.00", Cur.state->string))
   {
      ShowAlert("You can't rename default 0.0");
      return ETK_TRUE;
   }
   
   if (!strcmp(event->keyname, "Return"))
      on_StateEntryImage_mouse_clicked(
                                 ETK_OBJECT(ETK_ENTRY(object)->secondary_image),
                                 NULL);
   return ETK_TRUE;
}

Etk_Bool
on_StateEntryImage_mouse_clicked(Etk_Object *object, void *data)
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
         ShowAlert("<b>Wrong name format</b><br>Name must be in the form:<br>\"default 0.00\"");
   }
   else
      ShowAlert("You can't rename default 0.0");

   return ETK_TRUE;
}



Etk_Bool
on_AspectSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on AspectMinSpinner EMITTED\n");
   edje_edit_state_aspect_min_set(edje_o, Cur.part->string, Cur.state->string,
                           etk_range_value_get(ETK_RANGE(UI_AspectMinSpinner)));
   edje_edit_state_aspect_max_set(edje_o, Cur.part->string, Cur.state->string,
                           etk_range_value_get(ETK_RANGE(UI_AspectMaxSpinner)));
   return ETK_TRUE;
}

Etk_Bool
on_AspectComboBox_changed(Etk_Combobox *combobox, void *data)
{
   printf("Active Item Changed Signal on AspectComboBox EMITTED\n");
   int pref;
   pref = (int)etk_combobox_item_data_get(etk_combobox_active_item_get (combobox));
   edje_edit_state_aspect_pref_set(edje_o, Cur.part->string, Cur.state->string, pref);
   return ETK_TRUE;
}

Etk_Bool
on_StateMinMaxSpinner_value_changed(Etk_Range *range, double value, void *data)
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

   ev_redraw();
   return ETK_TRUE;
}

/* Image Frame Callbacks */
Etk_Bool
on_ImageComboBox_item_activated(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
{
   printf("Changed signal on Image Combo EMITTED\n");
   
   char *im;
   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   
   im = etk_combobox_item_field_get(item, 1);
   edje_edit_state_image_set(edje_o, Cur.part->string, Cur.state->string, im);

   return ETK_TRUE;
}

Etk_Bool
on_ImageTweenList_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_Tree_Col *col;
   char *selected = NULL;
   printf("Row selected signal on ImageTweenList EMITTED\n");
   
   col = etk_tree_nth_col_get(ETK_TREE(UI_ImageTweenList), 0);
   etk_tree_row_fields_get(row, col, NULL, NULL, &selected, NULL);
   if (!selected) return ETK_TRUE;
   
   Cur.tween = etk_string_set(Cur.tween, selected);
   etk_widget_disabled_set(UI_DeleteTweenButton, FALSE);
  // etk_widget_disabled_set(UI_MoveDownTweenButton, FALSE);
  // etk_widget_disabled_set(UI_MoveUpTweenButton, FALSE);
   
   return ETK_TRUE;
}

Etk_Bool
on_ImageAlphaSlider_value_changed(Etk_Object *object, double va, void *data)
{
   printf("ImageSlieder value_changed signale EMIT: %.2f\n",va);

   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   edje_edit_state_color_set(edje_o, Cur.part->string, Cur.state->string,
                             -1, -1, -1, (int)va);
   ev_redraw();

   return ETK_TRUE;
}

Etk_Bool
on_BorderSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed signal on BorderSpinner EMITTED (value: %f)\n",etk_range_value_get(range));

   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   edje_edit_state_image_border_set(edje_o, Cur.part->string, Cur.state->string,
      (int)etk_range_value_get(ETK_RANGE(UI_BorderLeftSpinner)),
      (int)etk_range_value_get(ETK_RANGE(UI_BorderRightSpinner)),
      (int)etk_range_value_get(ETK_RANGE(UI_BorderTopSpinner)),
      (int)etk_range_value_get(ETK_RANGE(UI_BorderBottomSpinner)));

   ev_redraw();

   return ETK_TRUE;
}

/* Position Frame Callbacks */
Etk_Bool
on_RelToComboBox_changed(Etk_Combobox *combobox, void *data)
{
   char *parent;
   parent = etk_combobox_item_field_get(etk_combobox_active_item_get(combobox), 1);
   
   if (strcmp(parent,"Interface") == 0)
        parent = NULL;
    
   if (parent && (strcmp(parent,Cur.part->string) == 0))
   {
      ShowAlert("A state can't rel to itself.");
      return ETK_TRUE;
   }
   
   switch ((int)data)
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
   ev_redraw();
   return ETK_TRUE;
}

Etk_Bool
on_RelSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on RelSpinner EMITTED (value: %f)\n",etk_range_value_get(range));

   if (etk_string_length_get(Cur.state) && etk_string_length_get(Cur.part))
   {
      switch ((int)data)
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
      ev_redraw();
   }
   return ETK_TRUE;
}

Etk_Bool
on_RelOffsetSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on Offset Spinner EMITTED\n");

   if (etk_string_length_get(Cur.state) && etk_string_length_get(Cur.part))
   {
      switch ((int)data)
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
      ev_redraw();
      //ev_draw_focus();
   }

   return ETK_TRUE;
}

/* Text Frame Callbacks */
Etk_Bool
on_FontComboBox_item_activated(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
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
on_EffectComboBox_changed(Etk_Combobox *combobox, void *data)
{
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   
   edje_edit_part_effect_set(edje_o, Cur.part->string,
      (int)etk_combobox_item_data_get(etk_combobox_active_item_get(combobox)));
   
   ev_redraw();

   return ETK_TRUE;
}

Etk_Bool
on_FontSizeSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on FontSizeSpinner EMITTED (value: %d)\n",(int)etk_range_value_get(range));

   edje_edit_state_text_size_set(edje_o, Cur.part->string, Cur.state->string,
                                 (int)etk_range_value_get(range));

   ev_redraw();
   return ETK_TRUE;
}

Etk_Bool
on_TextEntry_text_changed(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on TextEntry EMITTED (value %s)\n",etk_entry_text_get(ETK_ENTRY(object)));
   edje_edit_state_text_set(edje_o, Cur.part->string, Cur.state->string,
                            etk_entry_text_get(ETK_ENTRY(object)));

   ev_redraw();
   return ETK_TRUE;
}

Etk_Bool
on_FontAlignSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on AlignSpinner (h or v, text or part) EMITTED (value: %.2f)\n",etk_range_value_get(range));

   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   
   if ((int)data == TEXT_ALIGNH_SPINNER)
      edje_edit_state_text_align_x_set(edje_o, Cur.part->string, Cur.state->string,
                                       (double)etk_range_value_get(range));
   if ((int)data == TEXT_ALIGNV_SPINNER)
      edje_edit_state_text_align_y_set(edje_o, Cur.part->string, Cur.state->string,
                                       (double)etk_range_value_get(range));
   if ((int)data == STATE_ALIGNH_SPINNER)
      edje_edit_state_align_x_set(edje_o, Cur.part->string, Cur.state->string,
                                  (double)etk_range_value_get(range));
   if ((int)data == STATE_ALIGNV_SPINNER)
      edje_edit_state_align_y_set(edje_o, Cur.part->string, Cur.state->string,
                                  (double)etk_range_value_get(range));

   return ETK_TRUE;
}

/* Programs Callbacks */
Etk_Bool
on_ActionComboBox_changed(Etk_Combobox *combobox, void *data)
{
   int action;
   printf("Changed Signal on ActionComboBox EMITTED\n");
   
   if (!etk_string_length_get(Cur.prog)) return ETK_TRUE;

   //Get the new action from the combo data
   action = (int)etk_combobox_item_data_get(
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
on_ProgramEntry_key_down(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   printf("PRESSED %s\n", event->keyname);
   
   if (!strcmp(event->keyname, "Return"))
      on_ProgramEntryImage_mouse_clicked(
                                 ETK_OBJECT(ETK_ENTRY(object)->secondary_image),
                                 NULL);
   return ETK_TRUE;
}

Etk_Bool
on_ProgramEntryImage_mouse_clicked(Etk_Object *object, void *data)
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
      ShowAlert("Can't rename program.<br>Another program with this name just exist?");
   
   return ETK_TRUE;
}

Etk_Bool
on_SourceEntry_text_changed(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on SourceEntry Emitted\n");
   const char *str = etk_entry_text_get(ETK_ENTRY(object));
   edje_edit_program_source_set(edje_o, Cur.prog->string, str);
   return ETK_TRUE;
}

Etk_Bool
on_SourceEntry_item_changed(Etk_Combobox_Entry *combo, void *data)
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
on_SignalEntry_item_changed(Etk_Combobox_Entry *combo, void *data)
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
on_SignalEntry_text_changed(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on SignalEntry Emitted\n");
   const char *str = etk_entry_text_get(ETK_ENTRY(object));
   edje_edit_program_signal_set(edje_o, Cur.prog->string, str);
   return ETK_TRUE;
}

Etk_Bool
on_DelaySpinners_value_changed(Etk_Range *range, double value, void *data)
{
   printf("value Changed Signal on DelayFromSpinner Emitted\n");
   edje_edit_program_in_from_set(edje_o, Cur.prog->string,
                           etk_range_value_get(ETK_RANGE(UI_DelayFromSpinner)));
   
   edje_edit_program_in_range_set(edje_o, Cur.prog->string,
                           etk_range_value_get(ETK_RANGE(UI_DelayRangeSpinner)));
   
   return ETK_TRUE;
}

Etk_Bool
on_TargetEntry_text_changed(Etk_Object *object, void *data)
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
on_Param1Entry_text_changed(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on Param1Entry Emitted\n");

   edje_edit_program_state_set(edje_o, Cur.prog->string,
                               etk_entry_text_get(ETK_ENTRY(UI_Param1Entry)));
   
   return ETK_TRUE;
}

Etk_Bool
on_Param2Entry_text_changed(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on Param2Entry Emitted\n");

   edje_edit_program_state2_set(edje_o, Cur.prog->string,
                               etk_entry_text_get(ETK_ENTRY(UI_Param2Entry)));
   
   return ETK_TRUE;
}

Etk_Bool
on_Param1Spinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("value Changed Signal on Param1Spinner Emitted\n");
   edje_edit_program_value_set(edje_o, Cur.prog->string,
                              etk_range_value_get(ETK_RANGE(UI_Param1Spinner)));
   return ETK_TRUE;
}

Etk_Bool
on_Param2Spinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("value Changed Signal on Param2Spinner Emitted\n");
   edje_edit_program_value2_set(edje_o, Cur.prog->string,
                              etk_range_value_get(ETK_RANGE(UI_Param2Spinner)));
   return ETK_TRUE;
}

Etk_Bool
on_TransitionComboBox_changed(Etk_Combobox *combobox, void *data)
{
   int trans;
   printf("Changed Signal on TransitionComboBox Emitted\n");

   //get the transition from the combo data
   trans = (int)etk_combobox_item_data_get(etk_combobox_active_item_get(combobox));
   edje_edit_program_transition_set(edje_o, Cur.prog->string, trans);
   
   return ETK_TRUE;
}

Etk_Bool
on_DurationSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("value Changed Signal on DurationSpinner Emitted\n");
   if (etk_string_length_get(Cur.prog))
      edje_edit_program_transition_time_set(edje_o, Cur.prog->string,
                           etk_range_value_get(ETK_RANGE(UI_DurationSpinner)));
   
   return ETK_TRUE;
}

Etk_Bool
on_AfterEntry_text_changed(Etk_Object *object, void *data)
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

void
on_ColorCanvas_click(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Color c;
   printf("Clik Signal on ColorCanvas Emitted\n");
   if (UI_ColorWin) etk_widget_show_all(UI_ColorWin);
   current_color_object = (int)data;

   etk_signal_block("color-changed", ETK_OBJECT(UI_ColorPicker), ETK_CALLBACK(on_ColorDialog_change), NULL);
   switch (current_color_object)
   {
      case COLOR_OBJECT_RECT:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Rectangle color");
         edje_edit_state_color_get(edje_o, Cur.part->string, Cur.state->string, &c.r,&c.g,&c.b,&c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_TEXT:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Text color");
         edje_edit_state_color_get(edje_o, Cur.part->string, Cur.state->string, &c.r,&c.g,&c.b,&c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_SHADOW:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Shadow color");
         edje_edit_state_color3_get(edje_o, Cur.part->string, Cur.state->string, &c.r,&c.g,&c.b,&c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_OUTLINE:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Outline color");
         edje_edit_state_color2_get(edje_o, Cur.part->string, Cur.state->string, &c.r,&c.g,&c.b,&c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
   }
   etk_signal_unblock("color-changed", ETK_OBJECT(UI_ColorPicker), ETK_CALLBACK(on_ColorDialog_change), NULL);
}

Etk_Bool
on_ColorDialog_change(Etk_Object *object, void *data)
{
  // printf("ColorChangeSignal on ColorDialog EMITTED\n");
   Etk_Color color;
   Etk_Color premuled;

   color = etk_colorpicker_current_color_get(ETK_COLORPICKER(object));
 //  printf("Color: %d %d %d %d\n",color.r,color.g,color.b,color.a);

   if (color.r > 255) color.r = 255;
   if (color.g > 255) color.g = 255;
   if (color.b > 255) color.b = 255;
   if (color.a > 255) color.a = 255;

   if (color.r < 0) color.r = 0;
   if (color.g < 0) color.g = 0;
   if (color.b < 0) color.b = 0;
   if (color.a < 0) color.a = 0;

   premuled = color;
   evas_color_argb_premul(premuled.a,&premuled.r,&premuled.g,&premuled.b);

   switch (current_color_object){
    case COLOR_OBJECT_RECT:
      evas_object_color_set(RectColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
      edje_edit_state_color_set(edje_o, Cur.part->string, Cur.state->string,
                                premuled.r,premuled.g,premuled.b,premuled.a);

      break;
    case COLOR_OBJECT_TEXT:
      evas_object_color_set(TextColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
      edje_edit_state_color_set(edje_o, Cur.part->string, Cur.state->string,
                                premuled.r,premuled.g,premuled.b,premuled.a);

      break;
    case COLOR_OBJECT_SHADOW:
      evas_object_color_set(ShadowColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
      edje_edit_state_color3_set(edje_o, Cur.part->string, Cur.state->string,
                                premuled.r,premuled.g,premuled.b,premuled.a);

      break;
    case COLOR_OBJECT_OUTLINE:
      evas_object_color_set(OutlineColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
      edje_edit_state_color2_set(edje_o, Cur.part->string, Cur.state->string,
                                premuled.r,premuled.g,premuled.b,premuled.a);

      break;
   }

   ev_redraw();
   return ETK_TRUE;
}

/* Add/Remove Buttons Callbacks */
Etk_Bool
on_AddMenu_item_activated(Etk_Object *object, void *data)
{
   printf("Item Activated Signal on AddMenu EMITTED\n");
   Etk_Tree_Row *row;
   Etk_Combobox_Item *item;
   int i;
   char name[32];
   char name2[32];
   
   switch ((int)data)
   {
       case NEW_RECT:
         if (!etk_string_length_get(Cur.group))
         {
            ShowAlert("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New rectangle");
         i = 2;
         while (edje_edit_part_exist(edje_o, name))
            snprintf(name, sizeof(name), "New rectangle %d", i++);
         
         if (!edje_edit_part_add(edje_o, name, EDJE_PART_TYPE_RECTANGLE))
         {
            ShowAlert("Can't create part.");
            break;
         }
         
         row = AddPartToTree(name, NULL);
         etk_tree_row_select(row);
         etk_tree_row_unfold(row);
         PopulateRelComboBoxes();
         PopulateSourceComboEntry();
         break;
      
      case NEW_IMAGE:
         if (!etk_string_length_get(Cur.group))
         {
            ShowAlert("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New image");
         i = 2;
         while (edje_edit_part_exist(edje_o, name))
            snprintf(name, sizeof(name), "New image %d", i++);
      
         if (!edje_edit_part_add(edje_o, name, EDJE_PART_TYPE_IMAGE))
         {
            ShowAlert("Can't create part.");
            break;
         }
         //TODO generate a unique new name
         row = AddPartToTree(name, NULL);
         
         
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
         PopulateRelComboBoxes();
         PopulateSourceComboEntry();
         break;
      
      case NEW_TEXT:
         if (!etk_string_length_get(Cur.group))
         {
            ShowAlert("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New text");
         i = 2;
         while (edje_edit_part_exist(edje_o, name))
            snprintf(name, sizeof(name), "New text %d", i++);
         
         if (!edje_edit_part_add(edje_o, name, EDJE_PART_TYPE_TEXT))
         {
            ShowAlert("Can't create part.");
            break;
         }
         row = AddPartToTree(name, NULL);
         
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
         PopulateRelComboBoxes();
         PopulateSourceComboEntry();
         break;
      case NEW_SWAL:
         if (!etk_string_length_get(Cur.group))
         {
            ShowAlert("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New swallow");
         i = 2;
         while (edje_edit_part_exist(edje_o, name))
            snprintf(name, sizeof(name), "New swallow %d", i++);
         
         if (!edje_edit_part_add(edje_o, name, EDJE_PART_TYPE_SWALLOW))
         {
            ShowAlert("Can't create part.");
            break;
         }
         row = AddPartToTree(name, NULL);
         etk_tree_row_select(row);
         etk_tree_row_unfold(row);
         PopulateRelComboBoxes();
         PopulateSourceComboEntry();
         break;
      case NEW_GROUPSWAL:
         if (!etk_string_length_get(Cur.group))
         {
            ShowAlert("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New group swallow");
         i = 2;
         while (edje_edit_part_exist(edje_o, name))
            snprintf(name, sizeof(name), "New group swallow %d", i++);
         
         if (!edje_edit_part_add(edje_o, name, EDJE_PART_TYPE_GROUP))
         {
            ShowAlert("Can't create part.");
            break;
         }
         row = AddPartToTree(name, NULL);
         etk_tree_row_select(row);
         etk_tree_row_unfold(row);
         PopulateRelComboBoxes();
         PopulateSourceComboEntry();

         break;
      case NEW_DESC:
         if (!etk_string_length_get(Cur.part))
         {
            ShowAlert("You must first select a part.");
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
         
         edje_edit_state_rel1_relative_x_set(edje_o,Cur.part->string,name2,0.1);
         edje_edit_state_rel1_relative_y_set(edje_o,Cur.part->string,name2,0.1);
         edje_edit_state_rel2_relative_x_set(edje_o,Cur.part->string,name2,0.9);
         edje_edit_state_rel2_relative_y_set(edje_o,Cur.part->string,name2,0.9);
         edje_edit_state_text_size_set(edje_o, Cur.part->string, name2, 16);
      
         //Add state to tree
         row = AddStateToTree(Cur.part->string, name2);
         etk_tree_row_select(row);
         etk_tree_row_unfold(evas_hash_find(Parts_Hash,Cur.part->string));
         break;
      
      case NEW_PROG:
         if (!etk_string_length_get(Cur.group))
         {
            ShowAlert("You must first select a group.");
            break;
         }
         //generate a unique new name
         snprintf(name, sizeof(name), "New program");
         i = 2;
         while (edje_edit_program_exist(edje_o, name))
            snprintf(name, sizeof(name), "New program %d", i++);
      
         if (!edje_edit_program_add(edje_o, name))
         {
            ShowAlert("ERROR: can't add program");
            break;
         }
         row = AddProgramToTree(name);
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
            PopulateGroupsComboBox();
            etk_combobox_active_item_set(ETK_COMBOBOX(UI_GroupsComboBox),
               etk_combobox_last_item_get(ETK_COMBOBOX(UI_GroupsComboBox)));
         }
         else
         {
            ShowAlert("Can't create group.");
         }
         break;
   }
   ev_redraw();
   return ETK_TRUE;
}

Etk_Bool
on_RemoveMenu_item_activated(Etk_Object *object, void *data)
{
   Etk_Tree_Row *row, *next;
   printf("Item Activated Signal on RemoveMenu EMITTED\n");

   switch ((int)data)
   {
      case REMOVE_DESCRIPTION:
         if (!etk_string_length_get(Cur.state))
         {
            ShowAlert("No part state selected");
            break;
         }
         if (!strcmp(Cur.state->string,"default 0.00"))
         {
            ShowAlert("You can't remove default 0.0");
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
            ShowAlert("No part selected");
            break;
         }
         if (!edje_edit_part_del(edje_o, Cur.part->string))
         {
            ShowAlert("Can't delete part");
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
         
         PopulateRelComboBoxes();
         PopulateSourceComboEntry();
         //ev_redraw();
         break;
      
      case REMOVE_GROUP:
         if (!edje_edit_group_del(edje_o))
         {
            ShowAlert("Can't delete group");
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
            ShowAlert("You must first select a program");
         }
         if (!edje_edit_program_del(edje_o, Cur.prog->string))
         {
            ShowAlert("Can't delete program");
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

/* Dialogs Callbacks */
Etk_Bool
on_FileChooserDialog_response(Etk_Dialog *dialog, int response_id, void *data)
{
   char cmd[4096];

   printf("Response Signal on Filechooser EMITTED\n");

   if (response_id == ETK_RESPONSE_OK){

      switch(FileChooserOperation){
         case FILECHOOSER_OPEN:
            snprintf(cmd,4096,"%s/%s",
            etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
            etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            LoadEDJ(cmd);
         break;
         case FILECHOOSER_SAVE_EDJ:
            printf("SAVE EDJ\n");
            snprintf(cmd,4096,"%s/%s",
               etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
               etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            edje_edit_save(edje_o);
            if(!ecore_file_cp(Cur.edj_temp_name->string, cmd))
            {
               ShowAlert("<b>ERROR:<\b><br>Can't write file");
            }
            else
            {
               Cur.edj_file_name = etk_string_set(Cur.edj_file_name, cmd);
               ecore_evas_title_set(UI_ecore_MainWin, cmd);
            }
         break;
         case FILECHOOSER_SAVE_EDC:
              ShowAlert("Not yet implemented.");
         break;
         case FILECHOOSER_IMAGE:
            snprintf(cmd, 4096, "%s/%s", 
               etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
               etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            if (!edje_edit_image_add(edje_o, cmd))
            {
               ShowAlert("ERROR: Can't import image file.");
               break;
            }
            PopulateImageBrowser();
            
            Etk_Range *range;
            double upper;
            range = etk_scrolled_view_vscrollbar_get(
                    etk_iconbox_scrolled_view_get (UI_ImageBrowserIconbox));
            etk_range_range_get(range, NULL, &upper);
            etk_range_value_set(range, upper);
            
            etk_iconbox_icon_select(etk_iconbox_icon_get_by_label(
                                    UI_ImageBrowserIconbox,
            etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser))));
            
            break;
         case FILECHOOSER_FONT:
            snprintf(cmd, 4096, "%s/%s", 
               etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
               etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            if (!edje_edit_font_add(edje_o, cmd))
            {
               ShowAlert("ERROR: Can't import font file.");
               break;
            }
            PopulateFontsComboBox();
            etk_combobox_active_item_set(ETK_COMBOBOX(UI_FontComboBox),
                  etk_combobox_last_item_get(ETK_COMBOBOX(UI_FontComboBox)));
         break;
      }
      etk_widget_hide(ETK_WIDGET(dialog));
   }
   else{
      etk_widget_hide(ETK_WIDGET(dialog));
   }

   return ETK_TRUE;
}

Etk_Bool
on_FileChooser_selected(Etk_Filechooser_Widget *filechooser)
{
   printf("*** FILECHOOSER SELECTD ON *** \n");
   on_FileChooserDialog_response(ETK_DIALOG(UI_FileChooserDialog), ETK_RESPONSE_OK, NULL);
   return ETK_TRUE;
}

Etk_Bool
on_AlertDialog_response(Etk_Dialog *dialog, int response_id, void *data)
{
   etk_widget_hide(ETK_WIDGET(dialog));
   return ETK_TRUE;
}

Etk_Bool
on_NamesEntry_text_changed(Etk_Object *object, void *data)
{
   //printf("Text Changed Signal on one of the Names Entry Emitted\n");
   etk_widget_show(ETK_WIDGET(ETK_ENTRY(object)->secondary_image));
   return ETK_TRUE;
}
