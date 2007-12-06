#include <dirent.h>
#include <string.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include <Etk.h>
#include "main.h"
#include "callbacks.h"
#include "interface.h"
#include "evas.h"


static Evas_Object *logo;


void
ShowAlert(char* text)
{
   etk_message_dialog_text_set (ETK_MESSAGE_DIALOG(UI_AlertDialog), text);
   etk_widget_show_all(UI_AlertDialog);
}

/* functions to update interface*/

#if TEST_DIRECT_EDJE


void
AddPartToTree2(char *part_name)//, char *group_name)//, int place_after, Engrave_Part* after)
{
   Etk_Tree_Row *row=NULL;
   char buf[20];

   //printf("Add Part to tree: %s\n",par->name);

   switch (edje_edit_part_type_get(edje_o ,part_name)){
      case EDJE_PART_TYPE_IMAGE:     strcpy(buf,"IMAGE.PNG"); break;
      case EDJE_PART_TYPE_TEXT:      strcpy(buf,"TEXT.PNG");  break;
      case EDJE_PART_TYPE_RECTANGLE: strcpy(buf,"RECT.PNG");  break;
      default:                       strcpy(buf,"NONE.PNG");  break;
   }
   
 /*  if (place_after)
      row = etk_tree_row_insert(ETK_TREE(UI_PartsTree),
                                ecore_hash_get(hash,part->parent),
                                ecore_hash_get(hash,after),
                                COL_NAME, EdjeFile,buf, part->name,
                                COL_TYPE,ROW_PART,
                                NULL);
   else*/
      row = etk_tree_row_append(ETK_TREE(UI_PartsTree),
                                NULL,
                                COL_NAME, EdjeFile,buf, part_name,
                                COL_TYPE, ROW_PART,
                                NULL);

   Parts_Hash = evas_hash_add(Parts_Hash, part_name, row);
   //etk_tree_row_data_set(row, part_name);
}

void
AddStateToTree2(char *part_name, char *state_name)
{
   Etk_Tree_Row *row;

   const char *stock_key;

   stock_key = etk_stock_key_get(ETK_STOCK_TEXT_X_GENERIC, ETK_STOCK_SMALL);
   row = etk_tree_row_append(ETK_TREE(UI_PartsTree),
            evas_hash_find(Parts_Hash,part_name),
            COL_NAME, EdjeFile, "DESC.PNG", state_name,
            COL_VIS, TRUE,
            COL_TYPE, ROW_DESC,
            COL_PARENT, part_name, NULL);
}
#endif

void
AddGroupToTree(Engrave_Group* group)
{
   Etk_Tree_Row *row=NULL;

   row = etk_tree_row_append(ETK_TREE(UI_PartsTree), NULL,
            COL_NAME, EdjeFile,"NONE.PNG", group->name,
            COL_TYPE,ROW_GROUP,  NULL);

   ecore_hash_set (hash, group, row);
   etk_tree_row_data_set(row,group);
}

void
AddPartToTree(Engrave_Part* part, int place_after, Engrave_Part* after)
{
   Etk_Tree_Row *row=NULL;
   char buf[20];

   //printf("Add Part to tree: %s\n",par->name);


   switch (part->type){
      case ENGRAVE_PART_TYPE_IMAGE: strcpy(buf,"IMAGE.PNG"); break;
      case ENGRAVE_PART_TYPE_TEXT:  strcpy(buf,"TEXT.PNG");  break;
      case ENGRAVE_PART_TYPE_RECT:  strcpy(buf,"RECT.PNG");  break;
      default:                      strcpy(buf,"NONE.PNG");  break;
   }
   
   if (place_after)
      row = etk_tree_row_insert(ETK_TREE(UI_PartsTree),
                                ecore_hash_get(hash,part->parent),
                                ecore_hash_get(hash,after),
                                COL_NAME, EdjeFile,buf, part->name,
                                COL_TYPE,ROW_PART,
                                NULL);
   else
      row = etk_tree_row_append(ETK_TREE(UI_PartsTree),
                                ecore_hash_get(hash,part->parent),
                                COL_NAME, EdjeFile,buf, part->name,
                                COL_TYPE,ROW_PART,
                                NULL);

   ecore_hash_set(hash, part, row);
   etk_tree_row_data_set(row, part);
}

void
AddStateToTree(Engrave_Part_State* state)
{
   Etk_Tree_Row *row;
   Etk_String *str = etk_string_new("");
   char buf[4096];
   const char *stock_key;
   
   snprintf(buf,4096,"%s %.2f",state->name,state->value);
   stock_key = etk_stock_key_get(ETK_STOCK_TEXT_X_GENERIC, ETK_STOCK_SMALL);
   row = etk_tree_row_append(ETK_TREE(UI_PartsTree),
            ecore_hash_get(hash,state->parent),
            COL_NAME,EdjeFile,"DESC.PNG",buf,
            COL_VIS,TRUE,
            COL_TYPE,ROW_DESC, NULL);

   etk_tree_row_data_set (row, state);
   ecore_hash_set(hash, state, row);


   etk_object_destroy(ETK_OBJECT(str));

}

void
AddProgramToTree(Engrave_Program* prog)
{
   Etk_Tree_Row *row=NULL;

   //printf("Add Program to tree: %s\n",prog->name);
   
   //TODO: place the prog after all the parts
   row = etk_tree_row_append(ETK_TREE(UI_PartsTree),
               ecore_hash_get(hash,prog->parent),
               COL_NAME, EdjeFile,"PROG.PNG", prog->name,
               COL_TYPE,ROW_PROG,
               NULL);

   ecore_hash_set(hash, prog, row);
   etk_tree_row_data_set(row, prog);
}
#if TEST_DIRECT_EDJE
void 
PopulateTree2(void)
{
   Evas_List *parts, *pp;
   Evas_List *states, *sp;
   
   etk_tree_clear(ETK_TREE(UI_PartsTree));
        
   parts = pp = edje_edit_parts_list_get(edje_o);
   while(pp)
   {
      printf("  P: %s\n", (char*)pp->data);
      AddPartToTree2((char*)pp->data);
      states = sp = edje_edit_part_states_list_get(edje_o, (char*)pp->data);
      while(sp)
      {
         AddStateToTree2((char*)pp->data, (char*)sp->data);
         sp = sp->next;
      }
      edje_edit_string_list_free(states);
      pp = pp->next;
   }
   edje_edit_string_list_free(parts);
   etk_tree_row_select(etk_tree_first_row_get (ETK_TREE(UI_PartsTree)));
}
void 
PopulateGroupsComboBox()
{
   Evas_List *groups;
   Etk_Combobox_Item *ComboItem;
   
   groups = edje_file_collection_list(Cur.edj_file_name->string);
   while(groups)
   {
      ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_GroupsComboBox), (char*)groups->data);
      groups = groups->next;
   }
   edje_file_collection_list_free  (groups);
   etk_combobox_active_item_set(ETK_COMBOBOX(UI_GroupsComboBox),
               etk_combobox_last_item_get (ETK_COMBOBOX(UI_GroupsComboBox)));
}
#else
void
PopulateTree(void)
{
   Engrave_Group *group;
   Engrave_Part *part;
   Engrave_Part_State *state;
   Engrave_Program *prog;
   Evas_List *gp,*pp,*sp,*progp;

   for (gp = Cur.ef->groups; gp ;gp = gp->next)
   {

      group = gp->data;
      AddGroupToTree(group);
      for (pp = group->parts; pp ;pp = pp->next)
      {
         part = pp->data;
         AddPartToTree(part, 0, NULL);
         for (sp = part->states; sp; sp = sp->next)
         {
            state = sp->data;
            AddStateToTree(state);
           // printf(" **       State: %s \n",sta->name);
         }
      }
      for (progp = group->programs; progp; progp = progp->next)
      {
            prog = progp->data;
            AddProgramToTree(prog);
      }
   }
}
#endif
void 
PopulateFontsComboBox(void)
{
   Evas_List *l;
   Engrave_Font *ef;
   Etk_Combobox_Item *ComboItem;

   //Stop signal propagation
   etk_signal_disconnect("active-item-changed", ETK_OBJECT(UI_FontComboBox), ETK_CALLBACK(on_FontComboBox_changed), NULL);

   printf("Populate Fonts Combo\n");

   etk_combobox_clear(ETK_COMBOBOX(UI_FontComboBox));

   for (l = Cur.ef->fonts; l ; l = l->next)
   {
      ef = l->data;
      ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_FontComboBox),
                     etk_image_new_from_stock (ETK_STOCK_PREFERENCES_DESKTOP_FONT,ETK_STOCK_SMALL), 
                     engrave_font_name_get (ef));
      etk_combobox_item_data_set (ComboItem, ef);
   }

   //Renable  signal propagation
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_FontComboBox), ETK_CALLBACK(on_FontComboBox_changed), NULL);

}

void
PopulateImagesComboBox(void)
{
   Engrave_Image *image;
   Evas_List *l;
   Etk_Combobox_Item *ComboItem;
   char buf[4096];


   //Stop signal propagation
   etk_signal_disconnect("active-item-changed", ETK_OBJECT(UI_ImageComboBox), ETK_CALLBACK(on_ImageComboBox_changed), NULL);

   printf("Populate Images Combobox\n");

   etk_combobox_clear(ETK_COMBOBOX(UI_ImageComboBox));
   
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_ImageComboBox),
                     NULL, "<b>Choose an image</b>");
   etk_combobox_item_data_set (ComboItem, NULL);
   
   for (l = Cur.ef->images; l ; l = l->next)
   {
      image = l->data;
      snprintf(buf,4095,"%s/%s",Cur.ef->im_dir,image->name);
      ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_ImageComboBox),
                     etk_image_new_from_file (buf, NULL), image->name);
      etk_combobox_item_data_set (ComboItem, image);
   }

   //Renable  signal propagation
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_ImageComboBox), ETK_CALLBACK(on_ImageComboBox_changed), NULL);
}

void
PopulateRelComboBoxes(void)
{
   Evas_List *l;
   Etk_Combobox_Item *ComboItem;
   Engrave_Part*	part = NULL;
   char buf[20];
   printf("Populate 4 Rel Comboboxs\n");
   //Stop signal propagation
   etk_signal_disconnect("active-item-changed", ETK_OBJECT(UI_Rel1ToXComboBox), ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL1X_SPINNER);
   etk_signal_disconnect("active-item-changed", ETK_OBJECT(UI_Rel1ToYComboBox), ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL1Y_SPINNER);
   etk_signal_disconnect("active-item-changed", ETK_OBJECT(UI_Rel2ToXComboBox), ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL2X_SPINNER);
   etk_signal_disconnect("active-item-changed", ETK_OBJECT(UI_Rel2ToYComboBox), ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL2Y_SPINNER);


   etk_combobox_clear(ETK_COMBOBOX(UI_Rel1ToXComboBox));
   etk_combobox_clear(ETK_COMBOBOX(UI_Rel1ToYComboBox));
   etk_combobox_clear(ETK_COMBOBOX(UI_Rel2ToXComboBox));
   etk_combobox_clear(ETK_COMBOBOX(UI_Rel2ToYComboBox));
#if TEST_DIRECT_EDJE
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
      // Add all the part to all the 4 combobox
      Evas_List *parts;
      int type;
      
      parts = l = edje_edit_parts_list_get(edje_o);
      while (l)
      {
         printf("-- %s\n", (char *)l->data);
         type = edje_edit_part_type_get(edje_o,(char *)l->data);
         
         if (type == EDJE_PART_TYPE_RECTANGLE)
            snprintf(buf, 19,"RECT.PNG");
         else if (type == EDJE_PART_TYPE_TEXT)
            snprintf(buf, 19,"TEXT.PNG");
         else if (type == EDJE_PART_TYPE_IMAGE)
            snprintf(buf, 19,"IMAGE.PNG");
         else snprintf(buf, 19,"NONE.PNG");
         
         etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToXComboBox),
                                  etk_image_new_from_edje (EdjeFile,buf),
                                  (char *)l->data);
         etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToYComboBox),
                                  etk_image_new_from_edje (EdjeFile,buf),
                                  (char *)l->data);
         etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToXComboBox),
                                  etk_image_new_from_edje (EdjeFile,buf),
                                  (char *)l->data);
         etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToYComboBox),
                                  etk_image_new_from_edje (EdjeFile,buf),
                                  (char *)l->data);
         
         l = l->next;
      }
      edje_edit_string_list_free(parts);
   }
#else
   if (Cur.eg)
   {
      ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToXComboBox), etk_image_new_from_edje (EdjeFile,"NONE.PNG"),"Interface");
      etk_combobox_item_data_set (ComboItem, (void *)REL_COMBO_INTERFACE);
      ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToYComboBox),etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Interface");
      etk_combobox_item_data_set (ComboItem, (void *)REL_COMBO_INTERFACE);
      ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToXComboBox), etk_image_new_from_edje (EdjeFile,"NONE.PNG"),"Interface");
      etk_combobox_item_data_set (ComboItem, (void *)REL_COMBO_INTERFACE);
      ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToYComboBox),etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Interface");
      etk_combobox_item_data_set (ComboItem, (void *)REL_COMBO_INTERFACE);
      for (l = Cur.eg->parts; l; l = l->next)
      {
         part = l->data;
         if (part->type == ENGRAVE_PART_TYPE_RECT)
            snprintf(buf, 19,"RECT.PNG");
         if (part->type == ENGRAVE_PART_TYPE_TEXT)
            snprintf(buf, 19,"TEXT.PNG");
         if (part->type == ENGRAVE_PART_TYPE_IMAGE)
            snprintf(buf, 19,"IMAGE.PNG");

         ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToXComboBox),etk_image_new_from_edje (EdjeFile,buf), part->name);
         etk_combobox_item_data_set (ComboItem, part);
         ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToYComboBox),etk_image_new_from_edje (EdjeFile,buf), part->name);
         etk_combobox_item_data_set (ComboItem, part);
         ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToXComboBox),etk_image_new_from_edje (EdjeFile,buf), part->name);
         etk_combobox_item_data_set (ComboItem, part);
         ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToYComboBox), etk_image_new_from_edje (EdjeFile,buf),part->name);
         etk_combobox_item_data_set (ComboItem, part);
      }
   }
#endif
   //Reenable signal propagation
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_Rel1ToXComboBox), ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL1X_SPINNER);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_Rel1ToYComboBox), ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL1Y_SPINNER);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_Rel2ToXComboBox), ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL2X_SPINNER);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_Rel2ToYComboBox), ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL2Y_SPINNER);
}

void
PopulateSourceComboBox(void)
{
   Evas_List *l;
   Engrave_Part *ep;

   //Stop signal propagation
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_SourceEntry),
                  ETK_CALLBACK(on_SourceEntry_item_changed), NULL);

   printf("Populate Source Combobox\n");

   etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(UI_SourceEntry));

   for (l = Cur.eg->parts; l ; l = l->next)
   {
      ep = l->data;
      if (ep->type == ENGRAVE_PART_TYPE_RECT)
         etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SourceEntry),
                     etk_image_new_from_edje (EdjeFile,"RECT.PNG"),ep->name);
      if (ep->type == ENGRAVE_PART_TYPE_TEXT)
         etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SourceEntry),
                     etk_image_new_from_edje (EdjeFile,"TEXT.PNG"),ep->name);
      if (ep->type == ENGRAVE_PART_TYPE_IMAGE)
          etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(UI_SourceEntry),
                     etk_image_new_from_edje (EdjeFile,"IMAGE.PNG"),ep->name);
   }

   //Renable  signal propagation
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_SourceEntry),
                     ETK_CALLBACK(on_SourceEntry_item_changed), NULL);
}

void
PupulateTweenList(void)
{
   char buf[4096];
   Engrave_Image *im;
   Etk_Tree_Row *row;
   Evas_List *l;
   int row_num = 1;
   Etk_Tree_Col *col1 = etk_tree_nth_col_get (ETK_TREE(UI_ImageTweenList),0);
   
   etk_tree_clear(ETK_TREE(UI_ImageTweenList));
   if (Cur.eps->image.normal)
   {
      snprintf(buf,4095,"%s/%s",Cur.ef->im_dir,Cur.eps->image.normal->name);
      etk_tree_row_append(ETK_TREE(UI_ImageTweenList), NULL,
                           col1, buf, NULL,Cur.eps->image.normal->name,
                           NULL);
   }
   // for all the frame in the tween
   for (l = Cur.eps->image.tween; l; l = l->next)
   {
      im = l->data;
      snprintf(buf,4095,"%s/%s",Cur.ef->im_dir,im->name);
      row = etk_tree_row_append(ETK_TREE(UI_ImageTweenList), NULL,
                           col1, buf, NULL,im->name,
                           NULL);
      etk_tree_row_data_set(row,(void*)row_num);
      row_num++;
   }
   etk_widget_disabled_set(UI_DeleteTweenButton,TRUE);
   etk_widget_disabled_set(UI_MoveUpTweenButton,TRUE);
   etk_widget_disabled_set(UI_MoveDownTweenButton,TRUE);
}

void
UpdateGroupFrame(void)
{
   //Stop signal propagation
   etk_signal_block("text-changed",ETK_OBJECT(UI_GroupNameEntry),on_GroupNameEntry_text_changed, NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GroupMinWSpinner), ETK_CALLBACK(on_GroupSpinner_value_changed), (void *)MINW_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GroupMinHSpinner), ETK_CALLBACK(on_GroupSpinner_value_changed), (void *)MINH_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GroupMaxWSpinner), ETK_CALLBACK(on_GroupSpinner_value_changed), (void *)MAXW_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_GroupMaxHSpinner), ETK_CALLBACK(on_GroupSpinner_value_changed), (void *)MAXH_SPINNER);

   //Update name
   etk_entry_text_set(ETK_ENTRY(UI_GroupNameEntry),Cur.eg->name);

   //Update min e max spinners
   etk_range_value_set (ETK_RANGE(UI_GroupMinWSpinner), Cur.eg->min.w);
   etk_range_value_set (ETK_RANGE(UI_GroupMinHSpinner), Cur.eg->min.h);
   etk_range_value_set (ETK_RANGE(UI_GroupMaxWSpinner), Cur.eg->max.w);
   etk_range_value_set (ETK_RANGE(UI_GroupMaxHSpinner), Cur.eg->max.h);

   //ReEnable Signal Propagation
   etk_signal_unblock("text-changed",ETK_OBJECT(UI_GroupNameEntry),on_GroupNameEntry_text_changed, NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GroupMinWSpinner), ETK_CALLBACK(on_GroupSpinner_value_changed), (void *)MINW_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GroupMinHSpinner), ETK_CALLBACK(on_GroupSpinner_value_changed), (void *)MINH_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GroupMaxWSpinner), ETK_CALLBACK(on_GroupSpinner_value_changed), (void *)MAXW_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_GroupMaxHSpinner), ETK_CALLBACK(on_GroupSpinner_value_changed), (void *)MAXH_SPINNER);

}

void
UpdatePartFrame(void)
{
   if (Cur.ep){
      //printf("Update Part Frame: %s\n",Cur.ep->name);

      //Stop signal propagation
      etk_signal_block("text-changed",ETK_OBJECT(UI_PartNameEntry),
                        on_PartNameEntry_text_changed, NULL);
      etk_signal_block("toggled",ETK_OBJECT(UI_PartEventsCheck),
                        on_PartEventsCheck_toggled, NULL);

      //Update Part
      etk_entry_text_set(ETK_ENTRY(UI_PartNameEntry),Cur.ep->name);
      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(UI_PartEventsCheck),
                                    Cur.ep->mouse_events);	 

      //ReEnable Signal Propagation
      etk_signal_unblock("text-changed",ETK_OBJECT(UI_PartNameEntry),
                           on_PartNameEntry_text_changed, NULL);
      etk_signal_unblock("toggled",ETK_OBJECT(UI_PartEventsCheck),
                           on_PartEventsCheck_toggled, NULL);
   }
}

void
UpdateDescriptionFrame(void)
{
   if (Cur.eps)
   {
      const char *name;
      double index;
      double aspect_min;
      double aspect_max;
      int minw,minh,maxw,maxh;
      double h, v;

      //printf("Update Description Frame: %s\n",Cur.eps->name);

      //Stop signal propagation
      etk_signal_block("text-changed",ETK_OBJECT(UI_StateEntry),on_StateEntry_text_changed, NULL);
      etk_signal_block("value-changed", ETK_OBJECT(UI_StateIndexSpinner), ETK_CALLBACK(on_StateIndexSpinner_value_changed), NULL);
      etk_signal_block("value-changed", ETK_OBJECT(UI_AspectMinSpinner), ETK_CALLBACK(on_AspectSpinner_value_changed), NULL);
      etk_signal_block("value-changed", ETK_OBJECT(UI_AspectMaxSpinner), ETK_CALLBACK(on_AspectSpinner_value_changed), NULL);
      etk_signal_block("active-item-changed", ETK_OBJECT(UI_AspectComboBox), ETK_CALLBACK(on_AspectComboBox_changed), NULL);
      etk_signal_block("value-changed",ETK_OBJECT(UI_StateMinWSpinner),ETK_CALLBACK(on_StateMinMaxSpinner_value_changed), NULL);
      etk_signal_block("value-changed",ETK_OBJECT(UI_StateMinHSpinner),ETK_CALLBACK(on_StateMinMaxSpinner_value_changed), NULL);
      etk_signal_block("value-changed",ETK_OBJECT(UI_StateMaxWSpinner),ETK_CALLBACK(on_StateMinMaxSpinner_value_changed), NULL);
      etk_signal_block("value-changed",ETK_OBJECT(UI_StateMaxHSpinner),ETK_CALLBACK(on_StateMinMaxSpinner_value_changed), NULL);

      //Set description name & index
      name = engrave_part_state_name_get(Cur.eps,&index);
      etk_entry_text_set(ETK_ENTRY(UI_StateEntry),name);
      etk_range_value_set(ETK_RANGE(UI_StateIndexSpinner), index);

      //Set aspect min & max
      engrave_part_state_aspect_get(Cur.eps, &aspect_min, &aspect_max);
      etk_range_value_set(ETK_RANGE(UI_AspectMinSpinner), aspect_min);
      etk_range_value_set(ETK_RANGE(UI_AspectMaxSpinner), aspect_max);

      //Set aspect pref Combo
      etk_combobox_active_item_set(ETK_COMBOBOX(UI_AspectComboBox),
         etk_combobox_nth_item_get (ETK_COMBOBOX(UI_AspectComboBox), 
            engrave_part_state_aspect_preference_get(Cur.eps)));

      //Set min e max size
      engrave_part_state_min_size_get(Cur.eps, &minw, &minh);
      engrave_part_state_max_size_get(Cur.eps, &maxw, &maxh);
      etk_range_value_set(ETK_RANGE(UI_StateMinWSpinner), minw);
      etk_range_value_set(ETK_RANGE(UI_StateMinHSpinner), minh);
      etk_range_value_set(ETK_RANGE(UI_StateMaxWSpinner), maxw);
      etk_range_value_set(ETK_RANGE(UI_StateMaxHSpinner), maxh);
      
      engrave_part_state_align_get(Cur.eps, &h, &v);
      etk_range_value_set (ETK_RANGE(UI_StateAlignHSpinner), h);
      etk_range_value_set (ETK_RANGE(UI_StateAlignVSpinner), v);

      //ReEnable Signal Propagation
      etk_signal_unblock("text-changed",ETK_OBJECT(UI_StateEntry),on_StateEntry_text_changed, NULL);
      etk_signal_unblock("value-changed", ETK_OBJECT(UI_StateIndexSpinner), ETK_CALLBACK(on_StateIndexSpinner_value_changed), NULL);
      etk_signal_unblock("value-changed", ETK_OBJECT(UI_AspectMinSpinner), ETK_CALLBACK(on_AspectSpinner_value_changed), NULL);
      etk_signal_unblock("value-changed", ETK_OBJECT(UI_AspectMaxSpinner), ETK_CALLBACK(on_AspectSpinner_value_changed), NULL);
      etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_AspectComboBox), ETK_CALLBACK(on_AspectComboBox_changed), NULL);
      etk_signal_unblock("value-changed",ETK_OBJECT(UI_StateMinWSpinner),ETK_CALLBACK(on_StateMinMaxSpinner_value_changed), NULL);
      etk_signal_unblock("value-changed",ETK_OBJECT(UI_StateMinHSpinner),ETK_CALLBACK(on_StateMinMaxSpinner_value_changed), NULL);
      etk_signal_unblock("value-changed",ETK_OBJECT(UI_StateMaxWSpinner),ETK_CALLBACK(on_StateMinMaxSpinner_value_changed), NULL);
      etk_signal_unblock("value-changed",ETK_OBJECT(UI_StateMaxHSpinner),ETK_CALLBACK(on_StateMinMaxSpinner_value_changed), NULL);
   }
}

void
UpdateRectFrame(void)
{
   Etk_Color color;

   if (Cur.eps){
      printf("Update Rect Frame: %s (%d %d %d %d)\n",Cur.eps->name,Cur.eps->color.r,Cur.eps->color.g,Cur.eps->color.b,Cur.eps->color.a);

      etk_signal_block("color-changed", ETK_OBJECT(UI_ColorPicker), ETK_CALLBACK(on_ColorDialog_change), NULL);

      //Set ColorPicker
      color.r = Cur.eps->color.r;
      color.g = Cur.eps->color.g;
      color.b = Cur.eps->color.b;
      color.a = Cur.eps->color.a;
      etk_colorpicker_current_color_set (ETK_COLORPICKER(UI_ColorPicker), color);

      //Set Color rect
      evas_color_argb_premul(color.a,&color.r,&color.g,&color.b);
      evas_object_color_set(RectColorObject,color.r,color.g,color.b,color.a);

      etk_signal_unblock("color-changed", ETK_OBJECT(UI_ColorPicker), ETK_CALLBACK(on_ColorDialog_change), NULL);
   }
}

void
UpdateImageFrame(void)
{
   Engrave_Image *image;
   int i;
   Etk_Combobox_Item *item = NULL;
    
   //Stop signal propagation
   etk_signal_block("value-changed",ETK_OBJECT(UI_BorderLeftSpinner),ETK_CALLBACK(on_BorderSpinner_value_changed), (void*)BORDER_LEFT);
   etk_signal_block("value-changed",ETK_OBJECT(UI_BorderRightSpinner),ETK_CALLBACK(on_BorderSpinner_value_changed), (void*)BORDER_RIGHT);
   etk_signal_block("value-changed",ETK_OBJECT(UI_BorderTopSpinner),ETK_CALLBACK(on_BorderSpinner_value_changed), (void*)BORDER_TOP);
   etk_signal_block("value-changed",ETK_OBJECT(UI_BorderBottomSpinner),ETK_CALLBACK(on_BorderSpinner_value_changed), (void*)BORDER_BOTTOM);
   etk_signal_block("value-changed",ETK_OBJECT(UI_ImageAlphaSlider),ETK_CALLBACK(on_ImageAlphaSlider_value_changed), NULL);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_ImageComboBox), ETK_CALLBACK(on_ImageComboBox_changed), NULL);

   PupulateTweenList();

   if (Cur.eps->image.tween)
   {
      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(UI_ImageTweenRadio), TRUE);
      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(UI_ImageNormalRadio), FALSE);
      etk_widget_show(UI_ImageTweenList);
      etk_widget_show(UI_MoveUpTweenButton);
      etk_widget_show(UI_MoveDownTweenButton);
      etk_widget_show(UI_DeleteTweenButton);
      etk_combobox_active_item_set (ETK_COMBOBOX(UI_ImageComboBox),
               etk_combobox_nth_item_get(ETK_COMBOBOX(UI_ImageComboBox),0));
   }else{
      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(UI_ImageTweenRadio), FALSE);
      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(UI_ImageNormalRadio), TRUE);
      etk_widget_hide(UI_ImageTweenList);
      etk_widget_hide(UI_MoveUpTweenButton);
      etk_widget_hide(UI_MoveDownTweenButton);
      etk_widget_hide(UI_DeleteTweenButton);
      //Set the images combobox for normal image
      if (Cur.eps->image.normal)
      {
         i=0;
         //Loop for all the item in the Combobox
         while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_ImageComboBox),i)))
         {
            if ((image = etk_combobox_item_data_get(item)))
            {
               //Get the data for the item (should be an char* with the name of the image file)
               if (image == Cur.eps->image.normal)
               {
                  etk_combobox_active_item_set (ETK_COMBOBOX(UI_ImageComboBox),item);	//If we found the item set active
                  break;
               }
            }
            i++;
         }
      }else{
         etk_combobox_active_item_set (ETK_COMBOBOX(UI_ImageComboBox),
               etk_combobox_nth_item_get(ETK_COMBOBOX(UI_ImageComboBox),0));
      }
   }

   etk_range_value_set (ETK_RANGE(UI_BorderTopSpinner), Cur.eps->image.border.t);
   etk_range_value_set (ETK_RANGE(UI_BorderLeftSpinner), Cur.eps->image.border.l);
   etk_range_value_set (ETK_RANGE(UI_BorderRightSpinner), Cur.eps->image.border.r);
   etk_range_value_set (ETK_RANGE(UI_BorderBottomSpinner), Cur.eps->image.border.b);
   etk_range_value_set (ETK_RANGE(UI_ImageAlphaSlider), Cur.eps->color.a);

   //ReEnable Signal Propagation
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_BorderLeftSpinner), on_BorderSpinner_value_changed, (void*)BORDER_LEFT);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_BorderRightSpinner), on_BorderSpinner_value_changed, (void*)BORDER_RIGHT);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_BorderTopSpinner), on_BorderSpinner_value_changed, (void*)BORDER_TOP);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_BorderBottomSpinner), on_BorderSpinner_value_changed, (void*)BORDER_BOTTOM);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_ImageAlphaSlider), on_ImageAlphaSlider_value_changed, NULL);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_ImageComboBox), ETK_CALLBACK(on_ImageComboBox_changed), NULL);

}

void
UpdateTextFrame(void)
{
   int eff_num = 0;
   int i;
   double h, v;
   //int alpha;
   int r, g, b;
   Etk_Combobox_Item *item = NULL;
   Engrave_Font *ef;

   if (Cur.eps){
      printf("Update Text Frame: %s\n",Cur.eps->name);

      //Stop signal propagation
      etk_signal_block("text-changed",ETK_OBJECT(UI_TextEntry),on_TextEntry_text_changed, NULL);
      etk_signal_block("active-item-changed", ETK_OBJECT(UI_FontComboBox), ETK_CALLBACK(on_FontComboBox_changed), NULL);

      //Set Text Text in Cur.eps
      etk_entry_text_set(ETK_ENTRY(UI_TextEntry), 
         engrave_part_state_text_text_get(Cur.eps));


      //Set the font combobox
      if (Cur.eps->text.font){//selected_desc->text_font->len > 0){
         i=0;
         //Loop for all the item in the Combobox
         while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_FontComboBox),i))){
            //Get the data for the item (should be an Engrave_font*)         
            if ((ef = etk_combobox_item_data_get(item))){
               printf("ITEM: %s - %s\n",engrave_font_name_get(ef),engrave_part_state_text_font_get(Cur.eps));
               if (strcmp(engrave_font_name_get(ef), Cur.eps->text.font) == 0)
                  etk_combobox_active_item_set (ETK_COMBOBOX(UI_FontComboBox),item);	//If we found the item set active
            }
            i++;
         }
      }else{etk_combobox_active_item_set (ETK_COMBOBOX(UI_FontComboBox), etk_combobox_nth_item_get(ETK_COMBOBOX(UI_FontComboBox),0));}

      //Set the font size spinner
      etk_range_value_set (ETK_RANGE(UI_FontSizeSpinner), 
         (float)engrave_part_state_text_size_get(Cur.eps));
      
      //Set the font align spinners
      engrave_part_state_text_align_get(Cur.eps, &h, &v);
      etk_range_value_set (ETK_RANGE(UI_FontAlignHSpinner), h);
      etk_range_value_set (ETK_RANGE(UI_FontAlignVSpinner), v);

      //Set Effect ComboBox
      switch (Cur.ep->effect)
      {
         case ENGRAVE_TEXT_EFFECT_NONE: eff_num = 0; break;
         case ENGRAVE_TEXT_EFFECT_PLAIN: eff_num = 0; break;
         case ENGRAVE_TEXT_EFFECT_OUTLINE: eff_num = 1; break;
         case ENGRAVE_TEXT_EFFECT_SOFT_OUTLINE: eff_num = 2; break;
         case ENGRAVE_TEXT_EFFECT_SHADOW: eff_num = 3; break;
         case ENGRAVE_TEXT_EFFECT_SOFT_SHADOW: eff_num = 4; break;
         case ENGRAVE_TEXT_EFFECT_OUTLINE_SHADOW: eff_num = 5; break;
         case ENGRAVE_TEXT_EFFECT_OUTLINE_SOFT_SHADOW: eff_num = 6; break;
         case ENGRAVE_TEXT_EFFECT_FAR_SHADOW: eff_num = 7; break;
         case ENGRAVE_TEXT_EFFECT_FAR_SOFT_SHADOW: eff_num = 8; break;
         case ENGRAVE_TEXT_EFFECT_GLOW: eff_num = 9; break;
         default: break; // remove warning
      }
      etk_combobox_active_item_set (ETK_COMBOBOX(UI_EffectComboBox),
         etk_combobox_nth_item_get (ETK_COMBOBOX(UI_EffectComboBox), eff_num));

      //Set Text color Rects
      engrave_part_state_color_get(Cur.eps,&r,&g,&b,NULL);
      evas_object_color_set(TextColorObject, r, g, b, 255);
      engrave_part_state_color2_get(Cur.eps,&r,&g,&b,NULL);
      evas_object_color_set(ShadowColorObject, r, g, b, 255);
      engrave_part_state_color3_get(Cur.eps,&r,&g,&b,NULL);
      evas_object_color_set(OutlineColorObject, r, g, b, 255);

      //Renable  signal propagation
      etk_signal_unblock("text-changed", ETK_OBJECT(UI_TextEntry), ETK_CALLBACK(on_TextEntry_text_changed), NULL);
      etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_FontComboBox), ETK_CALLBACK(on_FontComboBox_changed), NULL);
   }
}

void
UpdatePositionFrame(void)
{
   //printf("Update Position: %s (offset: %d)\n",Cur.eps->name,Cur.eps->rel1.offset.x);
   //Stop signal propagation
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel1XSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void*)REL1X_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel1YSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void*)REL1Y_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel2XSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void*)REL2X_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel2YSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void*)REL2Y_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel1XOffsetSpinner), ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void*)REL1X_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel1YOffsetSpinner), ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void*)REL1Y_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel2XOffsetSpinner), ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void*)REL2X_SPINNER);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Rel2YOffsetSpinner), ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void*)REL2Y_SPINNER);

#if TEST_DIRECT_EDJE
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
   
   etk_range_value_set (ETK_RANGE(UI_Rel1XOffsetSpinner),
         edje_edit_state_rel1_offset_x_get(edje_o, Cur.part->string,Cur.state->string));
   etk_range_value_set (ETK_RANGE(UI_Rel1YOffsetSpinner),
         edje_edit_state_rel1_offset_y_get(edje_o, Cur.part->string,Cur.state->string));
   etk_range_value_set (ETK_RANGE(UI_Rel2XOffsetSpinner),
         edje_edit_state_rel2_offset_x_get(edje_o, Cur.part->string,Cur.state->string));
   etk_range_value_set (ETK_RANGE(UI_Rel2YOffsetSpinner),
         edje_edit_state_rel2_offset_y_get(edje_o, Cur.part->string,Cur.state->string));
#else
   //Set relative position spinners
   etk_range_value_set (ETK_RANGE(UI_Rel1XSpinner), Cur.eps->rel1.relative.x);
   etk_range_value_set (ETK_RANGE(UI_Rel1XOffsetSpinner), Cur.eps->rel1.offset.x);
   etk_range_value_set (ETK_RANGE(UI_Rel1YSpinner), Cur.eps->rel1.relative.y);
   etk_range_value_set (ETK_RANGE(UI_Rel1YOffsetSpinner), Cur.eps->rel1.offset.y);
   etk_range_value_set (ETK_RANGE(UI_Rel2XSpinner), Cur.eps->rel2.relative.x);
   etk_range_value_set (ETK_RANGE(UI_Rel2XOffsetSpinner), Cur.eps->rel2.offset.x);
   etk_range_value_set (ETK_RANGE(UI_Rel2YSpinner), Cur.eps->rel2.relative.y);
   etk_range_value_set (ETK_RANGE(UI_Rel2YOffsetSpinner), Cur.eps->rel2.offset.y);
#endif
   //Reenable signal propagation
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel1XSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void*)REL1X_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel1YSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void*)REL1Y_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel2XSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void*)REL2X_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel2YSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void*)REL2Y_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel1XOffsetSpinner), ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void*)REL1X_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel1YOffsetSpinner), ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void*)REL1Y_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel2XOffsetSpinner), ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void*)REL2X_SPINNER);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Rel2YOffsetSpinner), ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void*)REL2Y_SPINNER);
}

void
UpdateComboPositionFrame(void)
{
   int i=0;
   Etk_Combobox_Item *item = NULL;
   Engrave_Part *part = NULL;
   
   printf("SETTING COMBOS\n");
   
   //Stop signal propagation
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_Rel1ToXComboBox),
         ETK_CALLBACK(on_RelToComboBox_changed), (void*)REL1X_SPINNER);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_Rel1ToYComboBox),
         ETK_CALLBACK(on_RelToComboBox_changed), (void*)REL1Y_SPINNER);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_Rel2ToXComboBox),
         ETK_CALLBACK(on_RelToComboBox_changed), (void*)REL2X_SPINNER);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_Rel2ToYComboBox),
         ETK_CALLBACK(on_RelToComboBox_changed), (void*)REL2Y_SPINNER);
#if TEST_DIRECT_EDJE
   if (!etk_string_length_get(Cur.part)) return;
   if (!etk_string_length_get(Cur.state)) return;
   char *rel;
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
            etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel1ToXComboBox),item);
         i++;
      }
      edje_edit_string_free(rel);
   }
   else
      etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel1ToXComboBox), 
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
            etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel1ToYComboBox),item);
         i++;
      }
      edje_edit_string_free(rel);
   }
   else
      etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel1ToYComboBox), 
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
            etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel2ToXComboBox),item);
         i++;
      }
      edje_edit_string_free(rel);
   }
   else
      etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel2ToXComboBox), 
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
            etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel2ToYComboBox),item);
         i++;
      }
      edje_edit_string_free(rel);
   }
   else
      etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel2ToYComboBox), 
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToYComboBox),0));
   
#else
   //If rel1_to_x is know set the combobox
   if (Cur.eps->rel1.to_x)
   {
      i=0;
      while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel1ToXComboBox),i)))
      {
         //Loop for all the item in the Combobox
         if ((part = etk_combobox_item_data_get(item)))
         {
            //Get the data for the item (should be an Engrave_Part*)
            if ((int)part != REL_COMBO_INTERFACE)
               if (strcmp(part->name,Cur.eps->rel1.to_x) == 0)
                  //If we found the item set active
                  etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel1ToXComboBox),item);
         }
         i++;
      }
   }else{etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel1ToXComboBox), 
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel1ToXComboBox),0));}

   //If rel1_to_y is know set the combobox
   if (Cur.eps->rel1.to_y)
   {
      i=0;
      while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel1ToYComboBox),i)))
      {
         //Loop for all the item in the Combobox
         if ((part = etk_combobox_item_data_get(item)))
         {
            //Get the data for the item (should be an Engrave_part*)
            if ((int)part != REL_COMBO_INTERFACE)
               if (strcmp(part->name,Cur.eps->rel1.to_y) == 0)
                  //If we found the item set active
                  etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel1ToYComboBox),item);
         }
         i++;
      }
   }else{etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel1ToYComboBox), 
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel1ToYComboBox),0));}

   //If rel2_to_x is know set the combobox
   if (Cur.eps->rel2.to_x)
   {
      i=0;
      while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToXComboBox),i)))
      {
         //Loop for all the item in the Combobox
         if ((part = etk_combobox_item_data_get(item)))
         {
            //Get the data for the item (should be an Engrave_part*)
            if ((int)part != REL_COMBO_INTERFACE)
               if (strcmp(part->name,Cur.eps->rel2.to_x) == 0)
                  //If we found the item set active
                  etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel2ToXComboBox),item);
         }
         i++;
      }
   }else{etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel2ToXComboBox), 
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToXComboBox),0));}

   //If rel2_to_y is know set the combobox
   if (Cur.eps->rel2.to_y)
   {
      i=0;
      while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToYComboBox),i)))
      {
         //Loop for all the item in the Combobox
         if ((part = etk_combobox_item_data_get(item)))
         {
            //Get the data for the item (should be an Engrave_part*)
            if ((int)part != REL_COMBO_INTERFACE)
               if (strcmp(part->name,Cur.eps->rel2.to_y) == 0)
                  //If we found the item set active
                  etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel2ToYComboBox),item);
         }
         i++;
      }
   }else{etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel2ToYComboBox), 
         etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToYComboBox),0));}
#endif
   //Reenable signal propagation
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_Rel1ToXComboBox),
         ETK_CALLBACK(on_RelToComboBox_changed), (void*)REL1X_SPINNER);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_Rel1ToYComboBox),
         ETK_CALLBACK(on_RelToComboBox_changed), (void*)REL1Y_SPINNER);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_Rel2ToXComboBox),
         ETK_CALLBACK(on_RelToComboBox_changed), (void*)REL2X_SPINNER);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_Rel2ToYComboBox),
         ETK_CALLBACK(on_RelToComboBox_changed), (void*)REL2Y_SPINNER);

}

void
UpdateProgFrame(void)
{
   if (!Cur.epr)
      return;
   
   printf("Update Program: %s\n",Cur.epr->name);

   //Stop signal propagation
   etk_signal_block("text-changed", ETK_OBJECT(UI_ProgramEntry),
         ETK_CALLBACK(on_ProgramEntry_text_changed), NULL);
   etk_signal_block("text-changed",
         ETK_OBJECT(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SourceEntry))),
         ETK_CALLBACK(on_SourceEntry_text_changed), UI_SourceEntry);
   etk_signal_block("text-changed", ETK_OBJECT(UI_SignalEntry),
         ETK_CALLBACK(on_SignalEntry_text_changed), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_DelayFromSpinner),
         ETK_CALLBACK(on_DelaySpinners_value_changed), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_DelayRangeSpinner),
         ETK_CALLBACK(on_DelaySpinners_value_changed), NULL);
   etk_signal_block("text-changed", ETK_OBJECT(UI_TargetEntry),
         ETK_CALLBACK(on_TargetEntry_text_changed), NULL);
   etk_signal_block("text-changed", ETK_OBJECT(UI_Param1Entry),
         ETK_CALLBACK(on_Param1Entry_text_changed), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_Param1Spinner),
         ETK_CALLBACK(on_Param1Spinner_value_changed), NULL);
   etk_signal_block("active-item-changed", ETK_OBJECT(UI_TransiComboBox),
         ETK_CALLBACK(on_TransitionComboBox_changed), NULL);
   etk_signal_block("value-changed", ETK_OBJECT(UI_DurationSpinner),
         ETK_CALLBACK(on_DurationSpinner_value_changed), NULL);
   etk_signal_block("text-changed", ETK_OBJECT(UI_AfterEntry),
         ETK_CALLBACK(on_AfterEntry_text_changed), NULL);
   etk_signal_block("text-changed", ETK_OBJECT(UI_Param2Entry),
         ETK_CALLBACK(on_Param2Entry_text_changed), NULL);

         
   //Update Program
   etk_entry_text_set(ETK_ENTRY(UI_ProgramEntry),Cur.epr->name);

   //Update Source
   etk_entry_text_set(ETK_ENTRY(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SourceEntry))),
                      Cur.epr->source);
   
   //Update Signal
   etk_entry_text_set(ETK_ENTRY(UI_SignalEntry),Cur.epr->signal);

   //Update Delay
   etk_range_value_set (ETK_RANGE(UI_DelayFromSpinner), Cur.epr->in.from);
   etk_range_value_set (ETK_RANGE(UI_DelayRangeSpinner), Cur.epr->in.range);

   //Update Action
   if (Cur.epr->action == ENGRAVE_ACTION_SIGNAL_EMIT)
         etk_combobox_active_item_set (ETK_COMBOBOX(UI_ActionComboBox),
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_ActionComboBox),2));
   if (Cur.epr->action == ENGRAVE_ACTION_STATE_SET)
         etk_combobox_active_item_set (ETK_COMBOBOX(UI_ActionComboBox),
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_ActionComboBox),0));
   if (Cur.epr->action == ENGRAVE_ACTION_STOP)
      etk_combobox_active_item_set (ETK_COMBOBOX(UI_ActionComboBox),
         etk_combobox_nth_item_get(ETK_COMBOBOX(UI_ActionComboBox),1));

   //Update Target(s)
   Evas_List *tl;  //target list
   Etk_String *str=etk_string_new("");
   for (tl = Cur.epr->targets; tl; tl = tl->next)
   {
      str = etk_string_append(str,tl->data);
      str = etk_string_append(str,", ");
   }
   if (str->length > 2)
      etk_string_truncate (str, str->length - 2);
   etk_entry_text_set(ETK_ENTRY(UI_TargetEntry),str->string);

   //Update Param1/2
   etk_entry_text_set(ETK_ENTRY(UI_Param1Entry),Cur.epr->state);
   etk_entry_text_set(ETK_ENTRY(UI_Param2Entry),Cur.epr->state2);
   etk_range_value_set (ETK_RANGE(UI_Param1Spinner), Cur.epr->value);

   //Update Transition
    if (Cur.epr->transition== ENGRAVE_TRANSITION_LINEAR)
         etk_combobox_active_item_set (ETK_COMBOBOX(UI_TransiComboBox),
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_TransiComboBox),0));
   if (Cur.epr->transition == ENGRAVE_TRANSITION_SINUSOIDAL)
         etk_combobox_active_item_set (ETK_COMBOBOX(UI_TransiComboBox),
            etk_combobox_nth_item_get(ETK_COMBOBOX(UI_TransiComboBox),1));
   if (Cur.epr->transition == ENGRAVE_TRANSITION_ACCELERATE)
      etk_combobox_active_item_set (ETK_COMBOBOX(UI_TransiComboBox),
         etk_combobox_nth_item_get(ETK_COMBOBOX(UI_TransiComboBox),2));
   if (Cur.epr->transition == ENGRAVE_TRANSITION_DECELERATE)
      etk_combobox_active_item_set (ETK_COMBOBOX(UI_TransiComboBox),
         etk_combobox_nth_item_get(ETK_COMBOBOX(UI_TransiComboBox),3));
   etk_range_value_set (ETK_RANGE(UI_DurationSpinner), Cur.epr->duration);

   //Update Afters(s)
   etk_string_truncate(str,0);
   for (tl = Cur.epr->afters; tl; tl = tl->next)
   {
      str = etk_string_append(str,tl->data);
      str = etk_string_append(str,", ");
   }
   if (str->length > 2)
      etk_string_truncate (str, str->length - 2);
   etk_entry_text_set(ETK_ENTRY(UI_AfterEntry),str->string);
   etk_object_destroy(ETK_OBJECT(str));

   //Reenable signal propagation
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_ProgramEntry),
         ETK_CALLBACK(on_ProgramEntry_text_changed), NULL);
   etk_signal_unblock("text-changed",
         ETK_OBJECT(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SourceEntry))),
         ETK_CALLBACK(on_SourceEntry_text_changed), UI_SourceEntry);
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_SignalEntry),
         ETK_CALLBACK(on_SignalEntry_text_changed), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_DelayFromSpinner),
         ETK_CALLBACK(on_DelaySpinners_value_changed), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_DelayRangeSpinner),
         ETK_CALLBACK(on_DelaySpinners_value_changed), NULL);
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_TargetEntry),
         ETK_CALLBACK(on_TargetEntry_text_changed), NULL);
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_Param1Entry),
         ETK_CALLBACK(on_Param1Entry_text_changed), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_Param1Spinner),
         ETK_CALLBACK(on_Param1Spinner_value_changed), NULL);
   etk_signal_unblock("active-item-changed", ETK_OBJECT(UI_TransiComboBox),
         ETK_CALLBACK(on_TransitionComboBox_changed), NULL);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_DurationSpinner),
         ETK_CALLBACK(on_DurationSpinner_value_changed), NULL);
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_AfterEntry),
         ETK_CALLBACK(on_AfterEntry_text_changed), NULL);
   etk_signal_unblock("text-changed", ETK_OBJECT(UI_Param2Entry),
         ETK_CALLBACK(on_Param2Entry_text_changed), NULL);
}

void
UpdateScriptFrame(void)
{
   //Update ScriptBox
   if (Cur.epr){
      printf("Update prog script: %s\n",Cur.epr->script);
      etk_textblock_text_set(ETK_TEXT_VIEW(UI_ScriptBox)->textblock, 
                              engrave_program_script_get(Cur.epr), ETK_FALSE);
      return;
   }
   if (Cur.eg){
      printf("Update group script: %s\n",Cur.eg->script);
      etk_textblock_text_set (ETK_TEXT_VIEW(UI_ScriptBox)->textblock, 
                              engrave_group_script_get(Cur.eg), ETK_FALSE);
      return;
   }
   etk_textblock_clear(ETK_TEXT_VIEW(UI_ScriptBox)->textblock);
}

void
ShowFilechooser(int FileChooserType)
{
   etk_widget_show_all(UI_FileChooserDialog);
   
   FileChooserOperation = FileChooserType;
   switch(FileChooserType){
      case FILECHOOSER_OPEN:
         etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose an EDJ or EDC file to open");
         etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(UI_FileChooser),ETK_FALSE);
         etk_widget_hide(UI_FilechooserSaveButton);
      break;
      case FILECHOOSER_IMAGE:
         etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose an image to import");
         etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(UI_FileChooser),ETK_FALSE);
         etk_widget_hide(UI_FilechooserSaveButton);
      break;
      case FILECHOOSER_FONT:
         etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose an font to import");
         etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(UI_FileChooser),ETK_FALSE);
         etk_widget_hide(UI_FilechooserSaveButton);
      break;
      case FILECHOOSER_SAVE_EDJ:
         etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose the new edje name");
         etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(UI_FileChooser),ETK_TRUE);
         etk_widget_hide(UI_FilechooserLoadButton);
      break;
      case FILECHOOSER_SAVE_EDC:
         etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose the new edc name");
         etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(UI_FileChooser),ETK_TRUE);
         etk_widget_hide(UI_FilechooserLoadButton);
      break;
      default:
      break;
   }
}

/* functions to create interface*/
Etk_Widget*
create_filechooser_dialog(void)
{
   //Dialog
   UI_FileChooserDialog = etk_dialog_new();
   etk_object_properties_set (ETK_OBJECT(UI_FileChooserDialog), "action-area-homogeneous",ETK_FALSE,NULL);
   etk_signal_connect("delete-event", ETK_OBJECT(UI_FileChooserDialog), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_signal_connect("response", ETK_OBJECT(UI_FileChooserDialog), ETK_CALLBACK(on_FileChooserDialog_response), NULL);

   //Filechooser
   UI_FileChooser = etk_filechooser_widget_new();
   etk_dialog_pack_in_main_area(ETK_DIALOG(UI_FileChooserDialog), UI_FileChooser,
      ETK_BOX_START, ETK_BOX_EXPAND_FILL,0);
   etk_signal_connect("selected", ETK_OBJECT(UI_FileChooser), ETK_CALLBACK(on_FileChooser_selected), NULL);

   etk_dialog_button_add_from_stock(ETK_DIALOG(UI_FileChooserDialog),
      ETK_STOCK_DIALOG_CANCEL, ETK_RESPONSE_CANCEL );
   UI_FilechooserLoadButton = etk_dialog_button_add_from_stock(ETK_DIALOG(UI_FileChooserDialog),
      ETK_STOCK_DOCUMENT_OPEN ,ETK_RESPONSE_OK );
   UI_FilechooserSaveButton = etk_dialog_button_add_from_stock(ETK_DIALOG(UI_FileChooserDialog),
      ETK_STOCK_DOCUMENT_SAVE ,ETK_RESPONSE_OK );

   return UI_FileChooserDialog;
}

Etk_Widget*
create_a_color_button(char* label_text, int color_button_enum,int w,int h)
{
   Etk_Widget *vbox;
   Etk_Widget *ColorCanvas;
   Etk_Widget *label;
   Etk_Widget *shadow;

   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 3);

   //shadow
   shadow = etk_shadow_new();
   etk_shadow_border_set(ETK_SHADOW(shadow), 0);
   //etk_widget_size_request_set(shadow, 45, 45);
   etk_shadow_shadow_set(ETK_SHADOW(shadow),ETK_SHADOW_OUTSIDE, ETK_SHADOW_ALL, 10, 2, 2, 200);
   //etk_shadow_shadow_set(Etk_Shadow *shadow, Etk_Shadow_Type type, Etk_Shadow_Edges edges, int radius, int offset_x, int offset_y, int opacity);
   etk_box_append(ETK_BOX(vbox), shadow, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //ColorCanvas
   ColorCanvas = etk_canvas_new ();
  //etk_box_append(ETK_BOX(vbox), ColorCanvas, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_container_add(ETK_CONTAINER(shadow), ColorCanvas);
   etk_signal_connect("realized", ETK_OBJECT(ColorCanvas), ETK_CALLBACK(on_ColorCanvas_realize), (void*)color_button_enum);
   etk_widget_size_request_set(ColorCanvas, w, h);

   //Label
   if (label_text){
      label = etk_label_new(label_text);
      etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
      etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   }
   return vbox;
}

/*Etk_Widget*
create_toolbar_(Etk_Toolbar_Orientation o)
{
   Etk_Widget *button;
    
   //hbox
   UI_Toolbar = etk_hbox_new(ETK_FALSE, 0);
    
   //NewButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_NEW);
   //button = etk_tool_button_new_with_label("asdas");   
   etk_object_properties_set(button,"style",ETK_BUTTON_BOTH_HORIZ);
  // etk_object_properties_set(button,"label","New");
   // etk_object_properties_set(button,"stock_size",ETK_STOCK_BIG);
   //etk_toolbar_append(ETK_TOOLBAR(ToolBar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                     ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_NEW);
   etk_box_append(ETK_BOX(UI_Toolbar),button, ETK_BOX_START, ETK_BOX_NONE, 0);
    
   //OpenButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_OPEN);
   etk_object_properties_set(button,"style",ETK_BUTTON_BOTH_VERT);
   etk_object_properties_set(button,"label","New");
    etk_object_properties_set(button,"stock_size",ETK_STOCK_BIG);
   //etk_toolbar_append(ETK_TOOLBAR(ToolBar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                     ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_OPEN);
   etk_box_append(ETK_BOX(UI_Toolbar),button, ETK_BOX_START, ETK_BOX_NONE, 0);
    
   return UI_Toolbar;
}*/

Etk_Widget*
create_toolbar(Etk_Toolbar_Orientation o)
{
   Etk_Widget *button;
   Etk_Widget *sep;
   Etk_Widget *menu_item;
   Etk_Widget *image;
   
   //ToolBar
   UI_Toolbar = etk_toolbar_new ();
   etk_toolbar_orientation_set (ETK_TOOLBAR(UI_Toolbar), o);
   //etk_object_properties_set(ETK_OBJECT(UI_Toolbar),"theme-group","etk/iconbox/label");
   //etk_theme_edje_object_set(Evas_Object *object, const char *file, const char *group, const char *parent_group)
   
   //NewButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_NEW);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                     ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_NEW);

   //OpenButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_OPEN);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                     ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_OPEN);

   //SaveButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE);
   etk_object_properties_set(ETK_OBJECT(button),"label","Save",NULL);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                     ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_SAVE);
   
   //SaveEDJButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE_AS);
   etk_object_properties_set(ETK_OBJECT(button),"label","Save as",NULL);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                     ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_SAVE_EDJ);

   //SaveEDCButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE_AS);
   etk_object_properties_set(ETK_OBJECT(button),"label","Export edc",NULL);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                     ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_SAVE_EDC);
    

   sep = etk_vseparator_new();
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), sep, ETK_BOX_START);

   //AddButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_LIST_ADD);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                        ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_ADD);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);

   //AddMenu
   UI_AddMenu = etk_menu_new();

   //New Rectangle
   menu_item = etk_menu_item_image_new_with_label("Rectangle");
   image = etk_image_new_from_edje(EdjeFile,"RECT.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(on_AddMenu_item_activated), (void*)NEW_RECT);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //New Image
   menu_item = etk_menu_item_image_new_with_label("Image");
   image = etk_image_new_from_edje(EdjeFile,"IMAGE.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(on_AddMenu_item_activated),(void*) NEW_IMAGE);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));
   
   //New Text
   menu_item = etk_menu_item_image_new_with_label("Text");
   image = etk_image_new_from_edje(EdjeFile,"TEXT.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(on_AddMenu_item_activated), (void*)NEW_TEXT);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));
   
   //New Program
   menu_item = etk_menu_item_image_new_with_label("Program");
   image = etk_image_new_from_edje(EdjeFile,"PROG.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(on_AddMenu_item_activated), (void*)NEW_PROG);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));
   
   //Separator
   menu_item = etk_menu_item_separator_new();
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //New Description
   menu_item = etk_menu_item_image_new_with_label("A new state to part");
   image = etk_image_new_from_edje(EdjeFile,"DESC.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(on_AddMenu_item_activated), (void*)NEW_DESC);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //New Group
   menu_item = etk_menu_item_image_new_with_label("A new group to edj");
   image = etk_image_new_from_edje(EdjeFile,"NONE.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(on_AddMenu_item_activated), (void*)NEW_GROUP);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));

   //RemoveButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_LIST_REMOVE);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                     ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_REMOVE);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);

   //RemoveMenu
   UI_RemoveMenu = etk_menu_new();

   //description
   menu_item = etk_menu_item_image_new_with_label("Selected State");
   image = etk_image_new_from_edje(EdjeFile,"DESC.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
         ETK_CALLBACK(on_RemoveMenu_item_activated), (void*)REMOVE_DESCRIPTION);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_RemoveMenu), ETK_MENU_ITEM(menu_item));

   //part
   menu_item = etk_menu_item_image_new_with_label("Selected Part");
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
               ETK_CALLBACK(on_RemoveMenu_item_activated), (void*)REMOVE_PART);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_RemoveMenu), ETK_MENU_ITEM(menu_item));

   //group
   menu_item = etk_menu_item_image_new_with_label("Selected Group");
   image = etk_image_new_from_edje(EdjeFile,"NONE.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
               ETK_CALLBACK(on_RemoveMenu_item_activated), (void*)REMOVE_GROUP);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_RemoveMenu), ETK_MENU_ITEM(menu_item));

      
   //program 
   menu_item = etk_menu_item_image_new_with_label("Selected Program");
   image = etk_image_new_from_edje(EdjeFile,"PROG.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
               ETK_CALLBACK(on_RemoveMenu_item_activated), (void*)REMOVE_PROG);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_RemoveMenu), ETK_MENU_ITEM(menu_item));
    
   sep = etk_vseparator_new();
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), sep, ETK_BOX_START);

   //MoveUp Button
   button = etk_tool_button_new_from_stock( ETK_STOCK_GO_UP);
   etk_object_properties_set(ETK_OBJECT(button),"label","Lower",NULL);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                     ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_MOVE_UP);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);

   //MoveDown Button
   button = etk_tool_button_new_from_stock( ETK_STOCK_GO_DOWN);
   etk_object_properties_set(ETK_OBJECT(button),"label","Raise",NULL);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                  ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_MOVE_DOWN);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);

   sep = etk_vseparator_new();
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), sep, ETK_BOX_START);

   //OptionsButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_PREFERENCES_SYSTEM);
   etk_object_properties_set(ETK_OBJECT(button),"label","Options",NULL);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                     ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_OPTIONS);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);

   //OptionsMenu
   UI_OptionsMenu = etk_menu_new();

   //set bg1
   menu_item = etk_menu_item_image_new_with_label("White squared background");
   image = etk_image_new_from_edje(EdjeFile,"BG1_I.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
         ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_OPTION_BG1);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_OptionsMenu), ETK_MENU_ITEM(menu_item));

   //set bg2
   menu_item = etk_menu_item_image_new_with_label("Gray squared background");
   image = etk_image_new_from_edje(EdjeFile,"BG2_I.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
               ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_OPTION_BG2);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_OptionsMenu), ETK_MENU_ITEM(menu_item));

   //set bg3
   menu_item = etk_menu_item_image_new_with_label("White background");
   image = etk_image_new_from_edje(EdjeFile,"NONE.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
               ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_OPTION_BG3);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_OptionsMenu), ETK_MENU_ITEM(menu_item));

   //set bg4
   menu_item = etk_menu_item_image_new_with_label("Black background");
   image = etk_image_new_from_edje(EdjeFile,"NONE.PNG");
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
               ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_OPTION_BG4);
   etk_menu_shell_append(ETK_MENU_SHELL(UI_OptionsMenu), ETK_MENU_ITEM(menu_item));

   //DebugButton
   button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_PROPERTIES);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                        ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_DEBUG);
   etk_object_properties_set(ETK_OBJECT(button),"label","Debug",NULL);
    
   sep = etk_vseparator_new();
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), sep, ETK_BOX_START);

#if TEST_DIRECT_EDJE
   //UI_GroupsComboBox
   UI_GroupsComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_GroupsComboBox), ETK_COMBOBOX_LABEL, 30, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_GroupsComboBox));
   etk_widget_size_request_set(UI_GroupsComboBox,200, 10);
   etk_widget_padding_set (UI_GroupsComboBox, 0, 0, 7, 7);
   etk_combobox_items_height_set (ETK_COMBOBOX(UI_GroupsComboBox), 16);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), UI_GroupsComboBox, ETK_BOX_START);
    
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_GroupsComboBox),
                      ETK_CALLBACK(on_GroupsComboBox_changed), NULL);
#else
   //Test Button
   button = etk_tool_button_new_from_stock( ETK_STOCK_MEDIA_PLAYBACK_START);
   etk_signal_connect("clicked", ETK_OBJECT(button),
                        ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_PLAY);
   etk_object_properties_set(ETK_OBJECT(button),"label","Test group",NULL);
   etk_toolbar_append(ETK_TOOLBAR(UI_Toolbar), button, ETK_BOX_START);
#endif
    
   return UI_Toolbar;
}

Etk_Widget*
create_group_frame(void)
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
   UI_GroupMinWSpinner =  etk_spinner_new (0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_GroupMinWSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_GroupMinWSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("x");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_GroupMinHSpinner
   UI_GroupMinHSpinner =  etk_spinner_new (0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_GroupMinHSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_GroupMinHSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("max");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",1.0,NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_GroupMaxWSpinner
   UI_GroupMaxWSpinner =  etk_spinner_new (0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_GroupMaxWSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_GroupMaxWSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("x");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_GroupMaxHspinner
   UI_GroupMaxHSpinner =  etk_spinner_new (0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_GroupMaxHSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_GroupMaxHSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   etk_signal_connect("text-changed", ETK_OBJECT(UI_GroupNameEntry),
                      ETK_CALLBACK(on_GroupNameEntry_text_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_GroupMinWSpinner),
                      ETK_CALLBACK(on_GroupSpinner_value_changed),
                      (void *)MINW_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_GroupMinHSpinner),
                      ETK_CALLBACK(on_GroupSpinner_value_changed),
                      (void *)MINH_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_GroupMaxWSpinner),
                      ETK_CALLBACK(on_GroupSpinner_value_changed),
                      (void *)MAXW_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_GroupMaxHSpinner),
                      ETK_CALLBACK(on_GroupSpinner_value_changed),
                     (void *)MAXH_SPINNER);

   return vbox;
}

Etk_Widget*
create_tree(void)
{
   Etk_Tree_Col *col;
   
   //UI_PartsTree
   UI_PartsTree = etk_tree_new();
   etk_widget_padding_set(UI_PartsTree,2,2,2,2);
   etk_tree_mode_set (ETK_TREE(UI_PartsTree), ETK_TREE_MODE_TREE);
   etk_tree_headers_visible_set (ETK_TREE(UI_PartsTree), ETK_FALSE);
   etk_widget_size_request_set(UI_PartsTree, 260, 300);
   //Main column
   col = etk_tree_col_new(ETK_TREE(UI_PartsTree), "File contents",100,0);
   etk_tree_col_model_add(col,etk_tree_model_image_new());
   etk_tree_col_model_add(col,etk_tree_model_text_new());
   etk_tree_col_resizable_set (col, ETK_FALSE);
   etk_tree_col_expand_set (col,ETK_TRUE);
   //Visibility column
   col = etk_tree_col_new(ETK_TREE(UI_PartsTree), "vis", 10,0);
   etk_tree_col_visible_set (col, DEBUG_TREE);
   etk_tree_col_model_add(col,etk_tree_model_checkbox_new());
   etk_tree_col_resizable_set (col, ETK_FALSE);
   etk_tree_col_expand_set (col,ETK_FALSE);
   //RowType column
   col = etk_tree_col_new(ETK_TREE(UI_PartsTree), "type",10, 0);
   etk_tree_col_model_add(col,etk_tree_model_int_new());
   etk_tree_col_visible_set (col, DEBUG_TREE);
   etk_tree_col_resizable_set (col, ETK_FALSE);
   etk_tree_col_expand_set (col,ETK_FALSE);
   //Parent part row
   col = etk_tree_col_new(ETK_TREE(UI_PartsTree), "parent",100, 0);
   etk_tree_col_model_add(col,etk_tree_model_text_new());
   etk_tree_col_visible_set (col, DEBUG_TREE);
   etk_tree_col_resizable_set (col, ETK_FALSE);
   etk_tree_col_expand_set (col,ETK_FALSE);
   etk_tree_build(ETK_TREE(UI_PartsTree));

   etk_signal_connect("row-selected", ETK_OBJECT(UI_PartsTree),
                      ETK_CALLBACK(on_PartsTree_row_selected), NULL);

   return UI_PartsTree;
}

Etk_Widget*
create_description_frame(void)
{
   Etk_Widget *vbox;
   Etk_Widget *hbox;
   Etk_Widget *label;
   Etk_Combobox_Item *ComboItem;

   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 0);
  // etk_container_add(ETK_CONTAINER(UI_DescriptionFrame), vbox);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("<b>Name</b>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //StateEntry
   UI_StateEntry = etk_entry_new();
   etk_box_append(ETK_BOX(hbox), UI_StateEntry, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_StateIndexSpinner
   UI_StateIndexSpinner = etk_spinner_new (0.0, 1.0, 0.0, 0.1, 1.0);
   etk_spinner_digits_set (ETK_SPINNER(UI_StateIndexSpinner), 1);
   etk_widget_size_request_set(UI_StateIndexSpinner,45, 20);
   etk_box_append(ETK_BOX(hbox),UI_StateIndexSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("<b>Aspect</b>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("min:");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",1.0,NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_AspectMinSpinner
   UI_AspectMinSpinner = etk_spinner_new (0.0, 100.0, 0.0, 0.1, 1.0);
   etk_spinner_digits_set (ETK_SPINNER(UI_AspectMinSpinner), 1);
   etk_widget_size_request_set(UI_AspectMinSpinner,45, 20);
   etk_box_append(ETK_BOX(hbox),UI_AspectMinSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("max:");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",1.0,NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_AspectMaxSpinner
   UI_AspectMaxSpinner = etk_spinner_new (0.0, 100.0, 0.0, 0.1, 1.0);
   etk_spinner_digits_set (ETK_SPINNER(UI_AspectMaxSpinner), 1);
   etk_widget_size_request_set(UI_AspectMaxSpinner,45, 20);
   etk_box_append(ETK_BOX(hbox),UI_AspectMaxSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_AspectCombo
   UI_AspectComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_AspectComboBox), ETK_COMBOBOX_LABEL, 30, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_AspectComboBox));
   etk_box_append(ETK_BOX(hbox),UI_AspectComboBox, ETK_BOX_START, ETK_BOX_NONE, 0);

   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_AspectComboBox), "None");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_ASPECT_PREFERENCE_NONE);

   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_AspectComboBox), "Vertical");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_ASPECT_PREFERENCE_VERTICAL);

   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_AspectComboBox), "Horizontal");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_ASPECT_PREFERENCE_HORIZONTAL);

   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_AspectComboBox), "Both");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_ASPECT_PREFERENCE_BOTH);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("min");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_StateMinWSpinner
   UI_StateMinWSpinner =  etk_spinner_new (0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_StateMinWSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_StateMinWSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("x");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_StateMinHSpinner
   UI_StateMinHSpinner =  etk_spinner_new (0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_StateMinHSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_StateMinHSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("max");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",1.0,NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_StateMaxWSpinner
   UI_StateMaxWSpinner =  etk_spinner_new (0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_StateMaxWSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_StateMaxWSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("x");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_StateMaxHspinner
   UI_StateMaxHSpinner =  etk_spinner_new (0, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_StateMaxHSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_StateMaxHSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   //UI_StateAlignHSpinner
   label = etk_label_new("Align");
   etk_box_append(ETK_BOX(hbox),label, ETK_BOX_START, ETK_BOX_NONE, 0);
    
   UI_StateAlignHSpinner =  etk_spinner_new (0, 1, 0, 0.01, 0.1);
   etk_spinner_digits_set (ETK_SPINNER(UI_StateAlignHSpinner), 2);
   etk_widget_size_request_set(UI_StateAlignHSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_StateAlignHSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   //UI_StateAlignVSpinner
   label = etk_label_new("V Align");
   etk_box_append(ETK_BOX(hbox),label, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   UI_StateAlignVSpinner =  etk_spinner_new (0, 1, 0, 0.01, 0.1);
   etk_spinner_digits_set (ETK_SPINNER(UI_StateAlignVSpinner), 2);
   etk_widget_size_request_set(UI_StateAlignVSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_StateAlignVSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);
    
   etk_signal_connect("text-changed", ETK_OBJECT(UI_StateEntry),
                      ETK_CALLBACK(on_StateEntry_text_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateIndexSpinner),
                      ETK_CALLBACK(on_StateIndexSpinner_value_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_AspectMinSpinner),
                      ETK_CALLBACK(on_AspectSpinner_value_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_AspectMaxSpinner),
                      ETK_CALLBACK(on_AspectSpinner_value_changed), NULL);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_AspectComboBox),
                      ETK_CALLBACK(on_AspectComboBox_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateMinWSpinner),
                      ETK_CALLBACK(on_StateMinMaxSpinner_value_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateMinHSpinner),
                      ETK_CALLBACK(on_StateMinMaxSpinner_value_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateMaxWSpinner),
                      ETK_CALLBACK(on_StateMinMaxSpinner_value_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateMaxHSpinner),
                      ETK_CALLBACK(on_StateMinMaxSpinner_value_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateAlignVSpinner),
                      ETK_CALLBACK(on_FontAlignSpinner_value_changed), (void*)STATE_ALIGNV_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_StateAlignHSpinner),
                      ETK_CALLBACK(on_FontAlignSpinner_value_changed), (void*)STATE_ALIGNH_SPINNER);
   return vbox;
}

Etk_Widget*
create_rectangle_frame(void)
{
   return create_a_color_button("Color",COLOR_OBJECT_RECT,100,30);
}

Etk_Widget*
create_image_frame(void)
{
   Etk_Widget *label;
   Etk_Widget *table;
   Etk_Tree_Col *col1;

   //table
   table = etk_table_new (5, 8, ETK_TABLE_NOT_HOMOGENEOUS);

   //ImageTweenRadio
   UI_ImageNormalRadio = etk_radio_button_new_with_label("Normal",NULL);
   UI_ImageTweenRadio = etk_radio_button_new_with_label_from_widget("Tween", 
                        ETK_RADIO_BUTTON(UI_ImageNormalRadio));
   etk_table_attach_default (ETK_TABLE(table),UI_ImageNormalRadio, 1, 2, 0, 0);
   etk_table_attach_default (ETK_TABLE(table),UI_ImageTweenRadio, 3, 4, 0, 0);

   //imageComboBox
   UI_ImageComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_ImageComboBox),
                           ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_ImageComboBox),
                           ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_ImageComboBox));
   etk_table_attach_default (ETK_TABLE(table),UI_ImageComboBox, 0, 3, 1, 1);

   //AddImageButton
   UI_ImageAddButton = etk_button_new_from_stock (ETK_STOCK_DOCUMENT_OPEN);
   etk_object_properties_set (ETK_OBJECT(UI_ImageAddButton), "label","",NULL);
   etk_table_attach_default (ETK_TABLE(table),UI_ImageAddButton, 4, 4, 1, 1);

   //ImageTweenVBox
   UI_ImageTweenVBox = etk_vbox_new(ETK_TRUE, 2);
   etk_table_attach_default (ETK_TABLE(table),UI_ImageTweenVBox, 0, 0, 2, 2);
   
   //MoveUpTweenButton
   UI_MoveUpTweenButton = etk_button_new_from_stock (ETK_STOCK_GO_UP);
   etk_button_style_set(ETK_BUTTON(UI_MoveUpTweenButton),  ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(UI_MoveUpTweenButton), 
      ETK_CALLBACK(on_AllButton_click), (void*)IMAGE_TWEEN_UP);
   etk_box_append (ETK_BOX(UI_ImageTweenVBox), UI_MoveUpTweenButton, 
                     ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   //MoveDownTweenButton
   UI_MoveDownTweenButton = etk_button_new_from_stock (ETK_STOCK_GO_DOWN);
   etk_button_style_set(ETK_BUTTON(UI_MoveDownTweenButton),  ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(UI_MoveDownTweenButton), 
      ETK_CALLBACK(on_AllButton_click), (void*)IMAGE_TWEEN_DOWN);
   etk_box_append (ETK_BOX(UI_ImageTweenVBox), UI_MoveDownTweenButton, 
                     ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
                     
   //DeleteTweenButton
   UI_DeleteTweenButton = etk_button_new_from_stock (ETK_STOCK_EDIT_DELETE);
   etk_button_style_set(ETK_BUTTON(UI_DeleteTweenButton), ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(UI_DeleteTweenButton), 
      ETK_CALLBACK(on_AllButton_click), (void*)IMAGE_TWEEN_DELETE);
   etk_box_append (ETK_BOX(UI_ImageTweenVBox), UI_DeleteTweenButton, 
                     ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
                     
   //ImageTweenList
   UI_ImageTweenList = etk_tree_new();
   etk_tree_mode_set(ETK_TREE(UI_ImageTweenList), ETK_TREE_MODE_LIST);
   etk_tree_headers_visible_set(ETK_TREE(UI_ImageTweenList), FALSE);
  // etk_tree_multiple_select_set(ETK_TREE(UI_ImageTweenList), ETK_TRUE);
   col1 = etk_tree_col_new(ETK_TREE(UI_ImageTweenList), "Tween", 130, 0.0);
   etk_tree_col_model_add(col1, etk_tree_model_image_new());
   etk_tree_col_model_add(col1, etk_tree_model_text_new());
   etk_tree_build(ETK_TREE(UI_ImageTweenList));
   etk_table_attach_default (ETK_TABLE(table),UI_ImageTweenList, 1, 4, 2, 2);	

   label = etk_label_new("Alpha");
   etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 3, 3);

   //ImageAlphaSlider
   UI_ImageAlphaSlider = etk_hslider_new (0, 255, 15, 1,20);
   etk_table_attach_default (ETK_TABLE(table),UI_ImageAlphaSlider, 1, 4, 3, 3);

   label = etk_label_new("Left");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default (ETK_TABLE(table),label, 1, 1, 4, 4);

   label = etk_label_new("Right");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default (ETK_TABLE(table),label, 2, 2, 4, 4);

   label = etk_label_new("Top");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default (ETK_TABLE(table),label, 3, 3, 4, 4);

   label = etk_label_new("Bottom");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default (ETK_TABLE(table),label, 4, 4, 4, 4);

   label = etk_label_new("Border");
   //etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 5, 5);

   //UI_BorderLeftSpinner
   UI_BorderLeftSpinner = etk_spinner_new (0, 500, 0, 1, 10);
   etk_widget_size_request_set(UI_BorderLeftSpinner,45, 20);
   etk_table_attach_default (ETK_TABLE(table),UI_BorderLeftSpinner, 1, 1, 5, 5);

   //UI_BorderRightSpinner
   UI_BorderRightSpinner = etk_spinner_new (0, 500, 0, 1, 10);
   etk_widget_size_request_set(UI_BorderRightSpinner,45, 20);
   etk_table_attach_default (ETK_TABLE(table),UI_BorderRightSpinner, 2, 2, 5, 5);

   //UI_BorderTopSpinner
   UI_BorderTopSpinner = etk_spinner_new (0, 500, 0, 1, 10);
   etk_widget_size_request_set(UI_BorderTopSpinner,45, 20);
   etk_table_attach_default (ETK_TABLE(table),UI_BorderTopSpinner, 3, 3, 5, 5);

   //UI_BorderBottomSpinner
   UI_BorderBottomSpinner = etk_spinner_new (0, 500, 0, 1, 10);
   etk_widget_size_request_set(UI_BorderBottomSpinner,45, 20);
   etk_table_attach_default (ETK_TABLE(table),UI_BorderBottomSpinner, 4, 4, 5, 5);

   etk_signal_connect("clicked", ETK_OBJECT(UI_ImageNormalRadio), 
            ETK_CALLBACK(on_AllButton_click), (void*)IMAGE_NORMAL_RADIO);
   etk_signal_connect("clicked", ETK_OBJECT(UI_ImageTweenRadio), 
            ETK_CALLBACK(on_AllButton_click), (void*)IMAGE_TWEEN_RADIO);
   etk_signal_connect("row-selected", ETK_OBJECT(UI_ImageTweenList),
            ETK_CALLBACK(on_ImageTweenList_row_selected), NULL);
   etk_signal_connect("clicked", ETK_OBJECT(UI_ImageAddButton),
            ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_IMAGE_FILE_ADD);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_ImageComboBox),
            ETK_CALLBACK(on_ImageComboBox_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_ImageAlphaSlider),
            ETK_CALLBACK(on_ImageAlphaSlider_value_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_BorderLeftSpinner),
            ETK_CALLBACK(on_BorderSpinner_value_changed), (void *)BORDER_LEFT);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_BorderRightSpinner),
            ETK_CALLBACK(on_BorderSpinner_value_changed), (void *)BORDER_RIGHT);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_BorderTopSpinner),
            ETK_CALLBACK(on_BorderSpinner_value_changed), (void *)BORDER_TOP);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_BorderBottomSpinner),
            ETK_CALLBACK(on_BorderSpinner_value_changed), (void *)BORDER_BOTTOM);

   return table;
}

Etk_Widget*
create_text_frame(void)
{
   Etk_Widget *vbox;
   Etk_Widget *hbox;
   Etk_Widget *label;
   Etk_Widget *table;
   Etk_Combobox_Item *ComboItem;

   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 5);

   //table
   table = etk_table_new (5, 4, ETK_TABLE_NOT_HOMOGENEOUS);
   etk_box_append(ETK_BOX(vbox), table, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("Text");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 0,0);

   //Text Entry
   UI_TextEntry = etk_entry_new ();
   //etk_widget_size_request_set(UI_TextEntry,30, 30);
   etk_table_attach_default (ETK_TABLE(table),UI_TextEntry, 1, 4, 0,0);

   label = etk_label_new("Font");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 1,1);

   //FontComboBox
   UI_FontComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_FontComboBox),
                           ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_FontComboBox),
                           ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_FontComboBox));
   etk_table_attach_default (ETK_TABLE(table),UI_FontComboBox, 1, 1, 1,1);

   //FontAddButton
   UI_FontAddButton = etk_button_new_from_stock (ETK_STOCK_DOCUMENT_OPEN);
   etk_object_properties_set (ETK_OBJECT(UI_FontAddButton), "label","",NULL);
   etk_table_attach_default (ETK_TABLE(table),UI_FontAddButton, 2, 2, 1, 1);


   label = etk_label_new("Size");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default (ETK_TABLE(table),label, 3, 3, 1,1);

   //FontSizeSpinner
   UI_FontSizeSpinner =  etk_spinner_new (0, 200, 0, 1, 10);
   etk_widget_size_request_set(UI_FontSizeSpinner, 45, 20);
   etk_table_attach_default (ETK_TABLE(table),UI_FontSizeSpinner, 4, 4, 1,1);

   //FontAlignHSpinner
   label = etk_label_new("Align");
   etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 2,2);
    
   UI_FontAlignHSpinner =  etk_spinner_new (0, 1, 0, 0.01, 0.1);
   etk_spinner_digits_set (ETK_SPINNER(UI_FontAlignHSpinner), 2);
   etk_widget_size_request_set(UI_FontAlignHSpinner, 45, 20);
   etk_table_attach_default (ETK_TABLE(table),UI_FontAlignHSpinner, 1, 1, 2,2);
   
   //FontAlignVSpinner
   label = etk_label_new("V Align");
   etk_table_attach_default (ETK_TABLE(table),label, 2, 2, 2,2);
   
   UI_FontAlignVSpinner =  etk_spinner_new (0, 1, 0, 0.01, 0.1);
   etk_spinner_digits_set (ETK_SPINNER(UI_FontAlignVSpinner), 2);
   etk_widget_size_request_set(UI_FontAlignVSpinner, 45, 20);
   etk_table_attach_default (ETK_TABLE(table),UI_FontAlignVSpinner, 3, 4, 2,2);
   
   //PartEffectComboBox
   label = etk_label_new("Effect");
   etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 3,3);
   
   UI_EffectComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_EffectComboBox), 
      ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_EffectComboBox),
      ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_EffectComboBox));
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Plain");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_TEXT_EFFECT_PLAIN);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Outline");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_TEXT_EFFECT_OUTLINE);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Soft Outline");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_TEXT_EFFECT_SOFT_OUTLINE);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Shadow");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_TEXT_EFFECT_SHADOW);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Soft Shadow");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_TEXT_EFFECT_SOFT_SHADOW);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Outline Shadow");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_TEXT_EFFECT_OUTLINE_SHADOW);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Outline Soft Shadow");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_TEXT_EFFECT_OUTLINE_SOFT_SHADOW);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Far Shadow");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_TEXT_EFFECT_FAR_SHADOW);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Far Soft Shadow");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_TEXT_EFFECT_FAR_SOFT_SHADOW);
   ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox),
      etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Glow");
   etk_combobox_item_data_set (ComboItem, (void*)ENGRAVE_TEXT_EFFECT_GLOW);
   
   etk_table_attach_default (ETK_TABLE(table),UI_EffectComboBox, 1, 4, 3,3);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 10);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Color buttons
   etk_box_append(ETK_BOX(hbox), create_a_color_button("Text",COLOR_OBJECT_TEXT,30,30),
                  ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_box_append(ETK_BOX(hbox), create_a_color_button("Shadow",COLOR_OBJECT_SHADOW,30,30),
                  ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_box_append(ETK_BOX(hbox), create_a_color_button("Outline",COLOR_OBJECT_OUTLINE,30,30),
                  ETK_BOX_START, ETK_BOX_EXPAND, 0);

   etk_signal_connect("clicked", ETK_OBJECT(UI_FontAddButton),
                      ETK_CALLBACK(on_AllButton_click), (void*)TOOLBAR_FONT_FILE_ADD);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_FontComboBox),
                      ETK_CALLBACK(on_FontComboBox_changed), NULL);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_EffectComboBox),
                      ETK_CALLBACK(on_EffectComboBox_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FontSizeSpinner),
                      ETK_CALLBACK(on_FontSizeSpinner_value_changed), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_TextEntry),
                      ETK_CALLBACK(on_TextEntry_text_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FontAlignVSpinner),
                      ETK_CALLBACK(on_FontAlignSpinner_value_changed), (void*)TEXT_ALIGNV_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_FontAlignHSpinner),
                      ETK_CALLBACK(on_FontAlignSpinner_value_changed), (void*)TEXT_ALIGNH_SPINNER);
    
   return vbox;
}

Etk_Widget*
create_position_frame(void)
{
   Etk_Widget *vbox;
   Etk_Widget *hbox;
   Etk_Widget *label;

/*    //Position Notebook
   notebook = etk_notebook_new ();
   etk_container_add(ETK_CONTAINER(UI_PositionFrame), notebook);
 */
/*          //Simple TAB
         vbox = etk_vbox_new(ETK_FALSE, 0);
         etk_notebook_page_append (ETK_NOTEBOOK(notebook), "Simple", vbox);

            //hbox
            hbox = etk_hbox_new(ETK_FALSE, 0);
            etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

               label = etk_label_new("Container");
               etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
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
   // etk_notebook_page_append (ETK_NOTEBOOK(notebook), "Advanced", vbox);

   label = etk_label_new("<color=#FF0000><b>First_Point</b></>");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("<color=#FF0000><b>X</b></>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Rel1XSpinner
   UI_Rel1XSpinner = etk_spinner_new (-100.0, 100.0, 0.0, 0.01, 0.1);
   etk_spinner_digits_set (ETK_SPINNER(UI_Rel1XSpinner), 2);
   etk_widget_size_request_set(UI_Rel1XSpinner,45, 20);
   etk_box_append(ETK_BOX(hbox),UI_Rel1XSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#FF0000>+</>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //Rel1XOffsetEntry
   UI_Rel1XOffsetSpinner =  etk_spinner_new (-2000, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_Rel1XOffsetSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox),UI_Rel1XOffsetSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#FF0000>to</>");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",1.0,NULL);
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
   UI_Rel1YSpinner = etk_spinner_new (-100.0, 100.0, 0.0, 0.01, 0.1);
   etk_spinner_digits_set (ETK_SPINNER(UI_Rel1YSpinner), 2);
   etk_widget_size_request_set(UI_Rel1YSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel1YSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#FF0000>+</>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //Rel1YOffsetSpinner
   UI_Rel1YOffsetSpinner =  etk_spinner_new (-2000, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_Rel1YOffsetSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel1YOffsetSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#FF0000>to</> ");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",1.0,NULL);
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
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   label = etk_label_new("<color=#0000FF><b>X</b></>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Rel2XSpinner
   UI_Rel2XSpinner = etk_spinner_new (-100.0, 100.0, 0.0, 0.01, 0.1);
   etk_spinner_digits_set (ETK_SPINNER(UI_Rel2XSpinner), 2);
   etk_widget_size_request_set(UI_Rel2XSpinner,45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel2XSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#0000FF>+</>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //Rel2XOffsetSpinner
   UI_Rel2XOffsetSpinner = etk_spinner_new (-2000, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_Rel2XOffsetSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel2XOffsetSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#0000FF>to</>");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",1.0,NULL);
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
   UI_Rel2YSpinner = etk_spinner_new (-100.0, 100.0, 0.0, 0.01, 0.1);
   etk_spinner_digits_set (ETK_SPINNER(UI_Rel2YSpinner), 2);
   etk_widget_size_request_set(UI_Rel2YSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel2YSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#0000FF>+</>");
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   //Rel2YOffsetSpinner
   UI_Rel2YOffsetSpinner = etk_spinner_new (-2000, 2000, 0, 1, 10);
   etk_widget_size_request_set(UI_Rel2YOffsetSpinner, 45, 20);
   etk_box_append(ETK_BOX(hbox), UI_Rel2YOffsetSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);

   label = etk_label_new("<color=#0000FF>to</> ");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",1.0,NULL);
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
      ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void *)REL1X_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel1YOffsetSpinner),
      ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void *)REL1Y_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel2XOffsetSpinner),
      ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void *)REL2X_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel2YOffsetSpinner),
      ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void *)REL2Y_SPINNER);

   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel1XSpinner),
      ETK_CALLBACK(on_RelSpinner_value_changed), (void *)REL1X_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel1YSpinner),
      ETK_CALLBACK(on_RelSpinner_value_changed), (void *)REL1Y_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel2XSpinner),
      ETK_CALLBACK(on_RelSpinner_value_changed), (void *)REL2X_SPINNER);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Rel2YSpinner),
      ETK_CALLBACK(on_RelSpinner_value_changed), (void *)REL2Y_SPINNER);

   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_Rel1ToXComboBox),
               ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL1X_SPINNER);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_Rel1ToYComboBox),
               ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL1Y_SPINNER);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_Rel2ToXComboBox),
               ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL2X_SPINNER);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_Rel2ToYComboBox),
               ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL2Y_SPINNER);

   return vbox;
}

Etk_Widget*
create_part_frame(void)
{
   Etk_Widget *table;
   Etk_Widget *label;
   Etk_Widget *combo;

   //table
   table = etk_table_new (2, 3, ETK_TABLE_NOT_HOMOGENEOUS);

   //PartNameEntry
   label = etk_label_new("<b>Name</b>");
   etk_table_attach (ETK_TABLE(table), label, 0, 0, 0, 0,ETK_TABLE_NONE,0,0);
   UI_PartNameEntry = etk_entry_new();
   etk_table_attach_default (ETK_TABLE(table),UI_PartNameEntry, 1, 1, 0, 0);

   //PartEventsCheck
   UI_PartEventsCheck = etk_check_button_new_with_label ("<b>Accept mouse events</b>");
   etk_table_attach_default (ETK_TABLE(table),UI_PartEventsCheck, 0, 1, 1, 1);

   //PartClipToComboBox
   label = etk_label_new("<b>Clip_to</b>");
   etk_table_attach (ETK_TABLE(table), label, 0, 0, 2, 2,ETK_TABLE_NONE,0,0);

   combo = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(combo),
      ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(combo),
      ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(combo));
   etk_combobox_item_append(ETK_COMBOBOX(combo), 
         etk_image_new_from_edje (EdjeFile,"NONE.PNG"), "Not yet implemented");

   etk_table_attach_default (ETK_TABLE(table), combo, 1, 1, 2, 2);

   etk_signal_connect("text-changed", ETK_OBJECT(UI_PartNameEntry),
                      ETK_CALLBACK(on_PartNameEntry_text_changed), NULL);
   etk_signal_connect("toggled", ETK_OBJECT(UI_PartEventsCheck),
                      ETK_CALLBACK(on_PartEventsCheck_toggled), NULL);

   return table;
}

Etk_Widget*
create_program_frame(void)
{
   Etk_Widget *table;
   Etk_Widget *label;
   
   //table
   table = etk_table_new (4, 10, ETK_TABLE_NOT_HOMOGENEOUS);

   //UI_ProgramEntry
   label = etk_label_new("<b>Name</b>");
   etk_table_attach (ETK_TABLE(table), label, 0, 0, 0, 0,ETK_TABLE_NONE,0,0);
   UI_ProgramEntry = etk_entry_new();
   etk_tooltips_tip_set(UI_ProgramEntry, "Symbolic <b>name</b> of program as a unique identifier.");
   etk_table_attach_default (ETK_TABLE(table),UI_ProgramEntry, 1, 3, 0, 0);

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
   etk_table_attach (ETK_TABLE(table), label, 0, 0, 2, 2,ETK_TABLE_NONE,0,0);
   UI_SignalEntry = etk_entry_new();
   etk_tooltips_tip_set(UI_SignalEntry, "The name of the <b>signal</b> that will trigger the program.<br>Wildcards can be used to widen the scope, ex: \"mouse,down,*\"<br>Can be a mouse signal ( mouse,in ; mouse,up,1 )<br>Or a user defined signal (emitted by the application)");
   etk_table_attach_default (ETK_TABLE(table),UI_SignalEntry, 1, 3, 2, 2);

   //UI_DelayFromSpinner
   label = etk_label_new("<b>Delay</b>");
   etk_table_attach (ETK_TABLE(table), label, 0, 0, 3, 3,ETK_TABLE_NONE,0,0);
   UI_DelayFromSpinner = etk_spinner_new (0.0, 999.0, 0.0, 0.1, 1.0);
   etk_tooltips_tip_set(UI_DelayFromSpinner, "The number of seconds to wait before running the program");
   etk_spinner_digits_set (ETK_SPINNER(UI_DelayFromSpinner), 1);
   etk_widget_size_request_set(UI_DelayFromSpinner,45, 20);
   etk_table_attach_default (ETK_TABLE(table),UI_DelayFromSpinner, 1, 1, 3, 3);

   //UI_DelayRangeSpinner
   label = etk_label_new("<b> + random</b>");
   etk_table_attach (ETK_TABLE(table), label, 2, 2, 3, 3,ETK_TABLE_NONE,0,0);
   UI_DelayRangeSpinner = etk_spinner_new (0.0, 999.0, 0.0, 0.1, 1.0);
   etk_tooltips_tip_set(UI_DelayRangeSpinner, "The maximum <b>random</b> seconds which is added to <b>Delay</b>");
   etk_spinner_digits_set (ETK_SPINNER(UI_DelayRangeSpinner), 1);
   etk_widget_size_request_set(UI_DelayRangeSpinner,45, 20);
   etk_table_attach_default (ETK_TABLE(table),UI_DelayRangeSpinner, 3, 3, 3, 3);

   //UI_AfterEntry
   label = etk_label_new("<b>After(s)</b>");
   etk_table_attach (ETK_TABLE(table), label, 0, 0, 4, 4,ETK_TABLE_NONE,0,0);
   UI_AfterEntry = etk_entry_new();
   etk_tooltips_tip_set(UI_AfterEntry, "Specifies program(s) to run after the current program completes.<br>The <i>source</i> and <i>signal</i> parameters of a program run as an <i>after</j> are ignored.<br>Multiple programs can be specified separated by ',' (comma).");
   etk_table_attach_default (ETK_TABLE(table),UI_AfterEntry, 1, 3, 4, 4);

   //UI_ActionComboBox
   Etk_Combobox_Item *item = NULL;
   label = etk_label_new("<b>Action</b>");
   etk_table_attach (ETK_TABLE(table), label, 0, 0, 5, 5,ETK_TABLE_NONE,0,0);
   UI_ActionComboBox = etk_combobox_new();
   //etk_tooltips_tip_set(UI_ActionComboBox, "<b>Action</b> to be performed by the program.<br>STATE_SET is used to change the state of one or more targets parts<br>, ACTION_STOP and SIGNAL_EMIT.<br>");
   etk_combobox_column_add(ETK_COMBOBOX(UI_ActionComboBox),
      ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_ActionComboBox),
      ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_ActionComboBox));
   item = etk_combobox_item_append(ETK_COMBOBOX(UI_ActionComboBox),
                  etk_image_new_from_edje (EdjeFile,"DESC.PNG"), "State Set");
   etk_combobox_item_data_set (item, ENGRAVE_ACTION_STATE_SET);
   item = etk_combobox_item_append(ETK_COMBOBOX(UI_ActionComboBox),
                  etk_image_new_from_edje (EdjeFile,"DESC.PNG"), "Action Stop");
   etk_combobox_item_data_set (ETK_COMBOBOX_ITEM(item), (void*)ENGRAVE_ACTION_STOP);
   item = etk_combobox_item_append(ETK_COMBOBOX(UI_ActionComboBox),
                  etk_image_new_from_edje (EdjeFile,"DESC.PNG"), "Signal Emit");
   etk_combobox_item_data_set (ETK_COMBOBOX_ITEM(item),
                               (void*)ENGRAVE_ACTION_SIGNAL_EMIT);
   etk_table_attach_default (ETK_TABLE(table),UI_ActionComboBox, 1, 3, 5, 5);

   //UI_TargetEntry
   UI_TargetLabel = etk_label_new("<b>Target(s)</b>");
   etk_table_attach(ETK_TABLE(table), UI_TargetLabel, 0, 0, 6, 6,ETK_TABLE_NONE,0,0);
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
   UI_Param1Spinner = etk_spinner_new (0.0, 1.0, 0.0, 0.1, 1.0);
   etk_tooltips_tip_set(UI_Param1Spinner, "Description to set in the specified targets");
   etk_spinner_digits_set (ETK_SPINNER(UI_Param1Spinner), 1);
   etk_widget_size_request_set(UI_Param1Spinner,45, 20);
   etk_table_attach_default (ETK_TABLE(table),UI_Param1Spinner, 3, 3, 7, 7);


   //UI_TransiComboBox
   UI_TransiLabel = etk_label_new("<b>Transition</b>");
   etk_table_attach(ETK_TABLE(table), UI_TransiLabel, 0, 0, 8, 8,ETK_TABLE_NONE,0,0);
   UI_TransiComboBox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(UI_TransiComboBox),
      ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(UI_TransiComboBox),
      ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_build(ETK_COMBOBOX(UI_TransiComboBox));
   item = etk_combobox_item_append(ETK_COMBOBOX(UI_TransiComboBox),
                  etk_image_new_from_edje (EdjeFile,"DESC.PNG"), "Linear");
   etk_combobox_item_data_set (item, ENGRAVE_TRANSITION_LINEAR);
   item = etk_combobox_item_append(ETK_COMBOBOX(UI_TransiComboBox),
                  etk_image_new_from_edje (EdjeFile,"DESC.PNG"), "Sinusoidal");
   etk_combobox_item_data_set (item, (void*)ENGRAVE_TRANSITION_SINUSOIDAL);
   item = etk_combobox_item_append(ETK_COMBOBOX(UI_TransiComboBox),
                  etk_image_new_from_edje (EdjeFile,"DESC.PNG"), "Accelerate");
   etk_combobox_item_data_set (item, (void*)ENGRAVE_TRANSITION_ACCELERATE);
   item = etk_combobox_item_append(ETK_COMBOBOX(UI_TransiComboBox),
                  etk_image_new_from_edje (EdjeFile,"DESC.PNG"), "Decelerate");
   etk_combobox_item_data_set (item, (void*)ENGRAVE_TRANSITION_DECELERATE);
   etk_table_attach_default (ETK_TABLE(table),UI_TransiComboBox, 1, 1, 8, 8);

   //DurationSpinner
   UI_DurationLabel = etk_label_new("<b>seconds</b>");
   etk_table_attach(ETK_TABLE(table), UI_DurationLabel, 2, 2, 8, 8,ETK_TABLE_NONE,0,0);
   UI_DurationSpinner = etk_spinner_new (0.0, 999.0, 0.0, 0.1, 1.0);
   etk_tooltips_tip_set(UI_DurationSpinner, "The duration of the transition. In seconds.");
   etk_spinner_digits_set (ETK_SPINNER(UI_DurationSpinner), 1);
   etk_widget_size_request_set(UI_DurationSpinner,45, 20);
   etk_table_attach_default (ETK_TABLE(table),UI_DurationSpinner, 3, 3, 8, 8);

   //UI_Param2Entry
   UI_Param2Label = etk_label_new("<b>Source</b>");
   etk_table_attach(ETK_TABLE(table), UI_Param2Label, 0, 0, 9, 9,ETK_TABLE_NONE,0,0);
   UI_Param2Entry = etk_entry_new();
   etk_tooltips_tip_set(UI_Param2Entry, "!!!!!!");
   etk_table_attach_default(ETK_TABLE(table), UI_Param2Entry, 1, 2, 9, 9);

   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_ActionComboBox),
         ETK_CALLBACK(on_ActionComboBox_changed), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_ProgramEntry),
         ETK_CALLBACK(on_ProgramEntry_text_changed), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(UI_SourceEntry))),
         ETK_CALLBACK(on_SourceEntry_text_changed), UI_SourceEntry);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_SourceEntry),
         ETK_CALLBACK(on_SourceEntry_item_changed), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_SignalEntry),
         ETK_CALLBACK(on_SignalEntry_text_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_DelayFromSpinner),
         ETK_CALLBACK(on_DelaySpinners_value_changed),NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_DelayRangeSpinner),
         ETK_CALLBACK(on_DelaySpinners_value_changed),NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_TargetEntry),
         ETK_CALLBACK(on_TargetEntry_text_changed), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_Param1Entry),
         ETK_CALLBACK(on_Param1Entry_text_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_Param1Spinner),
         ETK_CALLBACK(on_Param1Spinner_value_changed), NULL);
   etk_signal_connect("active-item-changed", ETK_OBJECT(UI_TransiComboBox),
         ETK_CALLBACK(on_TransitionComboBox_changed), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_DurationSpinner),
         ETK_CALLBACK(on_DurationSpinner_value_changed), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_AfterEntry),
         ETK_CALLBACK(on_AfterEntry_text_changed), NULL);
   etk_signal_connect("text-changed", ETK_OBJECT(UI_Param2Entry),
         ETK_CALLBACK(on_Param2Entry_text_changed), NULL);

   return table;
}

Etk_Widget*
create_script_frame(void)
{
   Etk_Widget *sv;
   Etk_Widget *vbox;
   
   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 0);

   //ScriptBox
   UI_ScriptBox = etk_text_view_new();
   sv = etk_scrolled_view_new();
   etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(sv),
                                 ETK_POLICY_AUTO, ETK_POLICY_AUTO);
   etk_bin_child_set(ETK_BIN(sv), UI_ScriptBox);
   etk_box_append(ETK_BOX(vbox), sv, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //ScriptSaveButton
   UI_ScriptSaveButton = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE);
   etk_object_properties_set(ETK_OBJECT(UI_ScriptSaveButton),
                              "label","Save script",NULL);
   etk_box_append(ETK_BOX(vbox), UI_ScriptSaveButton,
                  ETK_BOX_START, ETK_BOX_NONE, 0);

   etk_signal_connect("clicked", ETK_OBJECT(UI_ScriptSaveButton),
            ETK_CALLBACK(on_AllButton_click), (void*)SAVE_SCRIPT);
 
   return vbox;
}

void
create_main_window(void)
{

    
   //Create the man ecore_evas window
   UI_ecore_MainWin = ecore_evas_software_x11_new(NULL, 0, 0, 0, 32, 32);
   ecore_evas_title_set(UI_ecore_MainWin, "Edje Editor");
   ecore_evas_callback_resize_set(UI_ecore_MainWin, ecore_resize_callback);
   ecore_evas_callback_delete_request_set(UI_ecore_MainWin, ecore_delete_cb);
   ecore_evas_resize(UI_ecore_MainWin, 950, 500);
   ecore_evas_size_min_set (UI_ecore_MainWin, 600, 350);
   ecore_evas_show(UI_ecore_MainWin);
   UI_evas = ecore_evas_get(UI_ecore_MainWin);
   
   //Load main edje interface
   edje_ui = edje_object_add(UI_evas);
   edje_object_file_set(edje_ui, EdjeFile, "MainUI");
   evas_object_move(edje_ui, 0, 0);
   evas_object_show(edje_ui);
   
   //Tooltips
   etk_tooltips_init ();
   etk_tooltips_enable();

   //Create the evas objects needed by the canvas (fakewin, handlers)
   prepare_canvas();

   //Toolbar
   create_toolbar(ETK_TOOLBAR_HORIZ);
   UI_ToolbarEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_ToolbarEmbed), UI_Toolbar);
   etk_embed_position_method_set(ETK_EMBED(UI_ToolbarEmbed),
                                 _embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_ToolbarEmbed);
   evas_object_move(etk_embed_object_get(ETK_EMBED(UI_ToolbarEmbed)), 130, 0);
   evas_object_resize(etk_embed_object_get(ETK_EMBED(UI_ToolbarEmbed)), 0, 50);
    
   //Tree
   create_tree();
   UI_PartsTreeEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_PartsTreeEmbed), UI_PartsTree);
   etk_embed_position_method_set(ETK_EMBED(UI_PartsTreeEmbed),
                                 _embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_PartsTreeEmbed);
   
   //Logo
   logo = edje_object_add(UI_evas);
   edje_object_file_set(logo, EdjeFile, "Logo");
   evas_object_show(logo);
   
   //GroupEmbed
   UI_GroupEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_GroupEmbed), create_group_frame());
   etk_embed_position_method_set(ETK_EMBED(UI_GroupEmbed),
                                 _embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_GroupEmbed);
   edje_object_part_swallow(edje_ui,"group_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_GroupEmbed)));
    
   //PartEmbed
   UI_PartEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_PartEmbed), create_part_frame());
   etk_embed_position_method_set(ETK_EMBED(UI_PartEmbed),
                                 _embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_PartEmbed);
   edje_object_part_swallow(edje_ui,"part_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_PartEmbed)));

   //DescriptionEmbed
   UI_DescriptionEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_DescriptionEmbed), create_description_frame());
   etk_embed_position_method_set(ETK_EMBED(UI_DescriptionEmbed),
                                 _embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_DescriptionEmbed);
   edje_object_part_swallow(edje_ui,"description_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_DescriptionEmbed)));

   //RectEmbed
   UI_RectEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_RectEmbed), create_rectangle_frame());
   etk_embed_position_method_set(ETK_EMBED(UI_RectEmbed),
                                 _embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_RectEmbed);
   edje_object_part_swallow(edje_ui,"rect_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_RectEmbed)));

   //TextEmbed
   UI_TextEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_TextEmbed), create_text_frame());
   etk_embed_position_method_set(ETK_EMBED(UI_TextEmbed),
                                 _embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_TextEmbed);
   edje_object_part_swallow(edje_ui,"text_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_TextEmbed)));

   //ImageEmbed
   UI_ImageEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_ImageEmbed), create_image_frame());
   etk_embed_position_method_set(ETK_EMBED(UI_ImageEmbed),
                                 _embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_ImageEmbed);
   edje_object_part_swallow(edje_ui,"image_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_ImageEmbed)));

   //PositionEmbed
   UI_PositionEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_PositionEmbed), create_position_frame());
   etk_embed_position_method_set(ETK_EMBED(UI_PositionEmbed),
                                 _embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_PositionEmbed);
   edje_object_part_swallow(edje_ui,"position_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_PositionEmbed)));

   //ProgramEmbed
   UI_ProgramEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_ProgramEmbed), create_program_frame());
   etk_embed_position_method_set(ETK_EMBED(UI_ProgramEmbed),
                                 _embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_ProgramEmbed);
   edje_object_part_swallow(edje_ui,"program_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_ProgramEmbed)));

   //ScriptEmbed
   UI_ScriptEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_ScriptEmbed), create_script_frame());
   etk_embed_position_method_set(ETK_EMBED(UI_ScriptEmbed),
                                 _embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_ScriptEmbed);
   edje_object_part_swallow(edje_ui,"script_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_ScriptEmbed)));
   
   //Filechooser
   UI_FileChooserDialog = create_filechooser_dialog();

   //Alert Dialog
   UI_AlertDialog = etk_message_dialog_new (ETK_MESSAGE_DIALOG_INFO,
                                       ETK_MESSAGE_DIALOG_OK, "Hallo world!");
   etk_widget_size_request_set(UI_AlertDialog, 240, 100);
   etk_signal_connect("delete-event", ETK_OBJECT(UI_AlertDialog),
                      ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_signal_connect("response", ETK_OBJECT(UI_AlertDialog),
                      ETK_CALLBACK(on_AlertDialog_response), NULL);

   //ColorPicker
   UI_ColorWin = etk_window_new();
   etk_signal_connect("delete-event", ETK_OBJECT(UI_ColorWin),
                      ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   UI_ColorPicker = etk_colorpicker_new();
   etk_colorpicker_use_alpha_set (ETK_COLORPICKER(UI_ColorPicker), TRUE);
   etk_container_add(ETK_CONTAINER(UI_ColorWin), UI_ColorPicker);
   etk_signal_connect("color-changed", ETK_OBJECT(UI_ColorPicker),
                        ETK_CALLBACK(on_ColorDialog_change), NULL);

   /*edje_object_signal_emit(edje_ui,"group_frame_show","edje_editor");
   edje_object_signal_emit(edje_ui,"part_frame_show","edje_editor");
   edje_object_signal_emit(edje_ui,"description_frame_show","edje_editor");
   edje_object_signal_emit(edje_ui,"rect_frame_show","edje_editor");
   edje_object_signal_emit(edje_ui,"text_frame_show","edje_editor");
   edje_object_signal_emit(edje_ui,"position_frame_show","edje_editor");
   edje_object_signal_emit(edje_ui,"image_frame_show","edje_editor");
   edje_object_signal_emit(edje_ui,"program_frame_show","edje_editor");
   edje_object_signal_emit(edje_ui,"script_frame_show","edje_editor");*/
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
}

void _embed_position_set(void *position_data, int *x, int *y)
{
   ecore_evas_geometry_get(position_data, x, y, NULL, NULL);
}
