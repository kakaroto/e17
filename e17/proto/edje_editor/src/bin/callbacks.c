#include <string.h>
#include <glib.h>
#include <Edje.h> 
#include <Etk.h> 
#include "callbacks.h" 
#include "interface.h"
#include "main.h"
#include "evas.h"

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
}

/* Called when the the user change the group */
void on_GroupComboBox_changed(Etk_Combobox *combobox, void *data){
	EDC_Group	*group = NULL;
	GList		*p;
	Etk_Tree_Row  *selected_row = NULL;

	group = etk_combobox_item_data_get (etk_combobox_active_item_get (combobox));
	if (group){
		if (selected_group) ev_hide_group(selected_group);
		printf("Change group: %s\n",group->name->str);
		
		//Hide all the dynamic frames
		etk_widget_hide(UI_DescriptionFrame);
		etk_widget_hide(UI_PartFrame);
		etk_widget_hide(UI_PositionFrame);
		etk_widget_hide(UI_TextFrame);
		etk_widget_hide(UI_ImageFrame);
		etk_widget_hide(UI_RectFrame);
	
		//Hide all the tree and show the selected one
		p = EDC_Group_list;
		while (p){
			etk_widget_hide(ETK_WIDGET(((EDC_Group*)(p->data))->tree_widget));
			p = g_list_next(p);
		}	
		etk_widget_show(ETK_WIDGET(group->tree_widget));
		
		//Set the main selection vars
		selected_group = group;
		selected_part = NULL;
		selected_desc = NULL;
		
		//If a row is selected reselect it (because we have more tree)
		if ((selected_row = etk_tree_selected_row_get(ETK_TREE(group->tree_widget))))
			etk_tree_row_select (selected_row);
			
	
		ev_resize_fake(group->min_x,group->min_y);
		
		//Update Fakewin Title
		if (EV_fakewin) edje_object_part_text_set (EV_fakewin, "title", selected_group->name->str);
	
		//Update min e max spinners
		etk_range_value_set (ETK_RANGE(UI_GroupMinXSpinner), selected_group->min_x);
		etk_range_value_set (ETK_RANGE(UI_GroupMinYSpinner), selected_group->min_y);
		etk_range_value_set (ETK_RANGE(UI_GroupMaxXSpinner), selected_group->max_x);
		etk_range_value_set (ETK_RANGE(UI_GroupMaxYSpinner), selected_group->max_y);
		
		ev_draw_all();
	
	}
	
	
	//Update relto comboboxes
	RecreateRelComboBoxes();
}


/* Called when the the user change group spinners (min & max)*/
void on_GroupSpinner_value_changed(Etk_Range *range, double value, void *data){
	printf("Group Spinner value changed signal EMIT\n");
	if (selected_group){
		switch ((int)data){
			case MINX_SPINNER: selected_group->min_x = etk_range_value_get(range);break;
			case MINY_SPINNER: selected_group->min_y = etk_range_value_get(range);break;
			case MAXX_SPINNER: selected_group->max_x = etk_range_value_get(range);break;
			case MAXY_SPINNER: selected_group->max_y = etk_range_value_get(range);break;
		}
	}
}

/* Called when the the user Click a row on one of the trees */
void on_PartTree_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data){
	printf("Row Selected Signal on one of the Tree EMITTED (");
	
	//If has child is a part 
	if (etk_tree_row_first_child_get(row)){
		selected_part = etk_tree_row_data_get (row);
		selected_desc = NULL;
		etk_widget_hide(UI_DescriptionFrame);
		etk_widget_hide(UI_PositionFrame);
		etk_widget_hide(UI_RectFrame);
		etk_widget_hide(UI_ImageFrame);
		etk_widget_hide(UI_TextFrame);
		etk_widget_show(UI_PartFrame);
		UpdatePartFrame();
	}else{ // else is a description
		selected_desc = etk_tree_row_data_get (row);
		selected_part = selected_desc->part;
		selected_part->current_description = selected_desc;
	
		UpdateDescriptionFrame();
		UpdatePositionFrame();
		
		UpdateComboPositionFrame();
		
		if (selected_part->type == PART_TYPE_TEXT) UpdateTextFrame();
		if (selected_part->type == PART_TYPE_IMAGE) UpdateImageFrame();
		if (selected_part->type == PART_TYPE_RECT) UpdateRectFrame();
		
		
		etk_widget_hide(UI_PartFrame);
		etk_widget_show(UI_DescriptionFrame);
		etk_widget_show(UI_PositionFrame);
		
		if (selected_desc->part->type == PART_TYPE_RECT) etk_widget_show(UI_RectFrame);
		else etk_widget_hide(UI_RectFrame);
		
		if (selected_desc->part->type == PART_TYPE_IMAGE) etk_widget_show(UI_ImageFrame);
		else etk_widget_hide(UI_ImageFrame);
			
		if (selected_desc->part->type == PART_TYPE_TEXT) etk_widget_show(UI_TextFrame);
		else etk_widget_hide(UI_TextFrame);
			
	}

	
	
	ev_draw_all();
	
}
/* Position Frame Callbacks */
void on_RelToComboBox_changed(Etk_Combobox *combobox, void *data){
	EDC_Part* part = NULL;
	//printf("Rel2X\n");
	
	part = etk_combobox_item_data_get (etk_combobox_active_item_get (combobox));
	if (part){
		switch ((int)data){
			case REL1X_SPINNER:
				if ((int)part == REL_COMBO_INTERFACE) g_string_truncate(selected_desc->rel1_to_x,0);	
				else g_string_printf(selected_desc->rel1_to_x,"%s",part->name->str);
			break;
			case REL1Y_SPINNER:
				if ((int)part == REL_COMBO_INTERFACE) g_string_truncate(selected_desc->rel1_to_y,0);	
				else g_string_printf(selected_desc->rel1_to_y,"%s",part->name->str);
			break;
			case REL2X_SPINNER:
				if ((int)part == REL_COMBO_INTERFACE) g_string_truncate(selected_desc->rel2_to_x,0);	
				else g_string_printf(selected_desc->rel2_to_x,"%s",part->name->str);
			break;
			case REL2Y_SPINNER:
				if ((int)part == REL_COMBO_INTERFACE) g_string_truncate(selected_desc->rel2_to_y,0);	
				else g_string_printf(selected_desc->rel2_to_y,"%s",part->name->str);
			break;
		}
	
	}
	
	ev_draw_all();
}


void on_RelSpinner_value_changed(Etk_Range *range, double value, void *data){
 	printf("Value Changed Signal on RelSpinner EMITTED\n");
	//printf("Value %f",etk_range_value_get(range));
	if (selected_desc){
		switch ((int)data){
			case REL1X_SPINNER: selected_desc->rel1_relative_x = etk_range_value_get(range);break;
			case REL1Y_SPINNER: selected_desc->rel1_relative_y = etk_range_value_get(range);break;
			case REL2X_SPINNER: selected_desc->rel2_relative_x = etk_range_value_get(range);break;
			case REL2Y_SPINNER: selected_desc->rel2_relative_y = etk_range_value_get(range);break;
		}
		
		ev_draw_all();
		ev_draw_focus();
	}
}
void on_RelOffsetSpinner_value_changed(Etk_Range *range, double value, void *data){
 	printf("Value Changed Signal on RelSpinner EMITTED\n");
	ShowAlert("Not yet implemented");

}
/* Image Frame Callbacks */
void on_ImageComboBox_changed(Etk_Combobox *combobox, void *data){
	printf("Image Combo Changed\n");
	char* image;
	if ((image = etk_combobox_item_data_get(etk_combobox_active_item_get (combobox)))){
		//Set an existing image
		if (selected_desc){
			g_string_printf(selected_desc->image_normal,"%s",image);
			ev_draw_part(selected_part);
			
		}
	}else{
		//Insert a new image in EDC
		printf("INSERT IMAGE\n");
		ShowFilechooser(FILECHOOSER_IMAGE);
	}
}
void on_BorderSpinner_value_changed(Etk_Range *range, double value, void *data){
	printf("Value %f\n",etk_range_value_get(range));
	if (selected_desc){
		switch ((int)data){
			case BORDER_TOP: selected_desc->image_border_top = etk_range_value_get(range);break;
			case BORDER_BOTTOM: selected_desc->image_border_bottom = etk_range_value_get(range);break;
			case BORDER_LEFT: selected_desc->image_border_left = etk_range_value_get(range);break;
			case BORDER_RIGHT: selected_desc->image_border_right = etk_range_value_get(range);break;
		}
		ev_draw_part(selected_part);
	}
}

void on_ImageAlphaSlider_value_changed(Etk_Object *object, double value, void *data){
	printf("ImageSlieder value_changed signale EMIT: %.2f\n",value);
	if (selected_desc){
		selected_desc->color_a = (int)value;
		ev_draw_part(selected_desc->part);
	}
}
/* Text Frame Callbacks */
void on_FontComboBox_changed(Etk_Combobox *combobox, void *data){
	printf("Font Combo Changed\n");
	char* font;
	if ((font = etk_combobox_item_data_get(etk_combobox_active_item_get (combobox)))){
		//Set an existing font
		if (selected_desc){
			g_string_printf(selected_desc->text_font,"%s",font);
			ev_draw_part(selected_part);
			
		}
	}else{
		//Insert a new font in EDC
		printf("INSERT FONT\n");
		ShowFilechooser(FILECHOOSER_FONT);
	}
}
void on_EffectComboBox_changed(Etk_Combobox *combobox, void *data){
	int effect;
	printf("Effect Combo Changed\n");
	if (selected_part){
		if ((effect = (int)etk_combobox_item_data_get(etk_combobox_active_item_get (combobox)))){
			selected_part->effect = effect;
		}
		ev_draw_all();
	}
}
void on_FontSizeSpinner_value_changed(Etk_Range *range, double value, void *data){
	printf("Set font size to: %d\n",(int)etk_range_value_get(range));
	selected_desc->text_size = (int)etk_range_value_get(range);
	ev_draw_part(selected_part);
}


void on_TextEntry_text_changed(Etk_Object *object, void *data){
 	printf("TEXT CHANGED\n");
	
	//Set the new value in selected_desc->text_text
	g_string_printf(selected_desc->text_text,"%s",etk_entry_text_get(ETK_ENTRY(object)));

	ev_draw_part(selected_part); 
}


void on_TextAlphaSlider_value_changed(Etk_Object *object, double value, void *data){
	printf("value changed event on text alpha slider EMIT\n");
	if (selected_desc){
		selected_desc->color_a = (int)value;
		ev_draw_part(selected_desc->part);
	}
}
/* Colors Callbacks */
void on_ColorCanvas_realize(Etk_Widget *canvas, void *data){
	//Must use the realize callback on the EtkCanvas object.
	//Because I can't add any object to the canvas before it is realized
	Evas_Object* rect;
	//Add the colored rectangle
	rect = evas_object_rectangle_add  (etk_widget_toplevel_evas_get(canvas));   	
	etk_canvas_object_add (ETK_CANVAS(canvas), rect);
	evas_object_color_set(rect, 100,100,100,255);
	evas_object_resize(rect,30,30);
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
void on_ColorCanvas_click(void *data, Evas *e, Evas_Object *obj, void *event_info){
	printf("click\n");
	if (UI_ColorWin) etk_widget_show_all(UI_ColorWin);
	current_color_object = (int)data;
}
void on_ColorAlphaSlider_value_changed(Etk_Object *object, double value, void *data){
	char string[256];
	
	selected_desc->color_a = (int)value;
	
	snprintf(string, 255, "%03.0f", value);
	etk_label_set(ETK_LABEL(data), string);
	
	evas_object_color_set(RectColorObject,selected_desc->color_r,selected_desc->color_g,selected_desc->color_b,255);
	ev_draw_part(selected_desc->part);

}
void on_ColorDialog_change(Etk_Object *object, void *data){
	Etk_Color color;
	
	color = etk_colorpicker_current_color_get (ETK_COLORPICKER(object));
	switch (current_color_object){
		case COLOR_OBJECT_RECT:
			evas_object_color_set(RectColorObject,color.r,color.g,color.b,color.a);
			selected_desc->color_r = color.r;
			selected_desc->color_g = color.g;
			selected_desc->color_b = color.b;
		break;
		case COLOR_OBJECT_TEXT:
			evas_object_color_set(TextColorObject,color.r,color.g,color.b,color.a);
			selected_desc->color_r = color.r;
			selected_desc->color_g = color.g;
			selected_desc->color_b = color.b;
		break;
		case COLOR_OBJECT_SHADOW:
			evas_object_color_set(ShadowColorObject,color.r,color.g,color.b,color.a);
			selected_desc->color2_r = color.r;
			selected_desc->color2_g = color.g;
			selected_desc->color2_b = color.b;
		break;
			case COLOR_OBJECT_OUTLINE:
			evas_object_color_set(OutlineColorObject,color.r,color.g,color.b,color.a);
			selected_desc->color3_r = color.r;
			selected_desc->color3_g = color.g;
			selected_desc->color3_b = color.b;
		break;
	}
	
	ev_draw_part(selected_desc->part);
}

/* Parts & Descriptions Callbacks*/
void on_PartNameEntry_text_changed(Etk_Object *object, void *data){
 	Etk_Tree_Col *col1=NULL;
	
 	printf("Text Changed Signal on PartNameEntry EMITTED (text: %s)\n",etk_entry_text_get(ETK_ENTRY(object)));
	
 	if (selected_part){
		RenamePart(selected_part,(char*)etk_entry_text_get(ETK_ENTRY(object)));
	
		//Update PartTree	
		col1 = etk_tree_nth_col_get(ETK_TREE(selected_group->tree_widget), 0);
		switch (selected_part->type){
			case PART_TYPE_IMAGE: etk_tree_row_fields_set(selected_part->tree_row,col1,EdjeFile->str,"IMAGE.PNG",etk_entry_text_get(ETK_ENTRY(object)),NULL); break;
			case PART_TYPE_RECT: etk_tree_row_fields_set(selected_part->tree_row,col1,EdjeFile->str,"RECT.PNG",etk_entry_text_get(ETK_ENTRY(object)),NULL); break;
			case PART_TYPE_TEXT: etk_tree_row_fields_set(selected_part->tree_row,col1,EdjeFile->str,"TEXT.PNG",etk_entry_text_get(ETK_ENTRY(object)),NULL); break;
			default: etk_tree_row_fields_set(selected_part->tree_row,col1,EdjeFile->str,"NONE.PNG",etk_entry_text_get(ETK_ENTRY(object)),NULL); break;
		}
	}
}
void on_StateEntry_text_changed(Etk_Object *object, void *data){
	Etk_Tree_Col *col1=NULL;
	GString *str = g_string_new("");
	printf("Text Changed Signal on StateEntry EMITTED\n");
	
	RenameDescription(selected_desc,(char*)etk_entry_text_get(ETK_ENTRY(object)),selected_desc->state_index);
	
 	//Update PartTree
	if (selected_desc){
		col1 = etk_tree_nth_col_get(ETK_TREE(selected_group->tree_widget), 0);
		g_string_printf(str,"%s %.2f",selected_desc->state->str, selected_desc->state_index);
		etk_tree_row_fields_set(selected_desc->tree_row,col1,EdjeFile->str,"DESC.PNG",str->str,NULL);
	}

	g_string_free(str,TRUE); 
}


void on_StateIndexSpinner_value_changed(Etk_Range *range, double value, void *data){
	Etk_Tree_Col *col1=NULL;
	GString *str = g_string_new("");
	printf("Value Changed Signal on StateIndexSpinner EMITTED\n");
	if (selected_desc){
		RenameDescription(selected_desc,NULL,etk_range_value_get(range));
		
		//Update PartTree
		col1 = etk_tree_nth_col_get(ETK_TREE(selected_group->tree_widget), 0);
		g_string_printf(str,"%s %.1f",selected_desc->state->str, selected_desc->state_index);
		etk_tree_row_fields_set(selected_desc->tree_row,col1,EdjeFile->str,"DESC.PNG",str->str,NULL);
	}
	g_string_free(str,TRUE); 
}
/* Toolbar Callbacks */
void on_ToolBarButton_click(Etk_Button *button, void *data){
	GList *current,*prev;
	switch ((int)data){
		case TOOLBAR_NEW:
			printf("Clicked signal on Toolbar Button 'New' EMITTED\n");
			ShowFilechooser(FILECHOOSER_NEW);
		break;
		case TOOLBAR_OPEN:
			printf("Clicked signal on Toolbar Button 'Open' EMITTED\n");
			ShowFilechooser(FILECHOOSER_OPEN);
		break;
		case TOOLBAR_SAVE:
			printf("Clicked signal on Toolbar Button 'Save' EMITTED\n");
			SaveEDC(NULL);
		break;
		case TOOLBAR_SAVE_AS:
			printf("Clicked signal on Toolbar Button 'Save as' EMITTED\n");
			ShowFilechooser(FILECHOOSER_SAVE_AS);
		break;
		case TOOLBAR_ADD:
			printf("Clicked signal on Toolbar Button 'Add' EMITTED\n");
			etk_menu_popup(ETK_MENU(UI_AddMenu));
			//etk_menu_popup_at_xy (ETK_MENU(AddMenu), 10, 10);
		break;
		case TOOLBAR_REMOVE:
			printf("Clicked signal on Toolbar Button 'Remove' EMITTED\n");
			etk_menu_popup(ETK_MENU(UI_RemoveMenu));
			//etk_menu_popup_at_xy (ETK_MENU(AddMenu), 10, 10);
		break;
		case TOOLBAR_MOVE_UP:
			printf("Clicked signal on Toolbar Button 'MoveUp' EMITTED\n");
			if (selected_desc){
				printf("MoveUP DESC: %s\n",selected_desc->state->str);
			}else if (selected_part){
				if ((current = g_list_find(selected_part->group->parts,selected_part))){
					if ((prev = g_list_previous(current))){
						printf("MoveUP PART: %s\n",selected_part->name->str);
						//current->data = prev->data;
						//prev->data = selected_part;
						//Update the tree
					}
				}
				
			}else{
				//TODO avvisare l'utente che non si ha selezionato nulla
			}
			ShowAlert("Not yet implemented");
		break;
		case TOOLBAR_MOVE_DOWN:
			printf("Clicked signal on Toolbar Button 'MoveDown' EMITTED\n");
			ShowAlert("Not yet implemented");
		break;
		case TOOLBAR_PLAY:
			printf("Clicked signal on Toolbar Button 'Play' EMITTED\n");
			SaveEDC(NULL);
			PlayEDC();
		break;
		case TOOLBAR_DEBUG:
			PrintDebugInformation(TRUE);
			//etk_menu_popup_at_xy (ETK_MENU(AddMenu), 10, 10);
		break;
	} 
}
void on_AddMenu_item_activated(Etk_Object *object, void *data){
	EDC_Group *group = NULL;
	EDC_Group *group2 = NULL;
	EDC_Part	*part;
	EDC_Description *new_desc;
	GList* p;
	int i;
	printf("Item Activated Signal on AddMenu EMITTED\n");
	switch ((int)data){
		case NEW_RECT:
			//printf("RECT\n");
			if (selected_group){
				part = EDC_Part_new(selected_group,"New Rectangle",PART_TYPE_RECT);
				new_desc = EDC_Description_new(part,"default",0);
				part->descriptions = g_list_append(part->descriptions,new_desc);
				AddPartToTree(part);
				
				selected_part = part;
				selected_desc = part->descriptions->data;
				
				etk_tree_row_select(selected_desc->tree_row);
				etk_tree_row_expand (selected_part->tree_row);
				
				RecreateRelComboBoxes();
			}
			
		break;
		case NEW_IMAGE:
			//printf("IMAGE\n");
			if (selected_group){
				part = EDC_Part_new(selected_group,"New Image",PART_TYPE_IMAGE);
				new_desc = EDC_Description_new(part,"default",0);

				part->descriptions = g_list_append(part->descriptions,new_desc);
				AddPartToTree(part);
				selected_part = part;
				selected_desc = part->descriptions->data;
				etk_tree_row_select(selected_desc->tree_row);
				etk_tree_row_expand (selected_part->tree_row);
				
				RecreateRelComboBoxes();
			}
		break;
		case NEW_TEXT:
			//printf("TEXT\n");
			if (selected_group){
				part = EDC_Part_new(selected_group,"New Text", PART_TYPE_TEXT);
				new_desc = EDC_Description_new(part,"default",0);
				
				part->type = PART_TYPE_TEXT;
				g_string_printf(part->name,"New Text");
				part->descriptions = g_list_append(part->descriptions,new_desc);
				AddPartToTree(part);
				selected_part = part;
				selected_desc = part->descriptions->data;
				etk_tree_row_select(selected_desc->tree_row);
				etk_tree_row_expand (selected_part->tree_row);
				
				RecreateRelComboBoxes();
			}
		break;
		case NEW_DESC:
			if (selected_part){
				printf("New Description in %s\n",selected_part->name->str);
				new_desc = EDC_Description_new(selected_part,"New state",0);
				
				selected_part->descriptions = g_list_append(selected_part->descriptions,new_desc);
				AddDescToTree(new_desc);
				selected_desc = new_desc;
				etk_tree_row_select(selected_desc->tree_row);
				etk_tree_row_expand (selected_part->tree_row);
			}else{
				ShowAlert("You must first select a part.");
			}
		break;
		case NEW_GROUP:
			group = EDC_Group_new("Nuovo Gruppo",100,100,400,400);
			
			//count groups to set the last item in the comboboxx
			p = EDC_Group_list;
			i=0;
			while (p){
				group2 = p->data; 
				p = g_list_next(p);
				i++;
			}
			etk_combobox_active_item_set(ETK_COMBOBOX(UI_GroupComboBox),etk_combobox_nth_item_get (ETK_COMBOBOX(UI_GroupComboBox), i-1));
		break;
	}
}
void on_RemoveMenu_item_activated(Etk_Object *object, void *data){
	Etk_Tree_Row* row;
	printf("Item Activated Signal on RemoveMenu EMITTED\n");
	switch ((int)data){
		case REMOVE_DESCRIPTION:
			if (selected_desc){
				if (strcmp(selected_desc->state->str,"default") || selected_desc->state_index != 0){
					printf("REMOVE DESCRIPTION: %s\n",selected_desc->state->str);
					row = etk_tree_prev_row_get(selected_desc->tree_row, TRUE, TRUE);
					EDC_Description_clear(selected_desc,FALSE);
					selected_desc = NULL;
					etk_tree_row_select (row);
				}else{
					ShowAlert("You can't remove default 0.0");
				}
			}else{
				ShowAlert("No description selected");
			}
			
		break;
		case REMOVE_PART:
			if (selected_part){
				printf("REMOVE PART: %s\n",selected_part->name->str);
				row = NULL;
				row = etk_tree_next_row_get(selected_part->tree_row, FALSE, FALSE);
				EDC_Part_clear(selected_part, FALSE);
				selected_part = NULL;
				selected_desc = NULL;
				if (row) etk_tree_row_select (row);
				else etk_tree_row_select(etk_tree_last_row_get (ETK_TREE(selected_group->tree_widget),TRUE,FALSE));
				//ev_draw_focus();
			}else{
				ShowAlert("No part selected");
			}
		break; 
		case REMOVE_GROUP:
			if (selected_group) {
				if (g_list_length(EDC_Group_list) > 1){
					EDC_Group_clear(selected_group,FALSE);
					selected_group = EDC_Group_list->data;
				}else{
					ShowAlert("You can't remove the last group");
				}
			}
		break;
	}
}

/* Dialogs Callbacks */
void on_FileChooser_response(Etk_Dialog *dialog, int response_id, void *data){
	GString *edc_file = g_string_new("");
	
	printf("Response Signal on Filechooser EMITTED\n");
	
	if (response_id == ETK_RESPONSE_OK){
		
		switch(FileChooserOperation){
			case FILECHOOSER_OPEN:
				g_string_printf(edc_file,"%s/%s",etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)), etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
				ClearAll();
		
				OpenEDC(edc_file->str);
				UpdateGroupComboBox();
			break;
			case FILECHOOSER_NEW:
				//printf("new: %s\n",etk_entry_text_get(UI_FilechooserFileNameEntry));
				ClearAll();
				CreateBlankEDC();
				UpdateGroupComboBox();
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
			break;
		}
		
		
		etk_widget_hide(ETK_WIDGET(dialog));
	}else{
		etk_widget_hide(ETK_WIDGET(dialog));
	}
	
	g_string_free(edc_file,TRUE);
}
void on_FileChooser_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data){
	GString *str=g_string_new("");
	if (etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)))
		g_string_append_printf(str,"%s/",etk_filechooser_widget_current_folder_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
	if (etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)))
		str = g_string_append(str,etk_filechooser_widget_selected_file_get (ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
	
	//printf("CHANGE: %s\n",str->str);
	etk_entry_text_set(ETK_ENTRY(UI_FilechooserFileNameEntry),str->str);
	g_string_free(str,TRUE);
}
void on_PlayDialog_response(Etk_Dialog *dialog, int response_id, void *data){
	GString *command = g_string_new("");
	if (response_id == ETK_RESPONSE_OK){
		printf("TEST IN VIEWER\n");
		g_string_printf(command,"edje_viewer %s",EDCFile->str);
		command->str[command->len - 1] = 'j'; 
		command = g_string_append(command," &");
		printf("TEST IN VIEWER %s\n",command->str);
		system(command->str); 
		etk_widget_hide(UI_PlayDialog);
	}else{
		etk_widget_hide(ETK_WIDGET(dialog));
	}
	g_string_free(command,TRUE);
}
void on_AlertDialog_response(Etk_Dialog *dialog, int response_id, void *data){
	etk_widget_hide(ETK_WIDGET(dialog));
}


