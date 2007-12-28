#include <string.h>
#include <Edje.h>
#include <Etk.h>
#include <Ecore_Evas.h>
#include "callbacks.h"
#include "interface.h"
#include "inout.h"
#include "main.h"
#include "evas.h"

#if TEST_DIRECT_EDJE
   #include <Edje_Edit.h>
#endif

extern void PROTO_engrave_part_state_remove(Engrave_Part *ep, Engrave_Part_State *eps);
extern void PROTO_engrave_group_part_remove(Engrave_Group *eg, Engrave_Part *ep);
extern void PROTO_engrave_file_group_remove(Engrave_File *ef, Engrave_Group *eg);
extern void PROTO_engrave_part_state_image_tween_remove_nth(Engrave_Part_State *eps,int tween_num);
extern void PROTO_engrave_part_state_image_tween_remove_all(Engrave_Part_State *eps);
extern int PROTO_engrave_part_raise(Engrave_Part *ep);
extern int PROTO_engrave_part_lower(Engrave_Part *ep);
extern void PROTO_engrave_group_program_remove(Engrave_Group *eg, Engrave_Program *epr);

int current_color_object;

/* Called when the window is destroyed */
void
ecore_delete_cb(Ecore_Evas *ee)
{
   etk_main_quit();
}

/* Catch all the signal from the editing edje object */
void
signal_cb(void *data, Evas_Object *o, const char *sig, const char *src)
{
   printf("CALLBACK for \"%s\" \"%s\"\n", sig, src);
}

/* All the buttons Callback */
Etk_Bool
on_AllButton_click(Etk_Button *button, void *data)
{
   char cmd[1024];
   Etk_Tree_Row *sel_row;
   int row_num;
   Etk_String *text;

   switch ((int)data)
   {
      case TOOLBAR_NEW:
         system("edje_editor &");
         break;
      case TOOLBAR_OPEN:
         //ShowAlert("Not yet implemented");
         ShowFilechooser(FILECHOOSER_OPEN);
         break;
#if TEST_DIRECT_EDJE
      case TOOLBAR_SAVE:
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
         ShowAlert("Not yet reimplemented ;)");
         break;
      case TOOLBAR_REMOVE:
         ShowAlert("Not yet reimplemented ;)");
         break;
      case TOOLBAR_MOVE_UP: //Lower
         ShowAlert("Not yet reimplemented ;)");
         break;
      case TOOLBAR_MOVE_DOWN: //Raise
         ShowAlert("Not yet reimplemented ;)");
         break;
      case TOOLBAR_IMAGE_FILE_ADD:
         ShowFilechooser(FILECHOOSER_IMAGE);
         break;
      case TOOLBAR_FONT_FILE_ADD:
         ShowAlert("Not yet reimplemented ;)");
         break;
      case IMAGE_TWEEN_DELETE:
         ShowAlert("Not yet reimplemented ;)");
         break;
#else
      case TOOLBAR_SAVE:
         if (Cur.open_file_name)
            SaveEDJ(Cur.open_file_name);
         else
            ShowFilechooser(FILECHOOSER_SAVE_EDJ);
         break;
      case TOOLBAR_SAVE_EDC:
         ShowFilechooser(FILECHOOSER_SAVE_EDC);
         break;
      case TOOLBAR_SAVE_EDJ:
         ShowFilechooser(FILECHOOSER_SAVE_EDJ);
         break;
      case TOOLBAR_ADD:
         etk_menu_popup(ETK_MENU(UI_AddMenu));
         //etk_menu_popup_at_xy (ETK_MENU(AddMenu), 10, 10);
         break;
      case TOOLBAR_REMOVE:
         etk_menu_popup(ETK_MENU(UI_RemoveMenu));
         //etk_menu_popup_at_xy (ETK_MENU(AddMenu), 10, 10);
         break;
      case TOOLBAR_MOVE_UP: //Lower
         if (Cur.ep){
            Evas_List *l;
            Engrave_Part_State *eps;
            printf("Lower %s\n",Cur.ep->name);
            
            PROTO_engrave_part_lower(Cur.ep);
            
            l = evas_list_find_list(Cur.eg->parts, Cur.ep);
            if (!l) return ETK_TRUE;
            
            //delete part row with all childrens
            etk_tree_row_delete(ecore_hash_get(hash,Cur.ep));
            ecore_hash_remove(hash,Cur.ep);
            
            //re-add the row in correct place
            if (l->prev)
               AddPartToTree(Cur.ep, TRUE, l->prev->data); 
            else
               AddPartToTree(Cur.ep, TRUE, NULL);
            
            //re-add all childrens
            l = Cur.ep->states;
            while (l)
            {
               AddStateToTree(l->data);
               l = l->next;
            }
            etk_tree_row_select (ecore_hash_get(hash,Cur.ep));
         }
         else
            ShowAlert("You must choose a part to lower");
         break;
      case TOOLBAR_MOVE_DOWN: //Raise
         if (Cur.ep){
            Evas_List *l;
            Engrave_Part_State *eps;
            printf("Raise %s\n",Cur.ep->name);
            
            PROTO_engrave_part_raise(Cur.ep);
            
            l = evas_list_find_list(Cur.eg->parts, Cur.ep);
            if (!l) return ETK_TRUE;
            
            //delete part row with all childrens
            etk_tree_row_delete(ecore_hash_get(hash,Cur.ep));
            ecore_hash_remove(hash,Cur.ep);
             
            //re-add the row in correct place
            if (l->prev)
               AddPartToTree(Cur.ep, TRUE, l->prev->data); 
            else
               AddPartToTree(Cur.ep, TRUE, NULL);
            
            //re-add all childrens
            l = Cur.ep->states;
            while (l)
            {
               AddStateToTree(l->data);
               l = l->next;
            }
            etk_tree_row_select (ecore_hash_get(hash,Cur.ep));
         }
         else
            ShowAlert("You must choose a part to raise");
         break;
      case IMAGE_TWEEN_RADIO:
            UpdateImageFrame();
            etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(UI_ImageTweenRadio), TRUE);
            etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(UI_ImageNormalRadio), FALSE);
            etk_widget_show(UI_ImageTweenList);
            etk_widget_show(UI_MoveUpTweenButton);
            etk_widget_show(UI_MoveDownTweenButton);
            etk_widget_show(UI_DeleteTweenButton);
            etk_combobox_active_item_set (ETK_COMBOBOX(UI_ImageComboBox),
               etk_combobox_nth_item_get(ETK_COMBOBOX(UI_ImageComboBox),0));
         break;
      case IMAGE_NORMAL_RADIO:
            PROTO_engrave_part_state_image_tween_remove_all(Cur.eps);
            UpdateImageFrame();
         break;
      case TOOLBAR_IMAGE_FILE_ADD:
         if (engrave_file_image_dir_get(Cur.ef)) ShowFilechooser(FILECHOOSER_IMAGE);
         else ShowAlert("You have to save the file once for insert image.");
         break;
      case TOOLBAR_FONT_FILE_ADD:
         if (engrave_file_font_dir_get(Cur.ef)) ShowFilechooser(FILECHOOSER_FONT);
         else ShowAlert("You have to save the file once for insert font.");
         break;
      case IMAGE_TWEEN_DELETE:
            sel_row = etk_tree_selected_row_get(ETK_TREE(UI_ImageTweenList));
            if ((row_num = (int)etk_tree_row_data_get (sel_row)))
            {
               PROTO_engrave_part_state_image_tween_remove_nth(Cur.eps,row_num-1);
               UpdateImageFrame();
            }
         break;
#endif
      case TOOLBAR_OPTIONS:
         etk_menu_popup(ETK_MENU(UI_OptionsMenu));
         //etk_menu_popup_at_xy (ETK_MENU(AddMenu), 10, 10);
         break;
      case TOOLBAR_OPTION_BG1:
         printf("SET_BG1\n");
         edje_object_signal_emit(edje_ui,"set_bg1","edje_editor");
         break;
       case TOOLBAR_OPTION_BG2:
         printf("SET_BG2\n");
         edje_object_signal_emit(edje_ui,"set_bg2","edje_editor");
         break;
      case TOOLBAR_OPTION_BG3:
         printf("SET_BG3\n");
         edje_object_signal_emit(edje_ui,"set_bg3","edje_editor");
         break;
      case TOOLBAR_OPTION_BG4:
         printf("SET_BG4\n");
         edje_object_signal_emit(edje_ui,"set_bg4","edje_editor");
         break;
      case TOOLBAR_PLAY:
         printf("Clicked signal on Toolbar Button 'Play' EMITTED\n");
         if (!Cur.eg)
            ShowAlert("You must select a group to test.");
         else if (!Cur.open_file_name) 
            ShowAlert("You need to save the file before testing it.");
         else
         {
            snprintf(cmd,1024,"edje_editor -t \"%s\" \"%s\" &",Cur.open_file_name,Cur.eg->name);
            printf("TESTING EDJE. cmd: %s\n",cmd);
            system(cmd);
         }
         break;
      case TOOLBAR_DEBUG:
         DebugInfo(FALSE);
         break;
      case IMAGE_TWEEN_UP:
            ShowAlert("Up not yet implemented.");
         break;
      case IMAGE_TWEEN_DOWN:
            ShowAlert("Down not yet implemented.");
         break;
      case SAVE_SCRIPT:
            text = etk_textblock_text_get(ETK_TEXT_VIEW(UI_ScriptBox)->textblock,ETK_TRUE);
            if (Cur.epr)
            {
               printf("Save script (in prog %s): %s\n",Cur.epr->name,text->string);
               engrave_program_script_set (Cur.epr, text->string);
               
            }else if (Cur.eg)
            {
               printf("Save script (in group %s): %s\n",Cur.eg->name,text->string);
               engrave_group_script_set(Cur.eg, text->string);
            }
            etk_object_destroy(ETK_OBJECT(text));
         break;
      default:
         break;
   }

   return ETK_TRUE;
}

/* Tree callbacks */
#if TEST_DIRECT_EDJE
Etk_Bool
on_PartsTree_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   int row_type=0;
   char *name;
   char *parent_name;
   int part_type = 0;
   Engrave_Group* old_group = Cur.eg;

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
         
         edje_object_signal_emit(edje_ui,"description_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"position_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"rect_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"image_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"text_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"group_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"program_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"part_frame_show","edje_editor");
         edje_object_signal_emit(edje_ui,"script_frame_hide","edje_editor");
         
         UpdatePartFrame();
         break;

      case ROW_DESC:
         Cur.state = etk_string_set(Cur.state, name);
         Cur.part = etk_string_set(Cur.part, parent_name);
       
         edje_edit_part_selected_state_set(edje_o, Cur.part->string, Cur.state->string);  
       
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
         edje_object_signal_emit(edje_ui,"group_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"program_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"script_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"description_frame_show","edje_editor");
         edje_object_signal_emit(edje_ui,"position_frame_show","edje_editor");
         break;
      
      case ROW_PROG:
         Cur.prog = etk_string_set(Cur.prog, name);
         Cur.part = etk_string_clear(Cur.part);
         Cur.state = etk_string_clear(Cur.state);
       
         edje_object_signal_emit(edje_ui,"description_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"position_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"rect_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"image_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"text_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"group_frame_hide","edje_editor");
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

#else
Etk_Bool
on_PartsTree_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   int row_type=0;
   char *part;
   char *p;
   Engrave_Group* old_group = Cur.eg;

   printf("Row Selected Signal on one of the Tree EMITTED \n");

   //get the type of the row (group,part,desc or prog) from the hidden col
   etk_tree_row_fields_get(row,
      COL_TYPE, &row_type,
      NULL);

   switch (row_type)
   {
      case ROW_GROUP:
         Cur.epr = NULL;
         Cur.eg = etk_tree_row_data_get (row);
         Cur.ep = NULL;
         Cur.eps = NULL;
         //Hide
         edje_object_signal_emit(edje_ui,"description_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"position_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"rect_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"image_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"text_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"part_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"program_frame_hide","edje_editor");
         //Show
         edje_object_signal_emit(edje_ui,"group_frame_show","edje_editor");
         edje_object_signal_emit(edje_ui,"script_frame_show","edje_editor");
         
         UpdateScriptFrame();
         break;
      case ROW_PART:
         Cur.epr = NULL;
         Cur.ep = etk_tree_row_data_get (row);
         Cur.eg = Cur.ep->parent;
         Cur.eps = NULL;
         
         edje_object_signal_emit(edje_ui,"description_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"position_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"rect_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"image_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"text_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"group_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"program_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"part_frame_show","edje_editor");
         edje_object_signal_emit(edje_ui,"script_frame_hide","edje_editor");
         
         UpdatePartFrame();

         break;
      case ROW_DESC:
         Cur.epr = NULL;
         Cur.eps = etk_tree_row_data_get (row);
         Cur.ep = Cur.eps->parent;
         Cur.eg = Cur.ep->parent;
         Cur.ep->current_state = Cur.eps;

         UpdateDescriptionFrame();
         UpdatePositionFrame();
         UpdateComboPositionFrame();

         if (Cur.ep->type == ENGRAVE_PART_TYPE_TEXT)
         {
            UpdateTextFrame();
            edje_object_signal_emit(edje_ui,"text_frame_show","edje_editor");
         }else
         {
            edje_object_signal_emit(edje_ui,"text_frame_hide","edje_editor");
         }

         if (Cur.ep->type == ENGRAVE_PART_TYPE_IMAGE)
         {
            UpdateImageFrame();
            edje_object_signal_emit(edje_ui,"image_frame_show","edje_editor");
         }else
         {
            edje_object_signal_emit(edje_ui,"image_frame_hide","edje_editor");
         }

         if (Cur.ep->type == ENGRAVE_PART_TYPE_RECT)
         {
            UpdateRectFrame();
            edje_object_signal_emit(edje_ui,"rect_frame_show","edje_editor");
         }else
         {
            edje_object_signal_emit(edje_ui,"rect_frame_hide","edje_editor");
         }

         edje_object_signal_emit(edje_ui,"part_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"group_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"program_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"script_frame_hide","edje_editor");
         
         edje_object_signal_emit(edje_ui,"description_frame_show","edje_editor");
         edje_object_signal_emit(edje_ui,"position_frame_show","edje_editor");

         break;
      case ROW_PROG:
         Cur.epr = etk_tree_row_data_get (row);
         Cur.eg = Cur.epr->parent;
         Cur.ep = NULL;
         Cur.eps = NULL;
       
         edje_object_signal_emit(edje_ui,"description_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"position_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"rect_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"image_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"text_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"group_frame_hide","edje_editor");
         edje_object_signal_emit(edje_ui,"part_frame_hide","edje_editor");
         
         edje_object_signal_emit(edje_ui,"program_frame_show","edje_editor");
         edje_object_signal_emit(edje_ui,"script_frame_show","edje_editor");
         
         UpdateScriptFrame();
         UpdateProgFrame();
         PopulateSourceComboBox();
         break;
   }

   //The group as changed
   if (Cur.eg != old_group){
      int w, h;

      UpdateGroupFrame();
      PopulateRelComboBoxes();

      //Update Fakewin
      engrave_group_max_size_get(Cur.eg,&w,&h);
      ev_resize_fake(w,h);
      edje_object_part_text_set (EV_fakewin, "title", Cur.eg->name);

      engrave_canvas_current_group_set (engrave_canvas, Cur.eg);
   }
   ev_redraw();
   return ETK_TRUE;
}
#endif
/* Group frame callbacks */
Etk_Bool
on_GroupNameEntry_text_changed(Etk_Object *object, void *data)
{
   Etk_Tree_Col *col1=NULL;

   printf("Text Changed Signal on PartNameEntry EMITTED (text: %s)\n",etk_entry_text_get(ETK_ENTRY(object)));
   if (Cur.eg && ecore_hash_get(hash,Cur.eg))
   {
      engrave_group_name_set(Cur.eg,etk_entry_text_get(ETK_ENTRY(object)));

      //Update PartsTree
      if ((col1 = etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), 0)))
         etk_tree_row_fields_set(ecore_hash_get(hash,Cur.eg),TRUE,
            col1,EdjeFile,"NONE.PNG",engrave_group_name_get(Cur.eg),
            NULL);

      //update FakeWin title
      edje_object_part_text_set (EV_fakewin,
         "title", engrave_group_name_get(Cur.eg));
   }

   return ETK_TRUE;
}

Etk_Bool
on_GroupSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Group Spinners value changed signal EMIT\n");
   if (Cur.eg)
   {
      switch ((int)data)
      {
         case MINW_SPINNER:
            printf("MINW\n");
            engrave_group_min_size_set(Cur.eg,
                  value,
                  etk_range_value_get(ETK_RANGE(UI_GroupMinHSpinner)));
            break;
         case MINH_SPINNER:
            engrave_group_min_size_set(Cur.eg,
                  etk_range_value_get(ETK_RANGE(UI_GroupMinWSpinner)),
                  value);
            break;
         case MAXW_SPINNER:
            engrave_group_max_size_set(Cur.eg,
                  value,
                  etk_range_value_get(ETK_RANGE(UI_GroupMaxHSpinner)));
            break;
         case MAXH_SPINNER:
            engrave_group_max_size_set(Cur.eg,
                  etk_range_value_get(ETK_RANGE(UI_GroupMaxWSpinner)),
                  value);
            break;
      }
   }

   return ETK_TRUE;
}

/* Parts & Descriptions Callbacks*/
Etk_Bool
on_PartNameEntry_text_changed(Etk_Object *object, void *data)
{
   Etk_Tree_Col *col1=NULL;
   Etk_Tree_Row *row;
   char *text;
   //printf("Text Changed Signal on PartNameEntry EMITTED (text: %s)\n",etk_entry_text_get(ETK_ENTRY(object)));
#if TEST_DIRECT_EDJE
   if (etk_string_length_get(Cur.part->string))
   {
      //Update PartTree
      row = etk_tree_selected_row_get(ETK_TREE(UI_PartsTree));
      text = etk_entry_text_get(ETK_ENTRY(object));
      
      printf("** TYPE: %d\n", edje_edit_part_type_get(edje_o, Cur.part->string));
      switch (edje_edit_part_type_get(edje_o, Cur.part->string))
      {
         case EDJE_PART_TYPE_IMAGE:
            etk_tree_row_fields_set(row,TRUE,
                                    COL_NAME, EdjeFile, "IMAGE.PNG", text,
                                    NULL); 
            break;
         case EDJE_PART_TYPE_RECTANGLE:
            etk_tree_row_fields_set(row,TRUE,
                                    COL_NAME, EdjeFile, "RECT.PNG", text,
                                    NULL); 
            break;
         case EDJE_PART_TYPE_TEXT:
            etk_tree_row_fields_set(row,TRUE,
                                    COL_NAME, EdjeFile, "TEXT.PNG", text,
                                    NULL); 
            break;
         default:
            etk_tree_row_fields_set(row,TRUE,
                                    COL_NAME, EdjeFile, "NONE.PNG", text,
                                    NULL);
            break;
      }
      
      /* Update hidden colon on every child */
      Etk_Tree_Row *child;
      child = etk_tree_row_first_child_get(row);
      etk_tree_row_fields_set(child, TRUE, COL_PARENT, text, NULL);
      while (child = etk_tree_row_next_get(child))
         etk_tree_row_fields_set(child, TRUE, COL_PARENT, text, NULL);
       
      /* Update Parts_Hash */
      Parts_Hash = evas_hash_del(Parts_Hash, Cur.part->string, NULL);
      Parts_Hash = evas_hash_add(Parts_Hash, text, row);
       
      /* change the name in edje */
      edje_edit_part_name_set(edje_o, Cur.part->string, text);
      
      /* Set new Current name */
      Cur.part = etk_string_set(Cur.part, text);
      
      /* Recreate rel combobox */
      PopulateRelComboBoxes();  //TODO do a focus-out callback for this (don't need to do on every key!!)
   }
#else
   if (Cur.ep)
   {
      engrave_part_name_set(Cur.ep,etk_entry_text_get(ETK_ENTRY(object)));
      //RenamePart(Cur.ep,etk_entry_text_get(ETK_ENTRY(object)));
      PopulateRelComboBoxes();
      //Update PartTree
      col1 = etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), 0);
      switch (Cur.ep->type)
      {
         case ENGRAVE_PART_TYPE_IMAGE: 
            etk_tree_row_fields_set(ecore_hash_get(hash,Cur.ep),TRUE,
               col1,EdjeFile,"IMAGE.PNG",etk_entry_text_get(ETK_ENTRY(object)),
               NULL); 
            break;
         case ENGRAVE_PART_TYPE_RECT:
            etk_tree_row_fields_set(ecore_hash_get(hash,Cur.ep),TRUE,
               col1,EdjeFile,"RECT.PNG",etk_entry_text_get(ETK_ENTRY(object)),
               NULL); 
            break;
         case ENGRAVE_PART_TYPE_TEXT:
            etk_tree_row_fields_set(ecore_hash_get(hash,Cur.ep),TRUE,
               col1,EdjeFile,"TEXT.PNG",etk_entry_text_get(ETK_ENTRY(object)),
               NULL); 
               break;
         default:
            etk_tree_row_fields_set(ecore_hash_get(hash,Cur.ep),TRUE,
               col1,EdjeFile,"NONE.PNG",etk_entry_text_get(ETK_ENTRY(object)),
               NULL); 
               break;
      }
   }
#endif
   return ETK_TRUE;
}

Etk_Bool
on_PartEventsCheck_toggled(Etk_Object *object, void *data)
{
   printf("Toggled Signal on EventsCheck EMITTED\n");
#if TEST_DIRECT_EDJE
   if (etk_string_length_get(Cur.part))
   {
      edje_edit_part_mouse_events_set(edje_o, Cur.part->string,
                     etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(object)));
   }
#else
   if (Cur.ep)
   {
      engrave_part_mouse_events_set(Cur.ep,
                     etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(object)));
   }
#endif
   return ETK_TRUE;
}

Etk_Bool
on_PartEventsRepeatCheck_toggled(Etk_Object *object, void *data)
{
   printf("Toggled Signal on EventsRepeatCheck EMITTED\n");
#if TEST_DIRECT_EDJE
   if (etk_string_length_get(Cur.part))
   {
      edje_edit_part_repeat_events_set(edje_o, Cur.part->string,
                     etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(object)));
   }
#endif
   return ETK_TRUE;
}

Etk_Bool
on_StateEntry_text_changed(Etk_Object *object, void *data)
{
   Etk_Tree_Col *col1=NULL;
   char buf[4096];
   const char *nn;   //new name
   printf("Text Changed Signal on StateEntry EMITTED\n");

   if (Cur.eps)
   {
      printf("FOLLOW %s %f\n",Cur.eps->name,Cur.eps->value);
      if ((strcmp("default", Cur.eps->name)) || Cur.eps->value)
      {
         if ((nn = etk_entry_text_get(ETK_ENTRY(object))))
            engrave_part_state_name_set(Cur.eps,nn,Cur.eps->value);
      }else
      {
         ShowAlert("You can't rename default 0.0");
      }

      //Update PartTree
      col1 = etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), 0);
      snprintf(buf,4095,"%s %.2f",Cur.eps->name,Cur.eps->value);
      etk_tree_row_fields_set(ecore_hash_get(hash,Cur.eps),TRUE,
         col1,EdjeFile,"DESC.PNG",buf,NULL);
   }

   return ETK_TRUE;
}

Etk_Bool
on_StateIndexSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   char buf[4096];
   Etk_Tree_Col *col1=NULL;

   printf("Value Changed Signal on StateIndexSpinner EMITTED\n");
   if (Cur.eps)
   {
      snprintf(buf,4096,"%s",engrave_part_state_name_get(Cur.eps,NULL));
      //RenameDescription(selected_desc,NULL,etk_range_value_get(range));
      if ((strcmp("default", buf)) || Cur.eps->value)
      {
         engrave_part_state_name_set(Cur.eps,buf,etk_range_value_get(range));
      }else
      {
         ShowAlert("You can't rename default 0.0");
      }
      //Update PartTree
      col1 = etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), 0);
      snprintf(buf,4095,"%s %.2f",Cur.eps->name,Cur.eps->value);
      etk_tree_row_fields_set(ecore_hash_get(hash,Cur.eps),TRUE,
         col1,EdjeFile,"DESC.PNG",buf,NULL);
   }

   return ETK_TRUE;
}

Etk_Bool
on_AspectSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on AspectMinSpinner EMITTED\n");
   engrave_part_state_aspect_set(Cur.eps,
      etk_range_value_get(ETK_RANGE(UI_AspectMinSpinner)),
      etk_range_value_get(ETK_RANGE(UI_AspectMaxSpinner)));
   return ETK_TRUE;
}

Etk_Bool
on_AspectComboBox_changed(Etk_Combobox *combobox, void *data)
{
   Engrave_Aspect_Preference prefer;
   printf("Active Item Changed Signal on AspectComboBox EMITTED\n");

   prefer = (Engrave_Aspect_Preference)etk_combobox_item_data_get(etk_combobox_active_item_get (combobox));
   engrave_part_state_aspect_preference_set(Cur.eps,prefer);

   return ETK_TRUE;
}

Etk_Bool
on_StateMinMaxSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Active Item Changed Signal on MinMaxSpinners EMITTED\n");

   engrave_part_state_min_size_set(Cur.eps,
      etk_range_value_get(ETK_RANGE(UI_StateMinWSpinner)),
      etk_range_value_get(ETK_RANGE(UI_StateMinHSpinner)));

   engrave_part_state_max_size_set(Cur.eps,
      etk_range_value_get(ETK_RANGE(UI_StateMaxWSpinner)),
      etk_range_value_get(ETK_RANGE(UI_StateMaxHSpinner)));

   ev_redraw();
   return ETK_TRUE;
}

/* Image Frame Callbacks */
Etk_Bool
on_ImageComboBox_changed(Etk_Combobox *combobox, void *data)
{
   Engrave_Image *image;
   printf("Changed signal on Image Combo EMITTED\n");
   
#if TEST_DIRECT_EDJE
   char *im;
   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   
   im = etk_combobox_item_field_get(etk_combobox_active_item_get (combobox), 1);
   edje_edit_state_image_set(edje_o, Cur.part->string, Cur.state->string, im);
#else
   if ((image = etk_combobox_item_data_get(etk_combobox_active_item_get (combobox)))){
      //Set an existing image
      if (Cur.eps){
         if (!etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(UI_ImageTweenRadio)))
         {
            engrave_part_state_image_normal_set(Cur.eps, image);
         }else{
            engrave_part_state_image_tween_add(Cur.eps,image);
            UpdateImageFrame();
         }
         ev_redraw();
      }
   }
#endif

   return ETK_TRUE;
}

Etk_Bool
on_ImageTweenList_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
#if TEST_DIRECT_EDJE
#else
   printf("Row selected signal on ImageTweenList EMITTED\n");
   if (row != etk_tree_first_row_get (ETK_TREE(UI_ImageTweenList)))
   {
      etk_widget_disabled_set(UI_DeleteTweenButton,FALSE);
      etk_widget_disabled_set(UI_MoveUpTweenButton,FALSE);
      etk_widget_disabled_set(UI_MoveDownTweenButton,FALSE);
   }else{
      etk_widget_disabled_set(UI_DeleteTweenButton,TRUE);
      etk_widget_disabled_set(UI_MoveUpTweenButton,TRUE);
      etk_widget_disabled_set(UI_MoveDownTweenButton,TRUE);
   }
#endif
   return ETK_TRUE;
}

Etk_Bool
on_ImageAlphaSlider_value_changed(Etk_Object *object, double va, void *data)
{
   printf("ImageSlieder value_changed signale EMIT: %.2f\n",va);
#if TEST_DIRECT_EDJE
   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   edje_edit_state_color_set(edje_o, Cur.part->string, Cur.state->string,
                             -1, -1, -1, (int)va);
   ev_redraw();
#else
   if (Cur.eps){
      engrave_part_state_color_set(Cur.eps, (int)va, (int)va, (int)va, (int)va);
      ev_redraw();
   }
#endif

   return ETK_TRUE;
}

Etk_Bool
on_BorderSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed signal on BorderSpinner EMITTED (value: %f)\n",etk_range_value_get(range));
   
#if TEST_DIRECT_EDJE
   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   edje_edit_state_image_border_set(edje_o, Cur.part->string, Cur.state->string,
      (int)etk_range_value_get(ETK_RANGE(UI_BorderLeftSpinner)),
      (int)etk_range_value_get(ETK_RANGE(UI_BorderRightSpinner)),
      (int)etk_range_value_get(ETK_RANGE(UI_BorderTopSpinner)),
      (int)etk_range_value_get(ETK_RANGE(UI_BorderBottomSpinner)));

   ev_redraw();
#else
   if (Cur.eps){
      engrave_part_state_image_border_set(Cur.eps,
         (int)etk_range_value_get(ETK_RANGE(UI_BorderLeftSpinner)),
         (int)etk_range_value_get(ETK_RANGE(UI_BorderRightSpinner)),
         (int)etk_range_value_get(ETK_RANGE(UI_BorderTopSpinner)),
         (int)etk_range_value_get(ETK_RANGE(UI_BorderBottomSpinner)));

      printf("TODO: s: %s  [%d] %d\n",Cur.eps->name,Cur.eps->image.border.l,(int)etk_range_value_get(ETK_RANGE(UI_BorderLeftSpinner)));

      ev_redraw();
   }
#endif
   return ETK_TRUE;
}

/* Position Frame Callbacks */
Etk_Bool
on_RelToComboBox_changed(Etk_Combobox *combobox, void *data)
{
#if TEST_DIRECT_EDJE
   char *parent;
   parent = etk_combobox_item_field_get(etk_combobox_active_item_get (combobox), 1);
   
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
         edje_edit_state_rel1_to_x_set(edje_o, Cur.part->string, Cur.state->string, parent);
         break;
      case REL1Y_SPINNER:
         edje_edit_state_rel1_to_y_set(edje_o, Cur.part->string, Cur.state->string, parent);
         break;
      case REL2X_SPINNER:
         edje_edit_state_rel2_to_x_set(edje_o, Cur.part->string, Cur.state->string, parent);
         break;
      case REL2Y_SPINNER:
        edje_edit_state_rel2_to_y_set(edje_o, Cur.part->string, Cur.state->string, parent);
         break;
   }

   edje_edit_part_selected_state_set(edje_o, Cur.part->string, Cur.state->string);  //this make edje redraw (need to update in lib)
   ev_redraw();
   return ETK_TRUE;
#else
   printf("RelTocomboBox changed signal EMITTED \n");
   Engrave_Part* part = NULL;

   part = etk_combobox_item_data_get (etk_combobox_active_item_get (combobox));
   if (part)
   {
      if (part == Cur.ep)
      {
         ShowAlert("A state can't rel to itself.");
         return ETK_TRUE;
      }
      switch ((int)data)
      {
         case REL1X_SPINNER:
            if ((int)part == REL_COMBO_INTERFACE)
               engrave_part_state_rel1_to_x_set(Cur.eps, NULL);
            else
               engrave_part_state_rel1_to_x_set(Cur.eps,part->name);
            break;
         case REL1Y_SPINNER:
            if ((int)part == REL_COMBO_INTERFACE)
               engrave_part_state_rel1_to_y_set(Cur.eps, NULL);
            else
               engrave_part_state_rel1_to_y_set(Cur.eps,part->name);
            break;
         case REL2X_SPINNER:
            if ((int)part == REL_COMBO_INTERFACE)
               engrave_part_state_rel2_to_x_set(Cur.eps, NULL);
            else
               engrave_part_state_rel2_to_x_set(Cur.eps,part->name);
            break;
         case REL2Y_SPINNER:
            if ((int)part == REL_COMBO_INTERFACE)
               engrave_part_state_rel2_to_y_set(Cur.eps, NULL);
            else
               engrave_part_state_rel2_to_y_set(Cur.eps,part->name);
            break;
      }

   }

   ev_redraw();
   return ETK_TRUE;
#endif
}

Etk_Bool
on_RelSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on RelSpinner EMITTED (value: %f)\n",etk_range_value_get(range));
#if TEST_DIRECT_EDJE
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
#else
   if (Cur.eps)
   {
      switch ((int)data)
      {
         case REL1X_SPINNER:
            Cur.eps->rel1.relative.x = etk_range_value_get(range);
            break;
         case REL1Y_SPINNER:
            Cur.eps->rel1.relative.y = etk_range_value_get(range);
            break;
         case REL2X_SPINNER:
            Cur.eps->rel2.relative.x = etk_range_value_get(range);
            break;
         case REL2Y_SPINNER:
            Cur.eps->rel2.relative.y = etk_range_value_get(range);
            break;
      }
      ev_redraw();
      //ev_draw_focus();
   }
#endif
   return ETK_TRUE;
}

Etk_Bool
on_RelOffsetSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on Offset Spinner EMITTED\n");
#if TEST_DIRECT_EDJE
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
#else
   if (Cur.eps)
   {
      switch ((int)data)
      {
         case REL1X_SPINNER:
            Cur.eps->rel1.offset.x = etk_range_value_get(range);
            break;
         case REL1Y_SPINNER:
            Cur.eps->rel1.offset.y = etk_range_value_get(range);
            break;
         case REL2X_SPINNER:
            Cur.eps->rel2.offset.x = etk_range_value_get(range);
            break;
         case REL2Y_SPINNER:
            Cur.eps->rel2.offset.y = etk_range_value_get(range);
            break;
      }
      ev_redraw();
      //ev_draw_focus();
   }
#endif
   return ETK_TRUE;
}

/* Text Frame Callbacks */
Etk_Bool
on_FontComboBox_changed(Etk_Combobox *combobox, void *data)
{
   printf("Changed Signal on FontComboBox EMITTED \n");
#if TEST_DIRECT_EDJE
   char *font;
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   
   font = etk_combobox_item_field_get(etk_combobox_active_item_get(combobox), 1);
   
   edje_edit_state_font_set(edje_o, Cur.part->string, Cur.state->string, font);
   
#else
   Engrave_Font *ef;
   if ((ef = etk_combobox_item_data_get(etk_combobox_active_item_get(combobox)))){
      //Set an existing font
      if (Cur.eps){
         printf("selected font: %s\n", engrave_font_name_get (ef));
         engrave_part_state_text_font_set(Cur.eps,engrave_font_name_get(ef));
         printf("changed font: %s\n", engrave_part_state_text_font_get(Cur.eps));
         ev_redraw();
      }
   }
#endif
   return ETK_TRUE;
}

Etk_Bool
on_EffectComboBox_changed(Etk_Combobox *combobox, void *data)
{
#if TEST_DIRECT_EDJE
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   
   edje_edit_part_effect_set(edje_o, Cur.part->string,
         etk_combobox_item_data_get(etk_combobox_active_item_get(combobox)));
   
   ev_redraw();
#else
   Engrave_Text_Effect effect;

   printf("Changed Signal on EffectComboBox EMITTED\n");
   if (Cur.ep)
   {
      if ((effect = (Engrave_Text_Effect)etk_combobox_item_data_get(etk_combobox_active_item_get (combobox))))
      {
         engrave_part_effect_set(Cur.ep,effect);
         ev_redraw();
      }
   }
#endif
   return ETK_TRUE;
}

Etk_Bool
on_FontSizeSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on FontSizeSpinner EMITTED (value: %d)\n",(int)etk_range_value_get(range));
#if TEST_DIRECT_EDJE
   edje_edit_state_text_size_set(edje_o, Cur.part->string, Cur.state->string,
                                 (int)etk_range_value_get(range));
#else
   engrave_part_state_text_size_set(Cur.eps,(int)etk_range_value_get(range));
#endif
   ev_redraw();
   return ETK_TRUE;
}

Etk_Bool
on_TextEntry_text_changed(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on TextEntry EMITTED (value %s)\n",etk_entry_text_get(ETK_ENTRY(object)));
#if TEST_DIRECT_EDJE
   edje_edit_state_text_set(edje_o, Cur.part->string, Cur.state->string,
                            etk_entry_text_get(ETK_ENTRY(object)));
#else
   engrave_part_state_text_text_set(Cur.eps,etk_entry_text_get(ETK_ENTRY(object)));
#endif
   ev_redraw();
   return ETK_TRUE;
}

Etk_Bool
on_FontAlignSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on AlignSpinner (h or v, text or part) EMITTED (value: %.2f)\n",etk_range_value_get(range));

#if TEST_DIRECT_EDJE
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   
   if (data == TEXT_ALIGNH_SPINNER)
      edje_edit_state_text_align_x_set(edje_o, Cur.part->string, Cur.state->string,
                                       (double)etk_range_value_get(range));
   if (data == TEXT_ALIGNV_SPINNER)
      edje_edit_state_text_align_y_set(edje_o, Cur.part->string, Cur.state->string,
                                       (double)etk_range_value_get(range));
#else
   if (data == TEXT_ALIGNH_SPINNER)
      engrave_part_state_text_align_set(Cur.eps, etk_range_value_get(range), -1);
   if (data == TEXT_ALIGNV_SPINNER)
      engrave_part_state_text_align_set(Cur.eps, -1, etk_range_value_get(range));
   if (data == STATE_ALIGNH_SPINNER)
      engrave_part_state_align_set(Cur.eps, etk_range_value_get(range), -1);
   if (data == STATE_ALIGNV_SPINNER)
      engrave_part_state_align_set(Cur.eps, -1, etk_range_value_get(range));
#endif
   return ETK_TRUE;
}

/* Programs Callbacks */
Etk_Bool
on_ActionComboBox_changed(Etk_Combobox *combobox, void *data)
{
   Engrave_Action action;
   char param1[200],param2[200];
   double value,value2;
   printf("Changed Signal on ActionComboBox EMITTED\n");

   if (!Cur.epr) return ETK_TRUE;

   //Get the current action in the current program
   engrave_program_action_get(Cur.epr,&action,&param1,&param2,200,200,&value,&value2);

   //Get the new action from the combo data
   action = (Engrave_Action)etk_combobox_item_data_get(
               etk_combobox_active_item_get (combobox));

   //set the action in the current program
   engrave_program_action_set(Cur.epr, action,
         param1,
         param2,
         value,
         value2
   );

   if (action == ENGRAVE_ACTION_SIGNAL_EMIT)
   {
      etk_widget_hide(UI_TargetEntry);
      etk_widget_hide(UI_TargetLabel);
      etk_widget_hide(UI_TransiComboBox);
      etk_widget_hide(UI_TransiLabel);
      etk_widget_hide(UI_DurationSpinner);
      etk_widget_hide(UI_DurationLabel);
      etk_widget_show(UI_Param1Entry);
      etk_widget_show(UI_Param1Label);
      etk_label_set(ETK_LABEL(UI_Param1Label), "<b>Signal</b>");
      etk_widget_hide(UI_Param1Spinner);
      etk_widget_show(UI_Param2Label);
      etk_widget_show(UI_Param2Entry);
   }
   if (action == ENGRAVE_ACTION_STATE_SET)
   {
      etk_widget_show(UI_TargetEntry);
      etk_widget_show(UI_TargetLabel);
      etk_widget_show(UI_TransiComboBox);
      etk_widget_show(UI_TransiLabel);
      etk_widget_show(UI_DurationSpinner);
      etk_widget_show(UI_DurationLabel);
      etk_widget_show(UI_Param1Entry);
      etk_widget_show(UI_Param1Label);
      etk_label_set(ETK_LABEL(UI_Param1Label), "<b>State</b>");
      etk_widget_show(UI_Param1Spinner);
      etk_widget_hide(UI_Param2Label);
      etk_widget_hide(UI_Param2Entry);
   }
   if (action == ENGRAVE_ACTION_STOP)
   {
      etk_widget_show(UI_TargetEntry);
      etk_widget_show(UI_TargetLabel);
      etk_widget_hide(UI_TransiComboBox);
      etk_widget_hide(UI_TransiLabel);
      etk_widget_hide(UI_DurationSpinner);
      etk_widget_hide(UI_DurationLabel);
      etk_widget_hide(UI_Param1Entry);
      etk_widget_hide(UI_Param1Label);
      etk_widget_hide(UI_Param1Spinner);
      etk_widget_hide(UI_Param2Label);
      etk_widget_hide(UI_Param2Entry);
   }

   return ETK_TRUE;
}

Etk_Bool
on_ProgramEntry_text_changed(Etk_Object *object, void *data)
{
   Etk_Tree_Col *col1=NULL;

   printf("Text Changed Signal on ProgramEntry Emitted\n");
   engrave_program_name_set(Cur.epr,etk_entry_text_get(ETK_ENTRY(UI_ProgramEntry)));
   if (Cur.epr && ecore_hash_get(hash,Cur.epr))
   {
      if ((col1 = etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), 0)))
         etk_tree_row_fields_set(ecore_hash_get(hash,Cur.epr),TRUE,
            col1,EdjeFile,"PROG.PNG",engrave_program_name_get(Cur.epr),
            NULL);
   }
   //TODO Check for dependencies! only in after?

   return ETK_TRUE;
}

Etk_Bool
on_SourceEntry_text_changed(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on SourceEntry Emitted\n");
   engrave_program_source_set(Cur.epr,
            etk_entry_text_get(ETK_ENTRY(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SourceEntry)))));

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

   etk_entry_text_set(ETK_ENTRY(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SourceEntry))),pname);

   return ETK_TRUE;
}

Etk_Bool
on_SignalEntry_text_changed(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on SignalEntry Emitted\n");
   engrave_program_signal_set(Cur.epr,etk_entry_text_get(ETK_ENTRY(UI_SignalEntry)));
   return ETK_TRUE;
}

Etk_Bool
on_DelaySpinners_value_changed(Etk_Range *range, double value, void *data)
{
   printf("value Changed Signal on DelayFromSpinner Emitted\n");
   engrave_program_in_set(Cur.epr,
      etk_range_value_get(ETK_RANGE(UI_DelayFromSpinner)),
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
   Cur.epr->targets=NULL; //ABSOLUTLY NOT THE RIGHT WAY!!!! TODO FIXME

   //Spit the string in token and add every targets
   tok = strtok (text,",");
   while (tok != NULL)
   {
      printf ("'%s'\n",tok);
      engrave_program_target_add(Cur.epr,tok);
      tok = strtok (NULL, ",");
   }

   //TODO Check if all the targets exists in the group, otherwise make the text red

   free(text);
   return ETK_TRUE;
}

Etk_Bool
on_Param1Entry_text_changed(Etk_Object *object, void *data)
{
   Engrave_Action action;
   printf("Text Changed Signal on Param1Entry Emitted\n");

   //get the action from the combo data
   action = (Engrave_Action)etk_combobox_item_data_get(
               etk_combobox_active_item_get (ETK_COMBOBOX(UI_ActionComboBox)));

   engrave_program_action_set(Cur.epr,action,
		etk_entry_text_get(ETK_ENTRY(UI_Param1Entry)),
		etk_entry_text_get(ETK_ENTRY(UI_Param2Entry)),
		Cur.epr->value,
		Cur.epr->value2
	);
   return ETK_TRUE;
}

Etk_Bool
on_Param2Entry_text_changed(Etk_Object *object, void *data)
{
   Engrave_Action action;
   printf("Text Changed Signal on Param2Entry Emitted\n");

   //get the action from the combo data
   action = (Engrave_Action)etk_combobox_item_data_get(
               etk_combobox_active_item_get (ETK_COMBOBOX(UI_ActionComboBox)));

   engrave_program_action_set(Cur.epr,action,
		etk_entry_text_get(ETK_ENTRY(UI_Param1Entry)),
      etk_entry_text_get(ETK_ENTRY(UI_Param2Entry)),
		Cur.epr->value,
		Cur.epr->value2
   );

   return ETK_TRUE;
}

Etk_Bool
on_Param1Spinner_value_changed(Etk_Range *range, double value, void *data)
{
   Engrave_Action action;
   printf("value Changed Signal on Param1Spinner Emitted\n");

   //get the action from the combo data
   action = (Engrave_Action)etk_combobox_item_data_get(
               etk_combobox_active_item_get (ETK_COMBOBOX(UI_ActionComboBox)));

   engrave_program_action_set(Cur.epr,action,
		etk_entry_text_get(ETK_ENTRY(UI_Param1Entry)),
		etk_entry_text_get(ETK_ENTRY(UI_Param2Entry)),
		etk_range_value_get(ETK_RANGE(UI_Param1Spinner)),
		Cur.epr->value2
	);

   return ETK_TRUE;
}

#if TEST_DIRECT_EDJE
Etk_Bool
on_GroupsComboBox_activated(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
{
   char *gr;
   gr = etk_combobox_item_field_get(item,0);
   printf("Group combo activated: %s\n",gr);
   ChangeGroup(gr);
}
#endif
Etk_Bool
on_TransitionComboBox_changed(Etk_Combobox *combobox, void *data)
{
   Engrave_Transition tran;
   printf("Changed Signal on TransitionComboBox Emitted\n");

   //get the transition from the combo data
   tran = (Engrave_Transition)etk_combobox_item_data_get(
               etk_combobox_active_item_get (combobox));

   engrave_program_transition_set(Cur.epr,tran,
      etk_range_value_get(ETK_RANGE(UI_DurationSpinner)));

   return ETK_TRUE;
}

Etk_Bool
on_DurationSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   Engrave_Transition tran;
   printf("value Changed Signal on DurationSpinner Emitted\n");

   //get the transition from the combo data
   tran = (Engrave_Transition)etk_combobox_item_data_get(
               etk_combobox_active_item_get (ETK_COMBOBOX(UI_TransiComboBox)));

   engrave_program_transition_set(Cur.epr,tran,
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
   Cur.epr->afters=NULL; //ABSOLUTLY NOT THE RIGHT WAY!!!! TODO FIXME

   //Spit the string in token and add every afters
   tok = strtok (text,",");
   while (tok != NULL)
   {
      printf ("'%s'\n",tok);
      engrave_program_after_add(Cur.epr,tok);
      tok = strtok (NULL, ",");
   }

   //TODO Check if all the after exists in the group, otherwise make the text red

   free(text);
   return ETK_TRUE;
}

/* Colors Callbacks */
Etk_Bool
on_ColorCanvas_realize(Etk_Widget *canvas, void *data)
{
   //Must use the realize callback on the EtkCanvas object.
   //Because I can't add any object to the canvas before it is realized
   Evas_Object* rect;
   //Add the colored rectangle
   rect = evas_object_rectangle_add  (etk_widget_toplevel_evas_get(canvas));
   etk_canvas_object_add (ETK_CANVAS(canvas), rect);
   evas_object_color_set(rect, 100,100,100,255);
   evas_object_resize(rect,300,300);
   etk_canvas_object_move(ETK_CANVAS(canvas),rect,0,0);
   evas_object_show(rect);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_DOWN, on_ColorCanvas_click, data);
   switch ((int)data){
      case COLOR_OBJECT_RECT:
         RectColorObject = rect;
         break;
      case COLOR_OBJECT_TEXT:
         TextColorObject = rect;
         break;
      case COLOR_OBJECT_SHADOW:
         ShadowColorObject = rect;
         break;
      case COLOR_OBJECT_OUTLINE:
         OutlineColorObject = rect;
         break;
   }

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
#if TEST_DIRECT_EDJE
         edje_edit_state_color_get(edje_o, Cur.part->string, Cur.state->string, &c.r,&c.g,&c.b,&c.a);
#else
         engrave_part_state_color_get(Cur.eps,&c.r,&c.g,&c.b,&c.a);
#endif
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_TEXT:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Text color");
#if TEST_DIRECT_EDJE
         edje_edit_state_color_get(edje_o, Cur.part->string, Cur.state->string, &c.r,&c.g,&c.b,&c.a);
#else
         engrave_part_state_color_get(Cur.eps,&c.r,&c.g,&c.b,&c.a);
#endif
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_SHADOW:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Shadow color");
#if TEST_DIRECT_EDJE
         edje_edit_state_color3_get(edje_o, Cur.part->string, Cur.state->string, &c.r,&c.g,&c.b,&c.a);
#else
         engrave_part_state_color2_get(Cur.eps,&c.r,&c.g,&c.b,&c.a);
#endif
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_OUTLINE:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Outline color");
#if TEST_DIRECT_EDJE
         edje_edit_state_color2_get(edje_o, Cur.part->string, Cur.state->string, &c.r,&c.g,&c.b,&c.a);
#else
         engrave_part_state_color3_get(Cur.eps,&c.r,&c.g,&c.b,&c.a);
#endif
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

   color = etk_colorpicker_current_color_get (ETK_COLORPICKER(object));
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
#if TEST_DIRECT_EDJE
      edje_edit_state_color_set(edje_o, Cur.part->string, Cur.state->string,
                                premuled.r,premuled.g,premuled.b,premuled.a);
#else
      engrave_part_state_color_set(Cur.eps,color.r,color.g,color.b,color.a);
#endif
      break;
    case COLOR_OBJECT_TEXT:
      evas_object_color_set(TextColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
#if TEST_DIRECT_EDJE
      edje_edit_state_color_set(edje_o, Cur.part->string, Cur.state->string,
                                premuled.r,premuled.g,premuled.b,premuled.a);
#else
      engrave_part_state_color_set(Cur.eps,color.r,color.g,color.b,color.a);
#endif
      break;
    case COLOR_OBJECT_SHADOW:
      evas_object_color_set(ShadowColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
#if TEST_DIRECT_EDJE
      edje_edit_state_color3_set(edje_o, Cur.part->string, Cur.state->string,
                                premuled.r,premuled.g,premuled.b,premuled.a);
#else
      engrave_part_state_color2_set(Cur.eps,color.r,color.g,color.b,color.a);
#endif
      break;
    case COLOR_OBJECT_OUTLINE:
      evas_object_color_set(OutlineColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
#if TEST_DIRECT_EDJE
      edje_edit_state_color2_set(edje_o, Cur.part->string, Cur.state->string,
                                premuled.r,premuled.g,premuled.b,premuled.a);
#else
      engrave_part_state_color3_set(Cur.eps,color.r,color.g,color.b,color.a);
#endif
      break;
   }

   ev_redraw();
   return ETK_TRUE;
}

/* Add/Remove Buttons Callbacks */
Etk_Bool
on_AddMenu_item_activated(Etk_Object *object, void *data)
{
   Engrave_Group *group = NULL;
   Engrave_Part *part;
   Engrave_Part_State *new_state;
   Engrave_Program *prog = NULL;

   printf("Item Activated Signal on AddMenu EMITTED\n");

   switch ((int)data)
   {
      case NEW_RECT:
         if (Cur.eg){
            part = engrave_part_new(ENGRAVE_PART_TYPE_RECT);
            engrave_part_name_set (part, "new rectangle");
            engrave_group_part_add(Cur.eg, part);
            AddPartToTree(part, 0, NULL);

            new_state = engrave_part_state_new();
            engrave_part_state_name_set(new_state, "default", 0.0);
            engrave_part_state_rel1_relative_set(new_state, 0.1, 0.1);
            engrave_part_state_rel2_relative_set(new_state, 0.9, 0.9);
            engrave_part_state_add(part,new_state);
            AddStateToTree(new_state);

            Cur.ep = part;

            etk_tree_row_select(ecore_hash_get(hash,new_state));
            etk_tree_row_unfold(ecore_hash_get(hash,Cur.eg));
            etk_tree_row_unfold(ecore_hash_get(hash,Cur.ep)); 

            PopulateRelComboBoxes();
         }else{
            ShowAlert("You must first select a group.");
         }
         break;
      case NEW_IMAGE:
         if (Cur.eg){
            part = engrave_part_new(ENGRAVE_PART_TYPE_IMAGE);
            engrave_part_name_set (part, "new image");
            engrave_group_part_add(Cur.eg, part);
            AddPartToTree(part, 0, NULL);

            new_state = engrave_part_state_new();
            engrave_part_state_name_set(new_state, "default", 0.0);
            engrave_part_state_rel1_relative_set(new_state, 0.1, 0.1);
            engrave_part_state_rel2_relative_set(new_state, 0.9, 0.9);
            engrave_part_state_add(part,new_state);
            AddStateToTree(new_state);

            Cur.ep = part;

            etk_tree_row_select(ecore_hash_get(hash,new_state));
            etk_tree_row_unfold(ecore_hash_get(hash,Cur.eg));
            etk_tree_row_unfold(ecore_hash_get(hash,Cur.ep));

            PopulateRelComboBoxes();
         }else{
            ShowAlert("You must first select a group.");
         }
         break;
      case NEW_TEXT:
         if (Cur.eg){
            part = engrave_part_new(ENGRAVE_PART_TYPE_TEXT);
            engrave_part_name_set (part, "new text");
            engrave_group_part_add(Cur.eg, part);
            AddPartToTree(part, 0, NULL);

            new_state = engrave_part_state_new();
            engrave_part_state_name_set(new_state, "default", 0.0);
            engrave_part_state_rel1_relative_set(new_state, 0.1, 0.1);
            engrave_part_state_rel2_relative_set(new_state, 0.9, 0.9);
            engrave_part_state_text_size_set(new_state,16);
            engrave_part_state_text_text_set(new_state,"something to say...");
            
            engrave_part_state_add(part,new_state);
            engrave_part_effect_set(part,ENGRAVE_TEXT_EFFECT_GLOW);
            
            AddStateToTree(new_state);

            Cur.ep = part;

            etk_tree_row_select(ecore_hash_get(hash,new_state));
            etk_tree_row_unfold(ecore_hash_get(hash,Cur.eg));
            etk_tree_row_unfold(ecore_hash_get(hash,Cur.ep));

            PopulateRelComboBoxes();
         }else{
            ShowAlert("You must first select a group.");
         }
         break;
      case NEW_PROG:
         if (Cur.eg){
            prog = engrave_program_new();
            engrave_program_name_set(prog,"new program");
            engrave_group_program_add(Cur.eg,prog);
            AddProgramToTree(prog);
         }else{
            ShowAlert("You must first select a group.");
         }
         break;
      case NEW_DESC:
         if (Cur.ep){
            new_state = engrave_part_state_new();
            engrave_part_state_name_set(new_state, "state", 0.0);
            engrave_part_state_add(Cur.ep,new_state);
            AddStateToTree(new_state);

            etk_tree_row_select(ecore_hash_get(hash,new_state));
            etk_tree_row_unfold(ecore_hash_get(hash,Cur.ep));
            etk_tree_row_unfold(ecore_hash_get(hash,Cur.eg));

         }else{
            ShowAlert("You must first select a part.");
         }
         break;
      case NEW_GROUP:
         group = engrave_group_new();
         engrave_group_name_set (group, "New group");
         engrave_file_group_add (Cur.ef, group);

         AddGroupToTree(group);

         etk_tree_row_select(ecore_hash_get(hash,group));
         break;
   }
   ev_redraw();

   return ETK_TRUE;
}

Etk_Bool
on_RemoveMenu_item_activated(Etk_Object *object, void *data)
{
   Etk_Tree_Row* row;
   printf("Item Activated Signal on RemoveMenu EMITTED\n");
   switch ((int)data){
      case REMOVE_DESCRIPTION:
         if (Cur.eps){
            if (strcmp(engrave_part_state_name_get(Cur.eps,NULL),"default") || Cur.eps->value != 0){
               printf("REMOVE DESCRIPTION: %s\n",Cur.eps->name);
               row = etk_tree_row_next_get(ecore_hash_get(hash,Cur.eps));
               if (!row) row = etk_tree_row_prev_get(ecore_hash_get(hash,Cur.eps));
               etk_tree_row_delete(ecore_hash_get(hash,Cur.eps));
               ecore_hash_remove (hash, Cur.eps);
               PROTO_engrave_part_state_remove(Cur.ep, Cur.eps);
               engrave_part_state_free(Cur.eps);

               Cur.eps = NULL;
               if (row) etk_tree_row_select (row);
               ev_redraw();

            }else{
               ShowAlert("You can't remove default 0.0");
            }
         }else{
            ShowAlert("No part state selected");
         }
      break;
      case REMOVE_PART:
         if (Cur.ep){
            printf("REMOVE PART: %s\n",Cur.ep->name);
            row = NULL;
            row = etk_tree_row_next_get(ecore_hash_get(hash,Cur.ep));
            if (!row) row = etk_tree_row_prev_get(ecore_hash_get(hash,Cur.ep));
            etk_tree_row_delete(ecore_hash_get(hash,Cur.ep));
            ecore_hash_remove (hash, Cur.ep);
            PROTO_engrave_group_part_remove(Cur.eg,Cur.ep);
            engrave_part_free(Cur.ep);

            Cur.ep = NULL;
            Cur.eps = NULL;

            if (row) etk_tree_row_select(row);
            else etk_tree_row_select(etk_tree_row_last_child_get (etk_tree_last_row_get (ETK_TREE(UI_PartsTree))));

            ev_redraw();
         }else{
            ShowAlert("No part selected");
         }
      break;
      case REMOVE_GROUP:
         if (Cur.eg)
         {
               row = NULL;
               row = etk_tree_row_next_get(ecore_hash_get(hash,Cur.eg));
               if (!row) row = etk_tree_row_prev_get(ecore_hash_get(hash,Cur.eg));

               etk_tree_row_delete(ecore_hash_get(hash,Cur.eg));
               ecore_hash_remove (hash, Cur.eg);
               PROTO_engrave_file_group_remove(Cur.ef,Cur.eg);
               engrave_group_free(Cur.eg);

               Cur.eg = NULL;
               Cur.ep = NULL;
               Cur.eps = NULL;

               ev_redraw();
               if (row) etk_tree_row_select(row);
         }else{
            ShowAlert("No group selected");
         }
      break;
      case REMOVE_PROG:
         if (Cur.epr)
         {
            row = NULL;
            row = etk_tree_row_next_get(ecore_hash_get(hash,Cur.epr));
            if (!row) row = etk_tree_row_prev_get(ecore_hash_get(hash,Cur.epr));
            etk_tree_row_delete(ecore_hash_get(hash,Cur.epr));
            ecore_hash_remove (hash, Cur.epr);
            PROTO_engrave_group_program_remove(Cur.eg, Cur.epr);
            engrave_program_free(Cur.epr);

            Cur.epr = NULL;

            ev_redraw();
            if (row) etk_tree_row_select(row);
         }else{
            ShowAlert("No program selected");
         }
      break;
   }
   return ETK_TRUE;
}

/* Dialogs Callbacks */
Etk_Bool
on_FileChooserDialog_response(Etk_Dialog *dialog, int response_id, void *data)
{
   char cmd[4096];
   int ret = 0;

   printf("Response Signal on Filechooser EMITTED\n");

   if (response_id == ETK_RESPONSE_OK){

      switch(FileChooserOperation){
         case FILECHOOSER_OPEN:
#if TEST_DIRECT_EDJE
            snprintf(cmd,4096,"%s/%s",
            etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
            etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            LoadEDJ2(cmd);
#else
            snprintf(cmd,4096,"edje_editor \"%s/%s\" &",
               etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
               etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            system(cmd);
#endif
         break;
         case FILECHOOSER_SAVE_EDJ:
            printf("SAVE EDJ\n");
            snprintf(cmd,4096,"%s/%s",
               etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
               etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
#if TEST_DIRECT_EDJE
            edje_edit_save(edje_o);
            if(!ecore_file_cp(Cur.edj_temp_name->string, cmd))
            {
               ShowAlert("<b>ERROR:<\b><br>Can't write file");
            }
#else
            SaveEDJ(cmd);
#endif
         break;
         case FILECHOOSER_SAVE_EDC:
            printf("SAVE EDC\n");
            snprintf(cmd,4096,"%s/%s",
               etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
               etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            if (!SaveEDC(cmd))
              ShowAlert("Error saving file.");
         break;
         case FILECHOOSER_IMAGE:
#if TEST_DIRECT_EDJE
            snprintf(cmd, 4096, "%s/%s", 
               etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
               etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            edje_edit_image_add(edje_o, cmd);
            PopulateImagesComboBox();
#else
            if (Cur.eps){
               //If the new image is not in the edc dir
               if (strcmp(etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),engrave_file_image_dir_get(Cur.ef))){
                  //TODO check if image already exist and is a valid image
                  //Copy the image to the image_dir
                  snprintf(cmd, 4096, "cp \"%s/%s\" \"%s\"", 
                           etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
                           etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
                           engrave_file_image_dir_get(Cur.ef));
                  ret = system(cmd);
                  if (ret < 0) {
                     ShowAlert("Error: unable to copy image!");
                     return ETK_TRUE;
                  }
               }
               //Set the new image
               Engrave_Image* eimg;
               eimg = engrave_image_new(etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),ENGRAVE_IMAGE_TYPE_LOSSY,95);	 
               engrave_file_image_add(Cur.ef,eimg);
               if (!etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(UI_ImageTweenRadio)))
                  engrave_part_state_image_normal_set(Cur.eps,eimg);
               else
                  engrave_part_state_image_tween_add(Cur.eps,eimg);
               
               PopulateImagesComboBox();
               UpdateImageFrame();
               ev_redraw();
            }
#endif
         break;
         case FILECHOOSER_FONT:
            if (Cur.eps){
               //If the new font is not in the edc dir
               if (strcmp(etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),engrave_file_font_dir_get(Cur.ef))){
                  //TODO check if font already exist in the EDCFileDir
                  //Copy the font to the EDCDir
                  snprintf(cmd, 4096, "cp \"%s/%s\" \"%s\"",
                        etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
                        etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
                        engrave_file_font_dir_get(Cur.ef));
                  ret = system(cmd);
                  if (ret < 0) {
                     ShowAlert("Error: unable to copy font!");
                     return ETK_TRUE;
                  }
               }
               //Set the new font
               Engrave_Font *efont;
               efont = engrave_font_new(
                  etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
                  etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
               engrave_file_font_add(Cur.ef,efont);
               engrave_part_state_text_font_set(Cur.eps,etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)));

               PopulateFontsComboBox();
               UpdateTextFrame();
               ev_redraw();
            }
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
