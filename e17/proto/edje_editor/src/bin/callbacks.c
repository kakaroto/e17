#include <string.h>
#include <Edje.h>
#include <Etk.h>
#include "callbacks.h"
#include "interface.h"
#include "main.h"
#include "evas.h"

extern void PROTO_engrave_part_state_remove(Engrave_Part *ep, Engrave_Part_State *eps);
extern void PROTO_engrave_group_part_remove(Engrave_Group *eg, Engrave_Part *ep);
extern void PROTO_engrave_file_group_remove(Engrave_File *ef, Engrave_Group *eg);

int current_color_object;

/* Called when the window is destroyed */
void etk_main_quit_cb(void *data){
   etk_main_quit();
}

/* Called when the canvas change size */
void on_canvas_geometry_changed(Etk_Object *canvas, const char *property_name, void *data){
   int cx, cy, cw, ch;
   //printf("Geometry Changed Signal on Canvas\n");
   //resize canvas bg
   etk_widget_geometry_get(ETK_canvas, &cx, &cy, &cw, &ch);
   evas_object_resize(EV_canvas_bg,cw,ch);
   evas_object_resize(EV_canvas_shadow,cw,ch);
   evas_object_image_fill_set( EV_canvas_shadow,	0,0,cw,ch);
   ev_redraw();
}

/* All the buttons Callback */
void
on_AllButton_click(Etk_Button *button, void *data)
{
   //GList *current,*prev;
   switch ((int)data)
   {
      case TOOLBAR_NEW:
         printf("Clicked signal on Toolbar Button 'New' EMITTED\n");
         //ShowFilechooser(FILECHOOSER_NEW);
         ShowAlert("Not yet implemented");
         break;
      case TOOLBAR_OPEN:
         printf("Clicked signal on Toolbar Button 'Open' EMITTED\n");
         //ShowAlert("Not yet implemented");
         ShowFilechooser(FILECHOOSER_OPEN);
         break;
      case TOOLBAR_SAVE:
         printf("Clicked signal on Toolbar Button 'Save' EMITTED\n");
         //SaveEDC(NULL);
         ShowAlert("Not yet implemented");
         break;
      case TOOLBAR_SAVE_AS:
         printf("Clicked signal on Toolbar Button 'Save as' EMITTED\n");
         //ShowFilechooser(FILECHOOSER_SAVE_AS);
         ShowAlert("Not yet implemented");
         break;
      case TOOLBAR_ADD:
         printf("Clicked signal on Toolbar Button 'Add' EMITTED\n");
         etk_menu_popup(ETK_MENU(UI_AddMenu));
         //etk_menu_popup_at_xy (ETK_MENU(AddMenu), 10, 10);
         break;
      case TOOLBAR_REMOVE:
         printf("Clicked signal on Toolbar Button 'Remove' EMITTED\n");
         //ShowAlert("Not yet implemented");
         etk_menu_popup(ETK_MENU(UI_RemoveMenu));
         //etk_menu_popup_at_xy (ETK_MENU(AddMenu), 10, 10);
         break;
      case TOOLBAR_MOVE_UP:
         printf("Clicked signal on Toolbar Button 'MoveUp' EMITTED\n");
         /* if (selected_desc){
            printf("MoveUP DESC: %s\n",selected_desc->state->str);
         }
         else if (selected_part){
            if ((current = g_list_find(selected_part->group->parts,selected_part))){
               if ((prev = g_list_previous(current))){
                  printf("MoveUP PART: %s\n",selected_part->name->str);
                  //current->data = prev->data;
                  //prev->data = selected_part;
                  //Update the tree
               }
            }
         }
         else{
            ShowAlert("No part to move selected");
         } */
         ShowAlert("Not yet implemented");
         break;
      case TOOLBAR_MOVE_DOWN:
         printf("Clicked signal on Toolbar Button 'MoveDown' EMITTED\n");
         ShowAlert("Not yet implemented");
         break;
      case TOOLBAR_PLAY:
         printf("Clicked signal on Toolbar Button 'Play' EMITTED\n");
         ShowAlert("Not yet implemented");
         //SaveEDC(NULL);
         //PlayEDC();
         break;
      case TOOLBAR_DEBUG:
         DebugInfo(FALSE);
         //ZOOM TEST :(
         //int x,y,w,h;
         //evas_output_size_get (etk_widget_toplevel_evas_get (ETK_canvas), &w, &h);
         //evas_output_size_set (etk_widget_toplevel_evas_get (ETK_canvas), w+10, h+10);
         //evas_output_viewport_get (etk_widget_toplevel_evas_get (ETK_canvas), &x, &y, &w, &h);
         //evas_output_viewport_set (etk_widget_toplevel_evas_get (ETK_canvas), x+20, y+20, w+10, h+10);
         //printf("ZOOM: %d %d - %d %d\n",x,y,w,h);
         break;
      case TOOLBAR_IMAGE_FILE_ADD:
         ShowAlert("Not yet implemented =)");
         //printf("INSERT IMAGE\n");
         //if (EDCFile->len > 0) ShowFilechooser(FILECHOOSER_IMAGE);
         //e   lse ShowAlert("You have to save the file once for insert image.");
            break;
      case TOOLBAR_FONT_FILE_ADD:
         ShowAlert("Not yet implemented =)");
         //printf("INSERT FONT\n");
         //i   f (EDCFile->len > 0) ShowFilechooser(FILECHOOSER_FONT);
         //else ShowAlert("You have to save the file once for insert font.");
         break;
   }
}

/* Tree callbacks */
void
on_PartsTree_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   int row_type=0;
   Engrave_Group* old_group = Cur.eg;

   printf("Row Selected Signal on one of the Tree EMITTED \n");

   //get the type of the row (group,part,desc or prog) from the hidden col
   etk_tree_row_fields_get(row,
      etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), 2),&row_type,
      NULL);

   switch (row_type)
   {
      case ROW_GROUP:
         Cur.epr = NULL;
         Cur.eg = etk_tree_row_data_get (row);
         Cur.ep = NULL;
         Cur.eps = NULL;
         etk_widget_hide(UI_DescriptionFrame);
         etk_widget_hide(UI_PositionFrame);
         etk_widget_hide(UI_RectFrame);
         etk_widget_hide(UI_ImageFrame);
         etk_widget_hide(UI_TextFrame);
         etk_widget_hide(UI_PartFrame);
         etk_widget_hide(UI_ProgramFrame);
         etk_widget_show(UI_GroupFrame);
         break;
      case ROW_PART:
         Cur.epr = NULL;
         Cur.ep = etk_tree_row_data_get (row);
         Cur.eg = Cur.ep->parent;
         Cur.eps = NULL;
         etk_widget_hide(UI_DescriptionFrame);
         etk_widget_hide(UI_PositionFrame);
         etk_widget_hide(UI_RectFrame);
         etk_widget_hide(UI_ImageFrame);
         etk_widget_hide(UI_TextFrame);
         etk_widget_hide(UI_GroupFrame);
         etk_widget_hide(UI_ProgramFrame);
         etk_widget_show(UI_PartFrame);
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
            etk_widget_show(UI_TextFrame);
         }else
         {
            etk_widget_hide(UI_TextFrame);
         }

         if (Cur.ep->type == ENGRAVE_PART_TYPE_IMAGE)
         {
            UpdateImageFrame();
            etk_widget_show(UI_ImageFrame);
         }else
         {
            etk_widget_hide(UI_ImageFrame);
         }

         if (Cur.ep->type == ENGRAVE_PART_TYPE_RECT)
         {
            UpdateRectFrame();
            etk_widget_show(UI_RectFrame);
         }else
         {
            etk_widget_hide(UI_RectFrame);
         }

         etk_widget_hide(UI_PartFrame);
         etk_widget_hide(UI_GroupFrame);
         etk_widget_hide(UI_ProgramFrame);
         etk_widget_show(UI_DescriptionFrame);
         etk_widget_show(UI_PositionFrame);
         break;
      case ROW_PROG:
         Cur.epr = etk_tree_row_data_get (row);
         Cur.eg = Cur.epr->parent;
         Cur.ep = NULL;
         Cur.eps = NULL;
         etk_widget_hide(UI_DescriptionFrame);
         etk_widget_hide(UI_PositionFrame);
         etk_widget_hide(UI_RectFrame);
         etk_widget_hide(UI_ImageFrame);
         etk_widget_hide(UI_TextFrame);
         etk_widget_hide(UI_GroupFrame);
         etk_widget_hide(UI_PartFrame);
         etk_widget_show(UI_ProgramFrame);
         UpdateProgFrame();
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

      engrave_canvas_current_group_set (ecanvas, Cur.eg);
   }
   ev_redraw();
}

/* Group frame callbacks */
void
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
}

void
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
                  etk_range_value_get(ETK_RANGE(UI_GroupMinWSpinner)),
                  etk_range_value_get(ETK_RANGE(UI_GroupMinHSpinner)));
            break;
         case MINH_SPINNER:
            engrave_group_min_size_set(Cur.eg,
                  etk_range_value_get(ETK_RANGE(UI_GroupMinWSpinner)),
                  etk_range_value_get(ETK_RANGE(UI_GroupMinHSpinner)));
            break;
         case MAXW_SPINNER:
            engrave_group_max_size_set(Cur.eg,
                  etk_range_value_get(ETK_RANGE(UI_GroupMaxWSpinner)),
                  etk_range_value_get(ETK_RANGE(UI_GroupMaxHSpinner)));
            break;
         case MAXH_SPINNER:
            engrave_group_max_size_set(Cur.eg,
                  etk_range_value_get(ETK_RANGE(UI_GroupMaxWSpinner)),
                  etk_range_value_get(ETK_RANGE(UI_GroupMaxHSpinner)));
            break;
      }
   }
}

/* Parts & Descriptions Callbacks*/
void
on_PartNameEntry_text_changed(Etk_Object *object, void *data)
{
   Etk_Tree_Col *col1=NULL;

   //printf("Text Changed Signal on PartNameEntry EMITTED (text: %s)\n",etk_entry_text_get(ETK_ENTRY(object)));

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
}

void
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
}

void
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
      //Update PartTree
      col1 = etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), 0);
      snprintf(buf,4095,"%s %.2f",Cur.eps->name,Cur.eps->value);
      etk_tree_row_fields_set(ecore_hash_get(hash,Cur.eps),TRUE,
         col1,EdjeFile,"DESC.PNG",buf,NULL);
   }
}

void
on_AspectSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on AspectMinSpinner EMITTED\n");
   engrave_part_state_aspect_set(Cur.eps,
      etk_range_value_get(ETK_RANGE(UI_AspectMinSpinner)),
      etk_range_value_get(ETK_RANGE(UI_AspectMaxSpinner)));
}

void
on_AspectComboBox_changed(Etk_Combobox *combobox, void *data)
{
   Engrave_Aspect_Preference prefer;
   printf("Active Item Changed Signal on AspectComboBox EMITTED\n");

   prefer = (Engrave_Aspect_Preference)etk_combobox_item_data_get(etk_combobox_active_item_get (combobox));
   engrave_part_state_aspect_preference_set(Cur.eps,prefer);

}

void
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
}

/* Image Frame Callbacks */
void
on_ImageComboBox_changed(Etk_Combobox *combobox, void *data)
{
   Engrave_Image *image;
   printf("Changed signal on Image Combo EMITED\n");

   if ((image = etk_combobox_item_data_get(etk_combobox_active_item_get (combobox)))){
      //Set an existing image
      if (Cur.eps){
         engrave_part_state_image_normal_set(Cur.eps, image);
         ev_redraw();
      }
   }
}

void
on_ImageAlphaSlider_value_changed(Etk_Object *object, double va, void *data)
{
   printf("ImageSlieder value_changed signale EMIT: %.2f\n",va);
   if (Cur.eps){
      engrave_part_state_color_set(Cur.eps, (int)va, (int)va, (int)va, (int)va);
      ev_redraw();
   }
}

void
on_BorderSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed signal on BorderSpinner EMITTED (value: %f)\n",etk_range_value_get(range));
   if (Cur.eps){
      engrave_part_state_image_border_set(Cur.eps,
         (int)etk_range_value_get(ETK_RANGE(UI_BorderLeftSpinner)),
         (int)etk_range_value_get(ETK_RANGE(UI_BorderRightSpinner)),
         (int)etk_range_value_get(ETK_RANGE(UI_BorderTopSpinner)),
         (int)etk_range_value_get(ETK_RANGE(UI_BorderBottomSpinner)));

      printf("TODO: s: %s  [%d] %d\n",Cur.eps->name,Cur.eps->image.border.l,(int)etk_range_value_get(ETK_RANGE(UI_BorderLeftSpinner)));

      ev_redraw();
   }
}

/* Position Frame Callbacks */
void
on_RelToComboBox_changed(Etk_Combobox *combobox, void *data)
{
   printf("RelTocomboBox changed signal EMITTED \n");
   Engrave_Part* part = NULL;

   part = etk_combobox_item_data_get (etk_combobox_active_item_get (combobox));
   if (part)
   {
      if (part == Cur.ep ) 
      {
         ShowAlert("A state can't rel to itself.");
         return;
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
}

void
on_RelSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on RelSpinner EMITTED (value: %f)\n",etk_range_value_get(range));

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
}

void
on_RelOffsetSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on Offset Spinner EMITTED\n");

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

}

/* Text Frame Callbacks */
void on_FontComboBox_changed(Etk_Combobox *combobox, void *data){
   printf("Changed Signal on FontComboBox EMITTED \n");
/*    char* font;
   if ((font = etk_combobox_item_data_get(etk_combobox_active_item_get (combobox)))){
      //Set an existing font
      if (selected_desc){
   g_string_printf(selected_desc->text_font,"%s",font);
   ev_draw_part(selected_part);

      }
   }
   else{
      //Insert a new font in EDC
      printf("INSERT FONT\n");
      ShowFilechooser(FILECHOOSER_FONT);
   } */
}

void 
on_EffectComboBox_changed(Etk_Combobox *combobox, void *data)
{
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
}

void 
on_FontSizeSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   printf("Value Changed Signal on FontSizeSpinner EMITTED (value: %d)\n",(int)etk_range_value_get(range));

   engrave_part_state_text_size_set(Cur.eps,(int)etk_range_value_get(range));

   ev_redraw();
}

void 
on_TextEntry_text_changed(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on TextEntry EMITTED (value %s)\n",etk_entry_text_get(ETK_ENTRY(object)));

   engrave_part_state_text_text_set(Cur.eps,etk_entry_text_get(ETK_ENTRY(object)));


   ev_redraw();
}

/* Programs Callbacks */
void
on_ActionComboBox_changed(Etk_Combobox *combobox, void *data)
{
   Engrave_Action action;
   char param1[200],param2[200];
   double value,value2;
   printf("Changed Signal on ActionComboBox EMITTED\n");

   if (!Cur.epr) return;

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
      etk_label_set(UI_Param1Label, "<b>Signal</b>");
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
      etk_label_set(UI_Param1Label, "<b>State</b>");
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
}

void
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
}

void
on_SourceEntry_text_changed(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on SourceEntry Emitted\n");
   engrave_program_source_set(Cur.epr,etk_entry_text_get(ETK_ENTRY(UI_SourceEntry)));
}
void
on_SignalEntry_text_changed(Etk_Object *object, void *data)
{
   printf("Text Changed Signal on SignalEntry Emitted\n");
   engrave_program_signal_set(Cur.epr,etk_entry_text_get(ETK_ENTRY(UI_SignalEntry)));
}

void 
on_DelaySpinners_value_changed(Etk_Range *range, double value, void *data)
{
   printf("value Changed Signal on DelayFromSpinner Emitted\n");
   engrave_program_in_set(Cur.epr,
      etk_range_value_get(ETK_RANGE(UI_DelayFromSpinner)),
      etk_range_value_get(ETK_RANGE(UI_DelayRangeSpinner)));
}

void
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
}

void
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
}

void
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
	
}

void
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
}

void
on_TransitionComboBox_changed(Etk_Combobox *combobox, void *data)
{
   Engrave_Transition tran;
   printf("Changed Signal on TransitionComboBox Emitted\n");
   
   //get the transition from the combo data
   tran = (Engrave_Transition)etk_combobox_item_data_get(
               etk_combobox_active_item_get (combobox));
   
   engrave_program_transition_set(Cur.epr,tran,
      etk_range_value_get(ETK_RANGE(UI_DurationSpinner)));
}

void
on_DurationSpinner_value_changed(Etk_Range *range, double value, void *data)
{
   Engrave_Transition tran;
   printf("value Changed Signal on DurationSpinner Emitted\n");
   
   //get the transition from the combo data
   tran = (Engrave_Transition)etk_combobox_item_data_get(
               etk_combobox_active_item_get (ETK_COMBOBOX(UI_TransiComboBox)));
   
   engrave_program_transition_set(Cur.epr,tran,
      etk_range_value_get(ETK_RANGE(UI_DurationSpinner)));
}

void
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
}

/* Colors Callbacks */
void
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
}

void 
on_ColorCanvas_click(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Color c;
   printf("Clik Signal on ColorCanvas Emitted\n");
   if (UI_ColorWin) etk_widget_show_all(UI_ColorWin);
   current_color_object = (int)data;

   etk_signal_block("color_changed", ETK_OBJECT(UI_ColorPicker), ETK_CALLBACK(on_ColorDialog_change));
   switch (current_color_object)
   {
      case COLOR_OBJECT_RECT:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Rectangle color");
         engrave_part_state_color_get(Cur.eps,&c.r,&c.g,&c.b,&c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_TEXT:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Text color");
         engrave_part_state_color_get(Cur.eps,&c.r,&c.g,&c.b,&c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_SHADOW:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Shadow color");
         engrave_part_state_color2_get(Cur.eps,&c.r,&c.g,&c.b,&c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_OUTLINE:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Outline color");
         engrave_part_state_color3_get(Cur.eps,&c.r,&c.g,&c.b,&c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
   }
   etk_signal_unblock("color_changed", ETK_OBJECT(UI_ColorPicker), ETK_CALLBACK(on_ColorDialog_change));
}

void
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
      engrave_part_state_color_set(Cur.eps,color.r,color.g,color.b,color.a);
      break;
    case COLOR_OBJECT_TEXT:
      evas_object_color_set(TextColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
      engrave_part_state_color_set(Cur.eps,color.r,color.g,color.b,color.a);
      break;
    case COLOR_OBJECT_SHADOW:
      evas_object_color_set(ShadowColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
      engrave_part_state_color2_set(Cur.eps,color.r,color.g,color.b,color.a);
      break;
    case COLOR_OBJECT_OUTLINE:
      evas_object_color_set(OutlineColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
      engrave_part_state_color3_set(Cur.eps,color.r,color.g,color.b,color.a);
      break;
   }

   ev_redraw();
}

void 
on_AddMenu_item_activated(Etk_Object *object, void *data)
{
   Engrave_Group *group = NULL;
   Engrave_Part *part;
   Engrave_Part_State *new_state;

   printf("Item Activated Signal on AddMenu EMITTED\n");

 /*  if (!Cur.eg && ((int)data != NEW_DESC) && ((int)data != NEW_GROUP))
   {
      group = engrave_group_new();
      engrave_group_name_set (group, "New group");
      engrave_file_group_add (Cur.ef, group);

      AddGroupToTree(group);
      Cur.eg = group;
   }*/
   switch ((int)data)
   {
      case NEW_RECT:
         if (Cur.eg){
            part = engrave_part_new(ENGRAVE_PART_TYPE_RECT);
            engrave_part_name_set (part, "new rectangle");
            engrave_group_part_add(Cur.eg, part);
            AddPartToTree(part);

            new_state = engrave_part_state_new();
            engrave_part_state_name_set(new_state, "default", 0.0);
            engrave_part_state_rel1_relative_set(new_state, 0.1, 0.1);
            engrave_part_state_rel2_relative_set(new_state, 0.9, 0.9);
            engrave_part_state_add(part,new_state);
            AddStateToTree(new_state);

            Cur.ep = part;
            //Cur.eps = new_state;

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
            AddPartToTree(part);

            new_state = engrave_part_state_new();
            engrave_part_state_name_set(new_state, "default", 0.0);
            engrave_part_state_rel1_relative_set(new_state, 0.1, 0.1);
            engrave_part_state_rel2_relative_set(new_state, 0.9, 0.9);
            engrave_part_state_add(part,new_state);
            AddStateToTree(new_state);

            Cur.ep = part;
            //Cur.eps = new_state;

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
            AddPartToTree(part);

            new_state = engrave_part_state_new();
            engrave_part_state_name_set(new_state, "default", 0.0);
            engrave_part_state_rel1_relative_set(new_state, 0.1, 0.1);
            engrave_part_state_rel2_relative_set(new_state, 0.9, 0.9);
            engrave_part_state_add(part,new_state);
            AddStateToTree(new_state);

            Cur.ep = part;
            //Cur.eps = new_state;

            etk_tree_row_select(ecore_hash_get(hash,new_state));
            etk_tree_row_unfold(ecore_hash_get(hash,Cur.eg));
            etk_tree_row_unfold(ecore_hash_get(hash,Cur.ep));

            PopulateRelComboBoxes();
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

            //Cur.eps = new_state;

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
}

void
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
   }
}

/* Dialogs Callbacks */
void
on_FileChooser_response(Etk_Dialog *dialog, int response_id, void *data)
{
   Etk_String *cmd = etk_string_new("");

   printf("Response Signal on Filechooser EMITTED\n");

   if (response_id == ETK_RESPONSE_OK){

      switch(FileChooserOperation){
         case FILECHOOSER_OPEN:
            etk_string_append_printf(cmd,"edje_editor %s/%s &",
               etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
               etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            system(cmd->string);


         break;
       /*   case FILECHOOSER_NEW:
            //printf("new: %s\n",etk_entry_text_get(UI_FilechooserFileNameEntry));
            ClearAll();
            CreateBlankEDC();
            g_string_printf(EDCFile,"%s",etk_entry_text_get(ETK_ENTRY(UI_FilechooserFileNameEntry)));
            g_string_printf(EDCFileDir,"%s",g_path_get_dirname(EDCFile->str));
            UpdateWindowTitle();
         break;
         case FILECHOOSER_IMAGE:
            printf("new image: %s\n",etk_entry_text_get(ETK_ENTRY(UI_FilechooserFileNameEntry)));
            if (selected_desc){
               //If the new image is not in the edc dir
               if (strcmp(etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)),EDCFileDir->str)){
                  //TODO check if image already exist in the EDCFileDir
                  //Copy the image to the EDCDir
                  FileCopy((char*)etk_entry_text_get(ETK_ENTRY(UI_FilechooserFileNameEntry)),EDCFileDir->str);
               }
               //Set the image name
               g_string_printf(selected_desc->image_normal,"%s",etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
               UpdateImageFrame();
               ev_draw_part(selected_part);

               UpdateImageComboBox();
               UpdateImageFrame();
            }
         break;
         case FILECHOOSER_FONT:
            printf("new font: %s\n",etk_entry_text_get(ETK_ENTRY(UI_FilechooserFileNameEntry)));
            if (selected_desc){
               //If the new font is not in the edc dir
               if (strcmp(etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)),EDCFileDir->str)){
                  //TODO check if font already exist in the EDCFileDir
                  //Copy the font to the EDCDir
                  FileCopy((char*)etk_entry_text_get(ETK_ENTRY(UI_FilechooserFileNameEntry)),EDCFileDir->str);
               }
               //Set the font name
               g_string_printf(selected_desc->text_font,"%s",etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)));

               UpdateFontComboBox();

               UpdateTextFrame();
               ev_draw_part(selected_part);
            }
         break;
         case FILECHOOSER_SAVE_AS:
            SaveEDC((char*)etk_entry_text_get(ETK_ENTRY(UI_FilechooserFileNameEntry)));
         break; */
      }
      etk_widget_hide(ETK_WIDGET(dialog));
   }
   else{
      etk_widget_hide(ETK_WIDGET(dialog));
   }
   etk_object_destroy(ETK_OBJECT(cmd));
}

void 
on_FileChooser_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_String *str=etk_string_new("");


   if (etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)))
     etk_string_append_printf(str,"%s/",etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)));

   if (etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)))
     str = etk_string_append_printf(str,etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)));


   etk_entry_text_set(ETK_ENTRY(UI_FilechooserFileNameEntry),str->string);
   etk_object_destroy(ETK_OBJECT(str));
}

void on_PlayDialog_response(Etk_Dialog *dialog, int response_id, void *data){
/*    GString *command = g_string_new("");
   if (response_id == ETK_RESPONSE_OK){
      printf("TEST IN VIEWER\n");
      g_string_printf(command,"edje_viewer %s",EDCFile->str);
      command->str[command->len - 1] = 'j';
      command = g_string_append(command," &");
      printf("TEST IN VIEWER %s\n",command->str);
      system(command->str);
      etk_widget_hide(UI_PlayDialog);
   }
   else{
      etk_widget_hide(ETK_WIDGET(dialog));
   }
   g_string_free(command,TRUE); */
}

void 
on_AlertDialog_response(Etk_Dialog *dialog, int response_id, void *data)
{
   etk_widget_hide(ETK_WIDGET(dialog));
}

