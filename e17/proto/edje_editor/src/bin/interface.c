#include <dirent.h>
#include <string.h>
#include <glib.h> 
#include <Etk.h>
#include "main.h" 
#include "callbacks.h"
#include "interface.h"


void ShowAlert(char* text){
	etk_message_dialog_text_set (ETK_MESSAGE_DIALOG(UI_AlertDialog), text);
	etk_widget_show_all(UI_AlertDialog);
}
//Add a font to the combo filtering only font file
int AddFontToCombo(char* fname){
	Etk_Combobox_Item *ComboItem; 
	char *ext;
	if ((strcmp(fname, ".") == 0) || (strcmp(fname, "..") == 0) || (strlen(fname) < 5)){
		return FALSE;
	}
	
	ext = fname + strlen(fname) - 4 ;

	if (strcmp(ext,".ttf")) return FALSE;
		
	//printf("	%s - %s\n",fname,ext);
	
	ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_FontComboBox), etk_image_new_from_stock (ETK_STOCK_PREFERENCES_DESKTOP_FONT,ETK_STOCK_MEDIUM), fname);
	etk_combobox_item_data_set (ComboItem, strdup(fname));
	return TRUE;
} 
//Add an image to the combo filtering only the image files
int AddImageToCombo(char* fname){
	GString *str = g_string_new("");
	Etk_Combobox_Item *ComboItem; 
	char *ext;
	if ((strcmp(fname, ".") == 0) || (strcmp(fname, "..") == 0) || (strlen(fname) < 5)){
		return FALSE;
	}
	
	ext = fname + strlen(fname) - 4 ;

	if (strcmp(ext,".png") && strcmp(ext,".gif")) return FALSE;
		
	//printf("IMAGE:	%s - %s\n",fname,ext);
	g_string_printf(str,"%s/%s",EDCFileDir->str,fname);
	ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_ImageComboBox), etk_image_new_from_file (str->str), fname);
	etk_combobox_item_data_set (ComboItem, strdup(fname));
	return TRUE;
} 

/* functions to update interface*/
void UpdateGroupComboBox(void){
	EDC_Group*	group;
	GList *p;
	Etk_Combobox_Item *ComboItem; 	
	if (EDC_Group_list){
		//printf("Update Group Combo with %d groups\n",g_list_length(EDC_Group_list));
		if (ETK_IS_COMBOBOX(UI_GroupComboBox))
			etk_combobox_clear(ETK_COMBOBOX(UI_GroupComboBox));
		
		p = EDC_Group_list;
		while(p){
			group = p->data;
			ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_GroupComboBox), group->name->str);
			etk_combobox_item_data_set (ComboItem, group);
			p = g_list_next(p);
		} 
		
		etk_signal_emit_by_name ("active_item_changed", ETK_OBJECT(UI_GroupComboBox),NULL,NULL);
	}
	
}

void UpdateFontComboBox(void){
	struct dirent   *dit;
	DIR *dir;
	Etk_Combobox_Item *ComboItem; 
	
	//Stop signal propagation 
	etk_signal_disconnect("active_item_changed", ETK_OBJECT(UI_FontComboBox), ETK_CALLBACK(on_FontComboBox_changed));

	if (EDCFileDir->len > 0){
		printf("Update Font Combo %s\n",EDCFileDir->str);
		
		etk_combobox_clear(ETK_COMBOBOX(UI_FontComboBox));
		
		//Scan the EDC Direcory to find font files
		dir = opendir(EDCFileDir->str);
			
		if (dir){
			while ((dit = readdir(dir)) != NULL){
				AddFontToCombo(dit->d_name);
			}
			closedir(dir);
			//Add the 'Insert new font' item at the end of the font list
 			ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_FontComboBox), etk_image_new_from_stock (ETK_STOCK_DOCUMENT_OPEN,ETK_STOCK_MEDIUM), "Insert new");
			etk_combobox_item_data_set (ComboItem, NULL);
		}
	}

	//Renable  signal propagation 
	etk_signal_connect("active_item_changed", ETK_OBJECT(UI_FontComboBox), ETK_CALLBACK(on_FontComboBox_changed), NULL);

}
void UpdateImageComboBox(void){
	struct dirent   *dit;
	DIR *dir;
	Etk_Combobox_Item *ComboItem; 
	
	//Stop signal propagation 
	etk_signal_disconnect("active_item_changed", ETK_OBJECT(UI_ImageComboBox), ETK_CALLBACK(on_ImageComboBox_changed));

	if (EDCFileDir->len > 0){
		printf("Update Image Combo %s\n",EDCFileDir->str);
		
		etk_combobox_clear(ETK_COMBOBOX(UI_ImageComboBox));
		
		//Scan the EDC Direcory to find font files
		dir = opendir(EDCFileDir->str);
			
		if (dir){
			while ((dit = readdir(dir)) != NULL){
				AddImageToCombo(dit->d_name);
			}
			closedir(dir);
			//Add the 'Insert new image' item at the end of the combo
 			ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_ImageComboBox), etk_image_new_from_stock (ETK_STOCK_DOCUMENT_OPEN,ETK_STOCK_MEDIUM), "Insert new");
			etk_combobox_item_data_set (ComboItem, NULL);
		}
	}

	//Renable  signal propagation 
	etk_signal_connect("active_item_changed", ETK_OBJECT(UI_ImageComboBox), ETK_CALLBACK(on_ImageComboBox_changed), NULL);

}
void UpdatePartFrame(void){
	if (selected_part){
		printf("Update Part Frame: %s\n",selected_part->name->str);
		
		//Stop signal propagation 
		etk_signal_disconnect("text_changed",ETK_OBJECT(UI_PartNameEntry),on_PartNameEntry_text_changed);
		
		//Update Part
		etk_entry_text_set(ETK_ENTRY(UI_PartNameEntry),selected_part->name->str);
		
		//ReEnable Signal Propagation
		etk_signal_connect("text_changed",ETK_OBJECT(UI_PartNameEntry),on_PartNameEntry_text_changed,NULL);
		
	}
}
void UpdateDescriptionFrame(void){
	if (selected_desc){
		printf("Update Description Frame: %s\n",selected_desc->state->str);
		
		//Stop signal propagation 
		etk_signal_disconnect("text_changed",ETK_OBJECT(UI_StateEntry),on_StateEntry_text_changed);
		etk_signal_disconnect("value_changed", ETK_OBJECT(UI_StateIndexSpinner), ETK_CALLBACK(on_StateIndexSpinner_value_changed));
		
		//Set description name & index
		etk_entry_text_set(ETK_ENTRY(UI_StateEntry),selected_desc->state->str);
		etk_range_value_set (ETK_RANGE(UI_StateIndexSpinner), selected_desc->state_index);
		
		//ReEnable Signal Propagation
		etk_signal_connect("text_changed",ETK_OBJECT(UI_StateEntry),on_StateEntry_text_changed,NULL);
		etk_signal_connect("value_changed", ETK_OBJECT(UI_StateIndexSpinner), ETK_CALLBACK(on_StateIndexSpinner_value_changed), NULL);
	}

}
void UpdateRectFrame(void){
	Etk_Color color;
	
	if (selected_desc){
		printf("Update Rect Frame: %s (%d %d %d %d)\n",selected_desc->state->str,selected_desc->color_r,selected_desc->color_g,selected_desc->color_b,selected_desc->color_a);


		//Set ColorPicker
		color.r = selected_desc->color_r;
		color.g = selected_desc->color_g;
		color.b = selected_desc->color_b;
		color.a = selected_desc->color_a;
	
		etk_colorpicker_current_color_set (ETK_COLORPICKER(UI_ColorPicker), color);

		//Set alpha slider
		etk_range_value_set(ETK_RANGE(UI_ColorAlphaSlider),selected_desc->color_a);
		
		//Set Color rect
		evas_object_color_set(RectColorObject,selected_desc->color_r,selected_desc->color_g,selected_desc->color_b,255);
	}
}
void UpdateImageFrame(void){
	GString	*str = g_string_new("");
	char* image;
	int i;
	Etk_Combobox_Item *item = NULL;
	//Stop signal propagation 
	etk_signal_disconnect("value_changed",ETK_OBJECT(UI_BorderLeftSpinner),on_BorderSpinner_value_changed);
	etk_signal_disconnect("value_changed",ETK_OBJECT(UI_BorderRightSpinner),on_BorderSpinner_value_changed);
	etk_signal_disconnect("value_changed",ETK_OBJECT(UI_BorderTopSpinner),on_BorderSpinner_value_changed);
	etk_signal_disconnect("value_changed",ETK_OBJECT(UI_BorderBottomSpinner),on_BorderSpinner_value_changed);
	etk_signal_disconnect("value_changed", ETK_OBJECT(UI_ImageAlphaSlider), on_ImageAlphaSlider_value_changed);

	//Set the images combobox
	if (selected_desc->image_normal->len > 0){
		i=0;
		while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_ImageComboBox),i))){	//Loop for all the item in the Combobox
			if ((image = etk_combobox_item_data_get(item))){							//Get the data for the item (should be an char* with the name of the image file)
				if (strcmp(image ,selected_desc->image_normal->str) == 0) 
					etk_combobox_active_item_set (ETK_COMBOBOX(UI_ImageComboBox),item);	//If we found the item set active
			}
		i++;
		}
	}else{etk_combobox_active_item_set (ETK_COMBOBOX(UI_ImageComboBox), etk_combobox_nth_item_get(ETK_COMBOBOX(UI_ImageComboBox),0));}

	
	etk_range_value_set (ETK_RANGE(UI_BorderTopSpinner), selected_desc->image_border_top);
	etk_range_value_set (ETK_RANGE(UI_BorderLeftSpinner), selected_desc->image_border_left);
	etk_range_value_set (ETK_RANGE(UI_BorderRightSpinner), selected_desc->image_border_right);
	etk_range_value_set (ETK_RANGE(UI_BorderBottomSpinner), selected_desc->image_border_bottom);
	etk_range_value_set (ETK_RANGE(UI_ImageAlphaSlider), selected_desc->color_a);
	
	//ReEnable Signal Propagation
	etk_signal_connect("value_changed", ETK_OBJECT(UI_BorderLeftSpinner), on_BorderSpinner_value_changed, (void *)BORDER_LEFT);	
	etk_signal_connect("value_changed", ETK_OBJECT(UI_BorderRightSpinner), on_BorderSpinner_value_changed, (void *)BORDER_RIGHT);	
	etk_signal_connect("value_changed", ETK_OBJECT(UI_BorderTopSpinner), on_BorderSpinner_value_changed, (void *)BORDER_TOP);	
	etk_signal_connect("value_changed", ETK_OBJECT(UI_BorderBottomSpinner), on_BorderSpinner_value_changed, (void *)BORDER_BOTTOM);	
	etk_signal_connect("value_changed", ETK_OBJECT(UI_ImageAlphaSlider), on_ImageAlphaSlider_value_changed, NULL);

	
	g_string_free(str,TRUE);
}
void UpdateTextFrame(void){
	int eff_num = 0;
	int i;
	Etk_Combobox_Item *item = NULL;
	char* font;
	
	if (selected_desc){
		printf("Update Text Frame: %s\n",selected_desc->state->str);
		
		//Stop signal propagation 
		etk_signal_disconnect("text_changed",ETK_OBJECT(UI_TextEntry),on_TextEntry_text_changed);
		etk_signal_disconnect("active_item_changed", ETK_OBJECT(UI_FontComboBox), ETK_CALLBACK(on_FontComboBox_changed));
		etk_signal_disconnect("value_changed", ETK_OBJECT(UI_TextAlphaSlider), ETK_CALLBACK(on_TextAlphaSlider_value_changed));

		//Set Text Text in selected_desc
		etk_entry_text_set(ETK_ENTRY(UI_TextEntry),selected_desc->text_text->str);
		
		//Set the font combobox
		if (selected_desc->text_font->len > 0){
			i=0;
			while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_FontComboBox),i))){	//Loop for all the item in the Combobox
				if ((font = etk_combobox_item_data_get(item))){							//Get the data for the item (should be an char* with the name of the font file)
					if (strcmp(font ,selected_desc->text_font->str) == 0) 
						etk_combobox_active_item_set (ETK_COMBOBOX(UI_FontComboBox),item);	//If we found the item set active
				}
			i++;
			}
		}else{etk_combobox_active_item_set (ETK_COMBOBOX(UI_FontComboBox), etk_combobox_nth_item_get(ETK_COMBOBOX(UI_FontComboBox),0));}
	
	
		//Set the font size spinner
		etk_range_value_set (ETK_RANGE(UI_FontSizeSpinner), selected_desc->text_size);
		
		//Set Effect ComboBox
		switch (selected_part->effect){
			case FX_PLAIN: eff_num = 0; break;
			case FX_OUTLINE: eff_num = 1; break;
			case FX_SOFT_OUTLINE: eff_num = 2; break;
			case FX_SHADOW: eff_num = 3; break;
			case FX_SOFT_SHADOW: eff_num = 4; break;
			case FX_OUTLINE_SHADOW: eff_num = 5; break;
			case FX_OUTLINE_SOFT_SHADOW: eff_num = 6; break;
		}
		etk_combobox_active_item_set (ETK_COMBOBOX(UI_EffectComboBox), etk_combobox_nth_item_get (ETK_COMBOBOX(UI_EffectComboBox), eff_num));
		
		//Set Text alpha 
		etk_range_value_set (ETK_RANGE(UI_TextAlphaSlider), selected_desc->color_a);
		
		//Set Text color Rects
		evas_object_color_set(TextColorObject,selected_desc->color_r,selected_desc->color_g,selected_desc->color_b,selected_desc->color_a);
		evas_object_color_set(ShadowColorObject,selected_desc->color2_r,selected_desc->color2_g,selected_desc->color2_b,selected_desc->color2_a);
		evas_object_color_set(OutlineColorObject,selected_desc->color3_r,selected_desc->color3_g,selected_desc->color3_b,selected_desc->color3_a);
	
		
		//Renable  signal propagation 
		etk_signal_connect("text_changed", ETK_OBJECT(UI_TextEntry), ETK_CALLBACK(on_TextEntry_text_changed), NULL);
		etk_signal_connect("active_item_changed", ETK_OBJECT(UI_FontComboBox), ETK_CALLBACK(on_FontComboBox_changed), NULL);
		etk_signal_connect("value_changed", ETK_OBJECT(UI_TextAlphaSlider), ETK_CALLBACK(on_TextAlphaSlider_value_changed), NULL);

	
	}
}
void UpdatePositionFrame(void){
	EDC_Description *desc = selected_desc;
	
	printf("Update Position: %s\n",desc->state->str);
	
	//Stop signal propagation
	etk_signal_disconnect("value_changed", ETK_OBJECT(UI_Rel1XSpinner), ETK_CALLBACK(on_RelSpinner_value_changed));
	etk_signal_disconnect("value_changed", ETK_OBJECT(UI_Rel1YSpinner), ETK_CALLBACK(on_RelSpinner_value_changed));
	etk_signal_disconnect("value_changed", ETK_OBJECT(UI_Rel2XSpinner), ETK_CALLBACK(on_RelSpinner_value_changed));
	etk_signal_disconnect("value_changed", ETK_OBJECT(UI_Rel2YSpinner), ETK_CALLBACK(on_RelSpinner_value_changed));
	
	//Set relative position spinners
 	etk_range_value_set (ETK_RANGE(UI_Rel1XSpinner), desc->rel1_relative_x);
	etk_range_value_set (ETK_RANGE(UI_Rel1XOffsetSpinner), desc->rel1_offset_x);
	etk_range_value_set (ETK_RANGE(UI_Rel1YSpinner), desc->rel1_relative_y);
	etk_range_value_set (ETK_RANGE(UI_Rel1YOffsetSpinner), desc->rel1_offset_y);
	etk_range_value_set (ETK_RANGE(UI_Rel2XSpinner), desc->rel2_relative_x);
	etk_range_value_set (ETK_RANGE(UI_Rel2XOffsetSpinner), desc->rel2_offset_x);
	etk_range_value_set (ETK_RANGE(UI_Rel2YSpinner), desc->rel2_relative_y);
	etk_range_value_set (ETK_RANGE(UI_Rel2YOffsetSpinner), desc->rel2_offset_y); 
	
	//Reenable signal propagation
	etk_signal_connect("value_changed", ETK_OBJECT(UI_Rel1XSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void *)REL1X_SPINNER);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_Rel1YSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void *)REL1Y_SPINNER);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_Rel2XSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void *)REL2X_SPINNER);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_Rel2YSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void *)REL2Y_SPINNER);

}

void UpdateComboPositionFrame(void){
	EDC_Description *desc = selected_desc;
	GString *str=g_string_new("");
	int i=0;
	Etk_Combobox_Item *item = NULL;
	EDC_Part *part=NULL;
	
		
	/* Set RelTo combos */
	/* //If rel1_to is know set rel1_to_x and rel1_to_y
	if  (desc->rel1_to->len > 0){
		desc->rel1_to_x = desc->rel1_to;
		desc->rel1_to_y = desc->rel1_to;
	} */
	printf("SETTING COMBOS %s\n", desc->rel1_to_x->str);
	//If rel1_to_x is know set the combobox
	if (desc->rel1_to_x->len > 0){
		i=0;
		while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel1ToXComboBox),i))){					//Loop for all the item in the Combobox
			if ((part = etk_combobox_item_data_get(item))){									//Get the data for the item (should be an EDC_part*)
				if ((int)part != REL_COMBO_INTERFACE)
					if (strcmp(part->name->str,desc->rel1_to_x->str) == 0) 
						etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel1ToXComboBox),item);				//If we found the item set active
			}
			i++;
		}
	}else{etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel1ToXComboBox), etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel1ToXComboBox),0));}
	
	//If rel1_to_y is know set the combobox
	if (desc->rel1_to_y->len > 0){
		i=0;
		while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel1ToYComboBox),i))){					//Loop for all the item in the Combobox
			if ((part = etk_combobox_item_data_get(item))){									//Get the data for the item (should be an EDC_part*)
				if ((int)part != REL_COMBO_INTERFACE)
					if (strcmp(part->name->str,desc->rel1_to_y->str) == 0) 
						etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel1ToYComboBox),item);				//If we found the item set active
			}
			i++;
		}
	}else{etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel1ToYComboBox), etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel1ToYComboBox),0));}
	
/* 	//If rel2_to is know set rel2_to_x and rel2_to_y
	if  (desc->rel2_to->len > 0){
		desc->rel2_to_x = desc->rel2_to;
		desc->rel2_to_y = desc->rel2_to;
	} */
	
	//If rel2_to_x is know set the combobox
	if (desc->rel2_to_x->len > 0){
		i=0;
		while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToXComboBox),i))){					//Loop for all the item in the Combobox
			if ((part = etk_combobox_item_data_get(item))){									//Get the data for the item (should be an EDC_part*)
				if ((int)part != REL_COMBO_INTERFACE)
					if (strcmp(part->name->str,desc->rel2_to_x->str) == 0) 
						etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel2ToXComboBox),item);				//If we found the item set active
			}
			i++;
		}
	}else{etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel2ToXComboBox), etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToXComboBox),0));}
	
	//If rel2_to_y is know set the combobox
	if (desc->rel2_to_y->len > 0){
		
		i=0;
		while ((item = etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToYComboBox),i))){					//Loop for all the item in the Combobox
			if ((part = etk_combobox_item_data_get(item))){									//Get the data for the item (should be an EDC_part*)
				if ((int)part != REL_COMBO_INTERFACE)
					if (strcmp(part->name->str,desc->rel2_to_y->str) == 0) 
						etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel2ToYComboBox),item);				//If we found the item set active
			}
			i++;
		}
	}else{etk_combobox_active_item_set (ETK_COMBOBOX(UI_Rel2ToYComboBox), etk_combobox_nth_item_get(ETK_COMBOBOX(UI_Rel2ToYComboBox),0));}
	
	g_string_free(str,TRUE);
}

void UpdateWindowTitle(void){
	GString *str;
	if (EDCFile){
		str = g_string_new("");
		g_string_printf(str,"Edje Editor - %s",EDCFile->str);
		etk_window_title_set(ETK_WINDOW(UI_MainWin), str->str);
		g_string_free(str,TRUE); 
	}
}
void RecreateRelComboBoxes(void){
	GList *p_list;
	Etk_Combobox_Item *ComboItem;
	EDC_Part*	part = NULL;
	GString	*str;
	
	etk_combobox_clear(ETK_COMBOBOX(UI_Rel1ToXComboBox));
	etk_combobox_clear(ETK_COMBOBOX(UI_Rel1ToYComboBox));
	etk_combobox_clear(ETK_COMBOBOX(UI_Rel2ToXComboBox));
	etk_combobox_clear(ETK_COMBOBOX(UI_Rel2ToYComboBox));
	
	if (selected_group){
		p_list = selected_group->parts;
		ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToXComboBox), etk_image_new_from_edje (EdjeFile->str,"NONE.PNG"),"Interface");
		etk_combobox_item_data_set (ComboItem, (void *)REL_COMBO_INTERFACE);
	 	ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToYComboBox),etk_image_new_from_edje (EdjeFile->str,"NONE.PNG"), "Interface");
		etk_combobox_item_data_set (ComboItem, (void *)REL_COMBO_INTERFACE);
		ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToXComboBox), etk_image_new_from_edje (EdjeFile->str,"NONE.PNG"),"Interface");
		etk_combobox_item_data_set (ComboItem, (void *)REL_COMBO_INTERFACE);
		ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToYComboBox),etk_image_new_from_edje (EdjeFile->str,"NONE.PNG"), "Interface");
		etk_combobox_item_data_set (ComboItem, (void *)REL_COMBO_INTERFACE); 
		 while(p_list){
			part = p_list->data;
			printf("%s\n",part->name->str);
			if (part->type == PART_TYPE_RECT) str = g_string_new("RECT.PNG");
			if (part->type == PART_TYPE_TEXT) str = g_string_new("TEXT.PNG");
			if (part->type == PART_TYPE_IMAGE) str = g_string_new("IMAGE.PNG");
			if (part->type == NONE) str = g_string_new("NONE.PNG");
			
			ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToXComboBox),etk_image_new_from_edje (EdjeFile->str,str->str), part->name->str);
			etk_combobox_item_data_set (ComboItem, part);
			ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel1ToYComboBox),etk_image_new_from_edje (EdjeFile->str,str->str), part->name->str);
			etk_combobox_item_data_set (ComboItem, part);
			ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToXComboBox),etk_image_new_from_edje (EdjeFile->str,str->str), part->name->str);
			etk_combobox_item_data_set (ComboItem, part);
			ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_Rel2ToYComboBox), etk_image_new_from_edje (EdjeFile->str,str->str),part->name->str);
			etk_combobox_item_data_set (ComboItem, part);
			p_list = g_list_next(p_list); 
			g_string_free(str,TRUE);
		} 
	}
}
void AddPartToTree(EDC_Part* part){
	Etk_Tree_Col *col1,*col2;
	Etk_Tree_Row *row=NULL;
	EDC_Description *desc;
	EDC_Group *parent_group = part->group;
	GList	*Descs;
	GString *str = g_string_new("");
	col1 = etk_tree_nth_col_get(ETK_TREE(parent_group->tree_widget), 0);
	col2 = etk_tree_nth_col_get(ETK_TREE(parent_group->tree_widget), 1);
	
	switch (part->type){
			case PART_TYPE_IMAGE: row = etk_tree_append(ETK_TREE(parent_group->tree_widget), col1, EdjeFile->str,"IMAGE.PNG", part->name->str,  NULL);break;
			case PART_TYPE_TEXT: row = etk_tree_append(ETK_TREE(parent_group->tree_widget), col1, EdjeFile->str,"TEXT.PNG", part->name->str,  NULL);break;
			case PART_TYPE_RECT: row = etk_tree_append(ETK_TREE(parent_group->tree_widget), col1, EdjeFile->str,"RECT.PNG", part->name->str,  NULL);break;
			default: row = etk_tree_append(ETK_TREE(parent_group->tree_widget), col1, EdjeFile->str,"NONE.PNG", part->name->str,  NULL);break;
	}
	
	if (row){
		part->tree_row = row;
		etk_tree_row_data_set (row, part);
		Descs = part->descriptions;
		while (Descs){
			desc = Descs->data;
			AddDescToTree(desc);
			Descs = g_list_next(Descs);
		}
	}
	
	g_string_free(str,TRUE);
}
void AddDescToTree(EDC_Description* desc){
	Etk_Tree_Col *col1,*col2;
	Etk_Tree_Row *row;
	EDC_Part* part = desc->part;
	EDC_Group* group = part->group;
	GString	*str=g_string_new("");
	
	col1 = etk_tree_nth_col_get(ETK_TREE(group->tree_widget), 0);
	col2 = etk_tree_nth_col_get(ETK_TREE(group->tree_widget), 1);
	
	g_string_printf(str,"%s %.1f",desc->state->str,desc->state_index);
	row = etk_tree_append_to_row(part->tree_row, col1,EdjeFile->str,"DESC.PNG",str->str, col2,TRUE,NULL); 
	
	
	etk_tree_row_data_set (row, desc);
	desc->tree_row = row;
	
	g_string_free(str,TRUE);
}
Etk_Widget* CreateNewTree(EDC_Group* group){
	Etk_Tree_Col *col;
	printf("Create a tree for group: %s\n", group->name->str);
	
	//PartTree
	group->tree_widget = etk_tree_new();
	etk_widget_size_request_set(group->tree_widget, 255, 300);
	
	etk_scrolled_view_policy_set (etk_tree_scrolled_view_get (ETK_TREE(group->tree_widget)),ETK_POLICY_HIDE,ETK_POLICY_AUTO);
	etk_tree_mode_set(ETK_TREE(group->tree_widget), ETK_TREE_MODE_TREE);
	etk_tree_multiple_select_set(ETK_TREE(group->tree_widget), ETK_FALSE);
	//etk_tree_headers_visible_set (ETK_TREE(PartList), FALSE);
	col = etk_tree_col_new(ETK_TREE(group->tree_widget), "Parts in group", etk_tree_model_icon_text_new(ETK_TREE(group->tree_widget), ETK_TREE_FROM_EDJE),100);
	etk_tree_col_resizable_set (col, FALSE);
	etk_tree_col_expand_set (col,TRUE);
	col = etk_tree_col_new(ETK_TREE(group->tree_widget), "visible", etk_tree_model_checkbox_new(ETK_TREE(group->tree_widget)), 10);
	etk_tree_col_resizable_set (col, FALSE);
	etk_tree_col_expand_set (col,FALSE);
	etk_tree_build(ETK_TREE(group->tree_widget));
	etk_box_append(ETK_BOX(UI_tree_vbox), group->tree_widget, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0); 
		
	etk_signal_connect("row_selected", ETK_OBJECT(group->tree_widget), ETK_CALLBACK(on_PartTree_row_selected), NULL);
	
	etk_widget_show(group->tree_widget);
	
	return group->tree_widget;
}
void ShowFilechooser(int FileChooserType){
	
	FileChooserOperation = FileChooserType;
	if (FileChooserType == FILECHOOSER_OPEN) etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose an EDC file to open");
	if (FileChooserType == FILECHOOSER_NEW) etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose a name for the new EDC");
	if (FileChooserType == FILECHOOSER_IMAGE) etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose an image to import");
	if (FileChooserType == FILECHOOSER_FONT) etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose an font to import");
	if (FileChooserType == FILECHOOSER_SAVE_AS) etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose a new name");
	
	etk_widget_show_all(UI_FileChooserDialog);
}

/* functions to create interface*/
Etk_Widget* create_play_dialog(void){
	Etk_Widget *dialog;
	Etk_Widget *label;
	Etk_Widget *vbox;

	//Dialog
	dialog = etk_dialog_new();
	//etk_container_border_width_set (dialog,5);	//TODO Can't exec this line(don't show the dialog). Etk bug?
	etk_dialog_has_separator_set (ETK_DIALOG(dialog), ETK_FALSE);
	etk_signal_connect("delete_event", ETK_OBJECT(dialog), ETK_CALLBACK(etk_window_hide_on_delete), NULL);	
	etk_signal_connect("response", ETK_OBJECT(dialog), ETK_CALLBACK(on_PlayDialog_response), NULL);	
	etk_widget_size_request_set(dialog, 400, 300);
	
	
	vbox = etk_vbox_new(ETK_FALSE, 10);
	etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), vbox, ETK_TRUE, ETK_TRUE, 0, ETK_FALSE);
	
	
		label = etk_label_new("<b>Executing_edje_cc</>");
		etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
		etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);	
		
		//Text_view
		UI_PlayTextView = etk_text_view_new ();
		etk_box_append(ETK_BOX(vbox), UI_PlayTextView, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);	
		//etk_textblock_object_cursor_visible_set (ETK_TEXT_VIEW(UI_PlayTextView)->textblock, ETK_FALSE); //TODO questa riga non funziona
 
 
	//etk_dialog_button_add_from_stock(ETK_DIALOG(dialog),ETK_STOCK_DOCUMENT_OPEN ,ETK_RESPONSE_OK );
	etk_dialog_button_add_from_stock(ETK_DIALOG(dialog),ETK_STOCK_DIALOG_CLOSE ,ETK_RESPONSE_CLOSE);
	etk_dialog_button_add (ETK_DIALOG(dialog), "Test!", ETK_RESPONSE_OK);
 
 
 
	etk_widget_show_all(dialog);
	etk_widget_hide(dialog);
	
	return dialog;
}
Etk_Widget* create_filechooser_dialog(void){
	Etk_Widget *dialog = NULL;
	Etk_Widget *label = NULL;
	Etk_Widget *file_tree = NULL;
	Etk_Widget *places_tree = NULL;
	Etk_Widget *fav_tree = NULL;
	

	//Dialog
	dialog = etk_dialog_new();
	etk_signal_connect("delete_event", ETK_OBJECT(dialog), ETK_CALLBACK(etk_window_hide_on_delete), NULL);	
	etk_signal_connect("response", ETK_OBJECT(dialog), ETK_CALLBACK(on_FileChooser_response), NULL);	
	
	//Filechooser
	UI_FileChooser = etk_filechooser_widget_new();
	etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), UI_FileChooser, ETK_TRUE, ETK_TRUE, 0, ETK_FALSE);

	file_tree = ETK_FILECHOOSER_WIDGET(UI_FileChooser)->files_tree;
	etk_signal_connect("row_clicked", ETK_OBJECT(file_tree), ETK_CALLBACK(on_FileChooser_row_selected), NULL);
	etk_signal_connect("row_selected", ETK_OBJECT(file_tree), ETK_CALLBACK(on_FileChooser_row_selected), NULL);
	places_tree = ETK_FILECHOOSER_WIDGET(UI_FileChooser)->places_tree;
	etk_signal_connect("row_clicked", ETK_OBJECT(places_tree), ETK_CALLBACK(on_FileChooser_row_selected), NULL);
	etk_signal_connect("row_selected", ETK_OBJECT(places_tree), ETK_CALLBACK(on_FileChooser_row_selected), NULL);
	fav_tree = ETK_FILECHOOSER_WIDGET(UI_FileChooser)->fav_tree;
	etk_signal_connect("row_clicked", ETK_OBJECT(fav_tree), ETK_CALLBACK(on_FileChooser_row_selected), NULL);
	etk_signal_connect("row_selected", ETK_OBJECT(fav_tree), ETK_CALLBACK(on_FileChooser_row_selected), NULL);
 
	
	//label
	label = etk_label_new("<b>File</b>");
	etk_object_properties_set (ETK_OBJECT(label), "xalign",1.0,NULL);
	etk_dialog_pack_widget_in_action_area(ETK_DIALOG(dialog),label,FALSE,TRUE,10,FALSE);
	
	//FileNameEntry
	UI_FilechooserFileNameEntry = etk_entry_new();
	
	etk_dialog_pack_widget_in_action_area(ETK_DIALOG(dialog),UI_FilechooserFileNameEntry,TRUE,TRUE,0,FALSE);
	/* 	Etk_Bool  	expand,
		Etk_Bool  	fill,
		int  	padding,
		Etk_Bool  	pack_at_end	 
	 */ 	
	etk_dialog_button_add_from_stock(ETK_DIALOG(dialog),ETK_STOCK_DOCUMENT_OPEN ,ETK_RESPONSE_OK );
	etk_dialog_button_add_from_stock(ETK_DIALOG(dialog),ETK_STOCK_DIALOG_CANCEL ,ETK_RESPONSE_CANCEL );
	
	return dialog;
}
Etk_Widget* create_a_color_button(char* label_text, int color_button_enum){
	Etk_Widget *vbox;
	Etk_Widget *ColorCanvas;
	Etk_Widget *label;
	Etk_Widget *shadow;
	
	//vbox
	vbox = etk_vbox_new(ETK_FALSE, 3);
		
		//shadow
		shadow = etk_shadow_new();
		etk_shadow_border_set(ETK_SHADOW(shadow), 0);
		etk_shadow_shadow_set(ETK_SHADOW(shadow),ETK_SHADOW_OUTSIDE, ETK_SHADOW_ALL, 10, 2, 2, 200);
		//etk_shadow_shadow_set(Etk_Shadow *shadow, Etk_Shadow_Type type, Etk_Shadow_Edges edges, int radius, int offset_x, int offset_y, int opacity);
		etk_box_append(ETK_BOX(vbox), shadow, ETK_BOX_START, ETK_BOX_NONE, 0);		

			//ColorCanvas
			ColorCanvas = etk_canvas_new ();
			etk_box_append(ETK_BOX(vbox), ColorCanvas, ETK_BOX_START, ETK_BOX_NONE, 0);
			etk_container_add(ETK_CONTAINER(shadow), ColorCanvas);
			etk_signal_connect("realize", ETK_OBJECT(ColorCanvas), ETK_CALLBACK(on_ColorCanvas_realize), (void*)color_button_enum);
			etk_widget_size_request_set(ColorCanvas, 30, 30);
	
		//Label
		if (label_text){
			label = etk_label_new(label_text);
			etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
			etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);	
		}
	return vbox;
}	
Etk_Widget* create_toolbar(void){
	Etk_Widget *ToolBar;
	Etk_Widget *button;
	Etk_Widget *sep;
	Etk_Widget *menu_item;
	Etk_Widget *image;
	
	//ToolBar
	ToolBar = etk_toolbar_new ();
	etk_toolbar_orientation_set (ETK_TOOLBAR(ToolBar), ETK_TOOLBAR_VERT);
	
		//NewButton
		button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_NEW);
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), button);
		etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(on_ToolBarButton_click), (void*)TOOLBAR_NEW);
		
		//OpenButton
		button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_OPEN);
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), button);
		etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(on_ToolBarButton_click), (void*)TOOLBAR_OPEN);
		
		//SaveButton
		button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE);
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), button);
		etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(on_ToolBarButton_click), (void*)TOOLBAR_SAVE);
		
		//SaveAsButton
		button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE_AS);
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), button);
		etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(on_ToolBarButton_click), (void*)TOOLBAR_SAVE_AS);
		
		sep = etk_hseparator_new();
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), sep);
		
		//AddButton
		button = etk_tool_button_new_from_stock(ETK_STOCK_LIST_ADD);
		etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(on_ToolBarButton_click), (void*)TOOLBAR_ADD);
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), button);

			//AddMenu
			UI_AddMenu = etk_menu_new();
			
				//New Rectangle
				menu_item = etk_menu_item_image_new_with_label("Rectangle");
				image = etk_image_new_from_edje(EdjeFile->str,"RECT.PNG");
				etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
				etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(on_AddMenu_item_activated), (void*)NEW_RECT);
				etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));
				
				//New Image
				menu_item = etk_menu_item_image_new_with_label("Image");
				image = etk_image_new_from_edje(EdjeFile->str,"IMAGE.PNG");
				etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
				etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(on_AddMenu_item_activated),(void*) NEW_IMAGE);
				etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));
				//New Text
				menu_item = etk_menu_item_image_new_with_label("Text");
				image = etk_image_new_from_edje(EdjeFile->str,"TEXT.PNG");
				etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
				etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(on_AddMenu_item_activated), (void*)NEW_TEXT);
				
				etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));
				//Separator
				menu_item = etk_menu_item_separator_new();
				etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));
				
				//New Description
				menu_item = etk_menu_item_image_new_with_label("Add a new description");
				etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(on_AddMenu_item_activated), (void*)NEW_DESC);
				etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));
				
				//New Group
				menu_item = etk_menu_item_image_new_with_label("Add a new group");
				etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(on_AddMenu_item_activated), (void*)NEW_GROUP);
				
				etk_menu_shell_append(ETK_MENU_SHELL(UI_AddMenu), ETK_MENU_ITEM(menu_item));
		
		//RemoveButton
		button = etk_tool_button_new_from_stock(ETK_STOCK_LIST_REMOVE);
		etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(on_ToolBarButton_click), (void*)TOOLBAR_REMOVE);
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), button);
			
				//RemoveMenu
				UI_RemoveMenu = etk_menu_new();
			
				//description
				menu_item = etk_menu_item_image_new_with_label("Selected Description");
				etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(on_RemoveMenu_item_activated), (void*)REMOVE_DESCRIPTION);
				etk_menu_shell_append(ETK_MENU_SHELL(UI_RemoveMenu), ETK_MENU_ITEM(menu_item));
				
				//part
				menu_item = etk_menu_item_image_new_with_label("Selected Part");
				etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(on_RemoveMenu_item_activated), (void*)REMOVE_PART);
				etk_menu_shell_append(ETK_MENU_SHELL(UI_RemoveMenu), ETK_MENU_ITEM(menu_item));
				
				//group
				menu_item = etk_menu_item_image_new_with_label("Selected Group");
				etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(on_RemoveMenu_item_activated), (void*)REMOVE_GROUP);
				etk_menu_shell_append(ETK_MENU_SHELL(UI_RemoveMenu), ETK_MENU_ITEM(menu_item));
		
		sep = etk_hseparator_new();
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), sep);
		
		//MoveUp Button
		button = etk_tool_button_new_from_stock( ETK_STOCK_GO_UP);
		etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(on_ToolBarButton_click), (void*)TOOLBAR_MOVE_UP);
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), button);
	
		//MoveDown Button
		button = etk_tool_button_new_from_stock( ETK_STOCK_GO_DOWN);
		etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(on_ToolBarButton_click), (void*)TOOLBAR_MOVE_DOWN);
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), button);
	
		sep = etk_hseparator_new();
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), sep);
	
		//Compile Button
		button = etk_tool_button_new_from_stock( ETK_STOCK_MEDIA_PLAYBACK_START);
		etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(on_ToolBarButton_click), (void*)TOOLBAR_PLAY);
		etk_object_properties_set(ETK_OBJECT(button),"label","Compile",NULL);
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), button);
		
		
		//DebugButton
		button = etk_tool_button_new_from_stock(ETK_STOCK_DOCUMENT_PROPERTIES);
		etk_toolbar_append(ETK_TOOLBAR(ToolBar), button);
		etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(on_ToolBarButton_click), (void*)TOOLBAR_DEBUG);
		etk_object_properties_set(ETK_OBJECT(button),"label","Debug",NULL);
		
	return ToolBar;
}


Etk_Widget* create_group_frame(void){
	Etk_Widget *frame;
	Etk_Widget *label;
	Etk_Widget *hbox;
	
	//frame
	frame = etk_frame_new("Groups");
	
		//vbox
		UI_tree_vbox = etk_vbox_new(ETK_FALSE, 0);
		etk_container_add(ETK_CONTAINER(frame), UI_tree_vbox);
	
			//GroupCombobox
			UI_GroupComboBox = etk_combobox_new();
			etk_combobox_column_add(ETK_COMBOBOX(UI_GroupComboBox), ETK_COMBOBOX_LABEL, 75, ETK_TRUE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
			etk_combobox_build(ETK_COMBOBOX(UI_GroupComboBox));
			etk_box_append(ETK_BOX(UI_tree_vbox), UI_GroupComboBox, ETK_BOX_START, ETK_BOX_NONE, 0);
			
			//hbox
			hbox = etk_hbox_new(ETK_FALSE, 0);
			etk_box_append(ETK_BOX(UI_tree_vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
			
				label = etk_label_new("min");
				etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);	
		
				//UI_GroupMinXSpinner
				UI_GroupMinXSpinner =  etk_spinner_new (0, 2000, 0, 1, 10);
				etk_widget_size_request_set(UI_GroupMinXSpinner, 45, 20);
				etk_box_append(ETK_BOX(hbox),UI_GroupMinXSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);
				
				label = etk_label_new("x");
				etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
				etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);	
			
				//UI_GroupMinYSpinner
				UI_GroupMinYSpinner =  etk_spinner_new (0, 2000, 0, 1, 10);
				etk_widget_size_request_set(UI_GroupMinYSpinner, 45, 20);
				etk_box_append(ETK_BOX(hbox),UI_GroupMinYSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);
			
			
				label = etk_label_new("max");
				etk_object_properties_set (ETK_OBJECT(label), "xalign",1.0,NULL);
				etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);	
			
				//UI_GroupMaxXSpinner
				UI_GroupMaxXSpinner =  etk_spinner_new (0, 2000, 0, 1, 10);
				etk_widget_size_request_set(UI_GroupMaxXSpinner, 45, 20);
				etk_box_append(ETK_BOX(hbox),UI_GroupMaxXSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);
			
				label = etk_label_new("x");
				etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
				etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);	
			
				//UI_GroupMaxYSpinner
				UI_GroupMaxYSpinner =  etk_spinner_new (0, 2000, 0, 1, 10);
				etk_widget_size_request_set(UI_GroupMaxYSpinner, 45, 20);
				etk_box_append(ETK_BOX(hbox),UI_GroupMaxYSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);
			
			
	etk_signal_connect("value_changed", ETK_OBJECT(UI_GroupMinXSpinner), ETK_CALLBACK(on_GroupSpinner_value_changed), (void *)MINX_SPINNER);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_GroupMinYSpinner), ETK_CALLBACK(on_GroupSpinner_value_changed), (void *)MINY_SPINNER);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_GroupMaxXSpinner), ETK_CALLBACK(on_GroupSpinner_value_changed), (void *)MAXX_SPINNER);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_GroupMaxYSpinner), ETK_CALLBACK(on_GroupSpinner_value_changed), (void *)MAXY_SPINNER);

	
	return frame;
}
Etk_Widget* create_description_frame(void){
	Etk_Widget *vbox;
	Etk_Widget *hbox;
	
	//DescriptionFrame
	UI_DescriptionFrame = etk_frame_new("Description property");
		
		//vbox
		vbox = etk_vbox_new(ETK_FALSE, 0);
		etk_container_add(ETK_CONTAINER(UI_DescriptionFrame), vbox);
		
			//hbox
			hbox = etk_hbox_new(ETK_FALSE, 0);
			etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
			
				//StateEntry
				UI_StateEntry = etk_entry_new();
				etk_box_append(ETK_BOX(hbox), UI_StateEntry, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
		
				//UI_StateIndexSpinner
				UI_StateIndexSpinner = etk_spinner_new (0.0, 1.0, 0.0, 0.1, 1.0);
				etk_spinner_digits_set (ETK_SPINNER(UI_StateIndexSpinner), 1);
				etk_widget_size_request_set(UI_StateIndexSpinner,45, 20);
				etk_box_append(ETK_BOX(hbox),UI_StateIndexSpinner, ETK_BOX_START, ETK_BOX_NONE, 0);
			
			
	etk_signal_connect("text_changed", ETK_OBJECT(UI_StateEntry), ETK_CALLBACK(on_StateEntry_text_changed), NULL);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_StateIndexSpinner), ETK_CALLBACK(on_StateIndexSpinner_value_changed), NULL);

	return UI_DescriptionFrame;
}
Etk_Widget* create_rectangle_frame(void){
	Etk_Widget *vbox;
	Etk_Widget *hbox;
	Etk_Widget *label;

	//RectFrame
	UI_RectFrame = etk_frame_new("Rectangle");
	
		//vbox
		vbox = etk_vbox_new(ETK_FALSE, 0);
		etk_container_add(ETK_CONTAINER(UI_RectFrame), vbox);
		
			label = etk_label_new("<b>Color</b>");
			etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
			etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);	
		
			//hbox
			hbox = etk_hbox_new(ETK_FALSE, 10);
			etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
			
			
				//color button
				etk_box_append(ETK_BOX(hbox), create_a_color_button(NULL,COLOR_OBJECT_RECT), ETK_BOX_START, ETK_BOX_NONE, 0);
				
				//ColorAlphaSlider
				label = etk_label_new("Alpha");
				etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
				UI_ColorAlphaSlider = etk_hslider_new (0, 255, 15, 1,20);
				etk_box_append(ETK_BOX(hbox),UI_ColorAlphaSlider, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
				UI_AlphaLabel = etk_label_new("000");
				etk_box_append(ETK_BOX(hbox), UI_AlphaLabel, ETK_BOX_START, ETK_BOX_NONE, 0);
	
	etk_signal_connect("value_changed", ETK_OBJECT(UI_ColorAlphaSlider), ETK_CALLBACK(on_ColorAlphaSlider_value_changed), UI_AlphaLabel);

	return UI_RectFrame;
}
Etk_Widget* create_image_frame(void){
	Etk_Widget *label;
	Etk_Widget *table;

	//ImageFrame
	UI_ImageFrame = etk_frame_new("Image");
		
		//table
		table = etk_table_new (5, 4, FALSE);
		etk_container_add(ETK_CONTAINER(UI_ImageFrame), table);
			
				label = etk_label_new("Image");
				//etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
				etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 0, 0);
		
				//imageComboBox
				UI_ImageComboBox = etk_combobox_new();
				etk_combobox_column_add(ETK_COMBOBOX(UI_ImageComboBox), ETK_COMBOBOX_IMAGE, 24, ETK_FALSE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_column_add(ETK_COMBOBOX(UI_ImageComboBox), ETK_COMBOBOX_LABEL, 75, ETK_TRUE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_build(ETK_COMBOBOX(UI_ImageComboBox));
				etk_combobox_item_append(ETK_COMBOBOX(UI_ImageComboBox), etk_image_new_from_stock (ETK_STOCK_DOCUMENT_OPEN,ETK_STOCK_MEDIUM), "Insert a new image");
				etk_table_attach_default (ETK_TABLE(table),UI_ImageComboBox, 1, 4, 0, 0);
			
				label = etk_label_new("Alpha");
				//etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
				etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 1, 1);
			
				//ImageAlphaSlider
				UI_ImageAlphaSlider = etk_hslider_new (0, 255, 15, 1,20);
				etk_table_attach_default (ETK_TABLE(table),UI_ImageAlphaSlider, 1, 4, 1, 1);
				
				label = etk_label_new("Left");
				etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
				etk_table_attach_default (ETK_TABLE(table),label, 1, 1, 2, 2);

				label = etk_label_new("Right");
				etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
				etk_table_attach_default (ETK_TABLE(table),label, 2, 2, 2, 2);
			
				label = etk_label_new("Top");
				etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
				etk_table_attach_default (ETK_TABLE(table),label, 3, 3, 2, 2);
				
				label = etk_label_new("Bottom");
				etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
				etk_table_attach_default (ETK_TABLE(table),label, 4, 4, 2, 2);
	
				label = etk_label_new("Border");
				//etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
				etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 3, 3);
				
				//UI_BorderLeftSpinner
				UI_BorderLeftSpinner = etk_spinner_new (0, 500, 0, 1, 10);
				etk_widget_size_request_set(UI_BorderLeftSpinner,45, 20);
				etk_table_attach_default (ETK_TABLE(table),UI_BorderLeftSpinner, 1, 1, 3, 3);
			
				//UI_BorderRightSpinner
				UI_BorderRightSpinner = etk_spinner_new (0, 500, 0, 1, 10);
				etk_widget_size_request_set(UI_BorderRightSpinner,45, 20);
				etk_table_attach_default (ETK_TABLE(table),UI_BorderRightSpinner, 2, 2, 3, 3);
	
				//UI_BorderTopSpinner
				UI_BorderTopSpinner = etk_spinner_new (0, 500, 0, 1, 10);
				etk_widget_size_request_set(UI_BorderTopSpinner,45, 20);
				etk_table_attach_default (ETK_TABLE(table),UI_BorderTopSpinner, 3, 3, 3, 3);
			
				//UI_BorderBottomSpinner
				UI_BorderBottomSpinner = etk_spinner_new (0, 500, 0, 1, 10);
				etk_widget_size_request_set(UI_BorderBottomSpinner,45, 20);
				etk_table_attach_default (ETK_TABLE(table),UI_BorderBottomSpinner, 4, 4, 3, 3);
			
	etk_signal_connect("value_changed", ETK_OBJECT(UI_ImageAlphaSlider), ETK_CALLBACK(on_ImageAlphaSlider_value_changed), NULL);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_BorderLeftSpinner), ETK_CALLBACK(on_BorderSpinner_value_changed), (void *)BORDER_LEFT);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_BorderRightSpinner), ETK_CALLBACK(on_BorderSpinner_value_changed), (void *)BORDER_RIGHT);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_BorderTopSpinner), ETK_CALLBACK(on_BorderSpinner_value_changed), (void *)BORDER_TOP);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_BorderBottomSpinner), ETK_CALLBACK(on_BorderSpinner_value_changed), (void *)BORDER_BOTTOM);

	return UI_ImageFrame;
}

Etk_Widget* create_text_frame(void){
	Etk_Widget *vbox;
	Etk_Widget *hbox;
	Etk_Widget *label;
	Etk_Widget *table;
	Etk_Combobox_Item *ComboItem; 	
	
	//TextFrame
	UI_TextFrame = etk_frame_new("Text");

		//vbox
		vbox = etk_vbox_new(ETK_FALSE, 5);
		etk_container_add(ETK_CONTAINER(UI_TextFrame), vbox);
			
			//table
			table = etk_table_new (4, 4, FALSE);
			etk_box_append(ETK_BOX(vbox), table, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	
				label = etk_label_new("Text");
				etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
				etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 0,0);
				
				//Text Entry
				UI_TextEntry = etk_entry_new ();
				//etk_widget_size_request_set(UI_TextEntry,30, 30);
				etk_table_attach_default (ETK_TABLE(table),UI_TextEntry, 1, 3, 0,0);
	
				label = etk_label_new("Font");
				etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
				etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 1,1);
				
				//FontComboBox
				UI_FontComboBox = etk_combobox_new();
				etk_combobox_column_add(ETK_COMBOBOX(UI_FontComboBox), ETK_COMBOBOX_IMAGE, 24, ETK_FALSE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_column_add(ETK_COMBOBOX(UI_FontComboBox), ETK_COMBOBOX_LABEL, 75, ETK_TRUE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_build(ETK_COMBOBOX(UI_FontComboBox));
				etk_combobox_item_append(ETK_COMBOBOX(UI_FontComboBox), etk_image_new_from_stock (ETK_STOCK_DOCUMENT_OPEN,ETK_STOCK_MEDIUM), "Insert a new font");
				etk_table_attach_default (ETK_TABLE(table),UI_FontComboBox, 1, 1, 1,1);
				
				label = etk_label_new("Size");
				etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
				etk_table_attach_default (ETK_TABLE(table),label, 2, 2, 1,1);
				
				//FontSizeSpinner
				UI_FontSizeSpinner =  etk_spinner_new (0, 200, 0, 1, 10);
				etk_widget_size_request_set(UI_FontSizeSpinner, 45, 20);
				etk_table_attach_default (ETK_TABLE(table),UI_FontSizeSpinner, 3, 3, 1,1);
			
				label = etk_label_new("Effect");
				etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 2,2);
		
				//PartEffectComboBox
				UI_EffectComboBox = etk_combobox_new();
				etk_combobox_column_add(ETK_COMBOBOX(UI_EffectComboBox), ETK_COMBOBOX_IMAGE, 24, ETK_FALSE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_column_add(ETK_COMBOBOX(UI_EffectComboBox), ETK_COMBOBOX_LABEL, 75, ETK_TRUE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_build(ETK_COMBOBOX(UI_EffectComboBox));
				ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox), etk_image_new_from_edje (EdjeFile->str,"NONE.PNG"), "Plain");
				etk_combobox_item_data_set (ComboItem, (void*)FX_PLAIN);
				ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox), etk_image_new_from_edje (EdjeFile->str,"NONE.PNG"), "Outline");
				etk_combobox_item_data_set (ComboItem, (void*)FX_OUTLINE);
				ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox), etk_image_new_from_edje (EdjeFile->str,"NONE.PNG"), "Soft Outline");
				etk_combobox_item_data_set (ComboItem, (void*)FX_SOFT_OUTLINE);
				ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox), etk_image_new_from_edje (EdjeFile->str,"NONE.PNG"), "Shadow");
				etk_combobox_item_data_set (ComboItem, (void*)FX_SHADOW);
				ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox), etk_image_new_from_edje (EdjeFile->str,"NONE.PNG"), "Soft Shadow");
				etk_combobox_item_data_set (ComboItem, (void*)FX_SOFT_SHADOW);
				ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox), etk_image_new_from_edje (EdjeFile->str,"NONE.PNG"), "Outline Shadow");
				etk_combobox_item_data_set (ComboItem, (void*)FX_OUTLINE_SHADOW);
				ComboItem = etk_combobox_item_append(ETK_COMBOBOX(UI_EffectComboBox), etk_image_new_from_edje (EdjeFile->str,"NONE.PNG"), "Outline Soft Shadow");
				etk_combobox_item_data_set (ComboItem, (void*)FX_OUTLINE_SOFT_SHADOW);
				etk_table_attach_default (ETK_TABLE(table),UI_EffectComboBox, 1, 3, 2,2);
					
				label = etk_label_new("Alpha");
				etk_table_attach_default (ETK_TABLE(table),label, 0, 0, 3,3);
			
			
				//TextAlphaSlider
				UI_TextAlphaSlider = etk_hslider_new (0, 255, 15, 1,20);
				etk_table_attach_default (ETK_TABLE(table),UI_TextAlphaSlider, 1, 3, 3,3);
			
					
			//hbox
			hbox = etk_hbox_new(ETK_FALSE, 10);
			etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
			
				//Color buttons
				etk_box_append(ETK_BOX(hbox), create_a_color_button("Text",COLOR_OBJECT_TEXT), ETK_BOX_START, ETK_BOX_EXPAND, 0);	
				etk_box_append(ETK_BOX(hbox), create_a_color_button("Shadow",COLOR_OBJECT_SHADOW), ETK_BOX_START, ETK_BOX_EXPAND, 0);	
				etk_box_append(ETK_BOX(hbox), create_a_color_button("Outline",COLOR_OBJECT_OUTLINE), ETK_BOX_START, ETK_BOX_EXPAND, 0);	


	etk_signal_connect("active_item_changed", ETK_OBJECT(UI_FontComboBox), ETK_CALLBACK(on_FontComboBox_changed), NULL);
	etk_signal_connect("active_item_changed", ETK_OBJECT(UI_EffectComboBox), ETK_CALLBACK(on_EffectComboBox_changed), NULL);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_FontSizeSpinner), ETK_CALLBACK(on_FontSizeSpinner_value_changed), NULL);
	etk_signal_connect("text_changed", ETK_OBJECT(UI_TextEntry), ETK_CALLBACK(on_TextEntry_text_changed), NULL);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_TextAlphaSlider), ETK_CALLBACK(on_TextAlphaSlider_value_changed), NULL);

	
	return UI_TextFrame;
}

Etk_Widget* create_position_frame(void){
	Etk_Widget *vbox;
	Etk_Widget *hbox;
	Etk_Widget *label;
	
	//PositionFrame
	UI_PositionFrame = etk_frame_new("Position");
	
/* 		//Position Notebook
		notebook = etk_notebook_new ();
		etk_container_add(ETK_CONTAINER(UI_PositionFrame), notebook);
 */			
/* 			//Simple TAB
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
			etk_container_add(ETK_CONTAINER(UI_PositionFrame), vbox);
//			etk_notebook_page_append (ETK_NOTEBOOK(notebook), "Advanced", vbox);
			
			label = etk_label_new("<color=#FF0000><b>First_Point</b></>");
			etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
			etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);	
			
			//hbox
			hbox = etk_hbox_new(ETK_FALSE, 0);
			etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);	
				
			
				label = etk_label_new("<color=#FF0000><b>X</b></>");
				etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);	
			
			 	//Rel1XSpinner
				UI_Rel1XSpinner = etk_spinner_new (0.0, 1.0, 0.0, 0.01, 0.1);
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
				etk_combobox_column_add(ETK_COMBOBOX(UI_Rel1ToXComboBox), ETK_COMBOBOX_IMAGE, 24, ETK_FALSE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_column_add(ETK_COMBOBOX(UI_Rel1ToXComboBox), ETK_COMBOBOX_LABEL, 75, ETK_TRUE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_build(ETK_COMBOBOX(UI_Rel1ToXComboBox));
				etk_box_append(ETK_BOX(hbox), UI_Rel1ToXComboBox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
				
			
			//hbox
			hbox = etk_hbox_new(ETK_FALSE, 0);
			etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);	
				
				label = etk_label_new("<color=#FF0000><b>Y</b></>");
				etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);	
				
				//Rel1YSpinner
				UI_Rel1YSpinner = etk_spinner_new (0.0, 1.0, 0.0, 0.01, 0.1);
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
				etk_combobox_column_add(ETK_COMBOBOX(UI_Rel1ToYComboBox), ETK_COMBOBOX_IMAGE, 24, ETK_FALSE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_column_add(ETK_COMBOBOX(UI_Rel1ToYComboBox), ETK_COMBOBOX_LABEL, 75, ETK_TRUE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
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
				UI_Rel2XSpinner = etk_spinner_new (0.0, 1.0, 0.0, 0.01, 0.1);
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
				etk_combobox_column_add(ETK_COMBOBOX(UI_Rel2ToXComboBox), ETK_COMBOBOX_IMAGE, 24, ETK_FALSE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_column_add(ETK_COMBOBOX(UI_Rel2ToXComboBox), ETK_COMBOBOX_LABEL, 75, ETK_TRUE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_build(ETK_COMBOBOX(UI_Rel2ToXComboBox));
				etk_box_append(ETK_BOX(hbox), UI_Rel2ToXComboBox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
				
				
				
			//hbox
			hbox = etk_hbox_new(ETK_FALSE, 0);
			etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);	
				
				label = etk_label_new("<color=#0000FF><b>Y</b></>");
				etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);	
			
				//Rel2YSpinner
				UI_Rel2YSpinner = etk_spinner_new (0.0, 1.0, 0.0, 0.01, 0.1);
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
				etk_combobox_column_add(ETK_COMBOBOX(UI_Rel2ToYComboBox), ETK_COMBOBOX_IMAGE, 24, ETK_FALSE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_column_add(ETK_COMBOBOX(UI_Rel2ToYComboBox), ETK_COMBOBOX_LABEL, 75, ETK_TRUE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
				etk_combobox_build(ETK_COMBOBOX(UI_Rel2ToYComboBox));
				etk_box_append(ETK_BOX(hbox), UI_Rel2ToYComboBox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
			
			
	etk_signal_connect("value_changed", ETK_OBJECT(UI_Rel1XOffsetSpinner), ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void *)REL1X_SPINNER);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_Rel1YOffsetSpinner), ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void *)REL1Y_SPINNER);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_Rel2XOffsetSpinner), ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void *)REL2X_SPINNER);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_Rel2YOffsetSpinner), ETK_CALLBACK(on_RelOffsetSpinner_value_changed), (void *)REL2Y_SPINNER);

			
	etk_signal_connect("value_changed", ETK_OBJECT(UI_Rel1XSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void *)REL1X_SPINNER);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_Rel1YSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void *)REL1Y_SPINNER);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_Rel2XSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void *)REL2X_SPINNER);
	etk_signal_connect("value_changed", ETK_OBJECT(UI_Rel2YSpinner), ETK_CALLBACK(on_RelSpinner_value_changed), (void *)REL2Y_SPINNER);
		
	etk_signal_connect("active_item_changed", ETK_OBJECT(UI_Rel1ToXComboBox), ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL1X_SPINNER);
	etk_signal_connect("active_item_changed", ETK_OBJECT(UI_Rel1ToYComboBox), ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL1Y_SPINNER);
	etk_signal_connect("active_item_changed", ETK_OBJECT(UI_Rel2ToXComboBox), ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL2X_SPINNER);
	etk_signal_connect("active_item_changed", ETK_OBJECT(UI_Rel2ToYComboBox), ETK_CALLBACK(on_RelToComboBox_changed), (void *)REL2Y_SPINNER);

			
	return UI_PositionFrame;
}
Etk_Widget* create_part_frame(void){
	Etk_Widget *table;
	Etk_Widget *label;
	Etk_Widget *combo;
	
	//PartFrame
	UI_PartFrame = etk_frame_new("Part property");
		
		//table
		table = etk_table_new (2, 3, FALSE);
		etk_container_add(ETK_CONTAINER(UI_PartFrame), table);
		
		
		//PartNameEntry
		label = etk_label_new("<b>Name</b>");
		etk_table_attach (ETK_TABLE(table), label, 0, 0, 0, 0,0,0,ETK_TABLE_NONE);
		UI_PartNameEntry = etk_entry_new();
		etk_table_attach_default (ETK_TABLE(table),UI_PartNameEntry, 1, 1, 0, 0);
		
		//PartClipToComboBox
		label = etk_label_new("<b>Clip_to</b>");
		etk_table_attach (ETK_TABLE(table), label, 0, 0, 2, 2,0,0,ETK_TABLE_NONE);
		
		combo = etk_combobox_new();
		etk_combobox_column_add(ETK_COMBOBOX(combo), ETK_COMBOBOX_IMAGE, 24, ETK_FALSE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
		etk_combobox_column_add(ETK_COMBOBOX(combo), ETK_COMBOBOX_LABEL, 75, ETK_TRUE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
		etk_combobox_build(ETK_COMBOBOX(combo));
		etk_combobox_item_append(ETK_COMBOBOX(combo), etk_image_new_from_edje (EdjeFile->str,"NONE.PNG"), "TODO");
		
		etk_table_attach_default (ETK_TABLE(table), combo, 1, 1, 2, 2);
		
		etk_signal_connect("text_changed", ETK_OBJECT(UI_PartNameEntry), ETK_CALLBACK(on_PartNameEntry_text_changed), NULL);

	return UI_PartFrame;
}


void create_main_window (void){
	Etk_Widget *vbox,*hbox;
	Etk_Widget *frame;

	
	
	UI_FileChooserDialog = create_filechooser_dialog();
		
	UI_PlayDialog = create_play_dialog();
	
	//Alert Dialog
	UI_AlertDialog = etk_message_dialog_new (ETK_MESSAGE_DIALOG_INFO, ETK_MESSAGE_DIALOG_OK, "Hallo world!");
	etk_widget_size_request_set(UI_AlertDialog, 240, 100);
	etk_signal_connect("delete_event", ETK_OBJECT(UI_AlertDialog), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
	etk_signal_connect("response", ETK_OBJECT(UI_AlertDialog), ETK_CALLBACK(on_AlertDialog_response), NULL);	

	//Create ColorPicker Window
	UI_ColorWin = etk_window_new();
	etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Choose color");
	
	
		//ColorPicker
		UI_ColorPicker = etk_colorpicker_new();
		etk_container_add(ETK_CONTAINER(UI_ColorWin), UI_ColorPicker);
	
	
	
	//UI_MainWin
	UI_MainWin = etk_window_new();
	etk_window_title_set(ETK_WINDOW(UI_MainWin), "Edje Editor");
	etk_container_border_width_set(ETK_CONTAINER(UI_MainWin), 2);
	
	//hbox
	hbox = etk_hbox_new(ETK_FALSE, 0);
	etk_container_add(ETK_CONTAINER(UI_MainWin), hbox);
		
		//ToolBar
		etk_box_append(ETK_BOX(hbox), create_toolbar(), ETK_BOX_START, ETK_BOX_NONE, 0);

		//vbox
		vbox = etk_vbox_new(ETK_FALSE, 0);
		etk_box_append(ETK_BOX(hbox), vbox, ETK_BOX_START, ETK_BOX_NONE, 0);

			//Group Frame
			etk_box_append(ETK_BOX(vbox), create_group_frame(), ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
		
		 	//Description Frame
			etk_box_append(ETK_BOX(vbox), create_description_frame(), ETK_BOX_START, ETK_BOX_NONE, 0);
				
			//Rectangle Frame
			etk_box_append(ETK_BOX(vbox), create_rectangle_frame(), ETK_BOX_START, ETK_BOX_NONE, 0);	
			
			//Image Frame
			etk_box_append(ETK_BOX(vbox), create_image_frame(), ETK_BOX_START, ETK_BOX_NONE, 0);	
			
			//Text Frame
			etk_box_append(ETK_BOX(vbox), create_text_frame(), ETK_BOX_START, ETK_BOX_NONE, 0);	
			
			//Position Frame
			etk_box_append(ETK_BOX(vbox), create_position_frame(), ETK_BOX_START, ETK_BOX_NONE, 0);	
			
			//Part Frame
			etk_box_append(ETK_BOX(vbox), create_part_frame(), ETK_BOX_START, ETK_BOX_NONE, 0);	
		
	
		//canvas frame
		frame = etk_frame_new(NULL);
		etk_container_border_width_set (ETK_CONTAINER(frame), 0);
		etk_widget_size_request_set(frame, 300, 300);
		etk_box_append(ETK_BOX(hbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
		
		//canvas
		ETK_canvas = etk_canvas_new ();
		etk_container_add(ETK_CONTAINER(frame), ETK_WIDGET(ETK_canvas));
		
		
		//canvas bg
		EV_canvas_bg = evas_object_image_add(etk_widget_toplevel_evas_get(ETK_canvas));
		evas_object_image_file_set(EV_canvas_bg, EdjeFile->str, "images/0");		//TODO Find a method to load by name and not by number
		evas_object_image_fill_set( EV_canvas_bg,	0,0,128,128); 	
		etk_canvas_object_add (ETK_CANVAS(ETK_canvas), EV_canvas_bg);
		evas_object_show(EV_canvas_bg);
		//canvas shadow
	 	EV_canvas_shadow = evas_object_image_add(etk_widget_toplevel_evas_get(ETK_canvas));
		evas_object_image_file_set(EV_canvas_shadow, EdjeFile->str, "images/1");
		evas_object_image_fill_set( EV_canvas_shadow,	0,0,480,480); 	
		etk_canvas_object_add (ETK_CANVAS(ETK_canvas), EV_canvas_shadow);
		//etk_canvas_object_move (ETK_CANVAS(ETK_canvas), EV_canvas_shadow);
		evas_object_show(EV_canvas_shadow); 
		
	// SIGNALS 
	
	etk_signal_connect("color_changed", ETK_OBJECT(UI_ColorPicker), ETK_CALLBACK(on_ColorDialog_change), NULL);
	etk_signal_connect("delete_event", ETK_OBJECT(UI_ColorWin), ETK_CALLBACK(etk_window_hide_on_delete), NULL);	
	etk_signal_connect("destroyed", ETK_OBJECT(UI_MainWin), ETK_CALLBACK(etk_main_quit_cb), NULL);
	etk_signal_connect("active_item_changed", ETK_OBJECT(UI_GroupComboBox), ETK_CALLBACK(on_GroupComboBox_changed), NULL);
	etk_object_notification_callback_add(ETK_OBJECT(ETK_canvas), "geometry", on_canvas_geometry_changed, NULL);
  

	etk_widget_show_all(ETK_WIDGET(UI_MainWin));
	
	/* etk_widget_hide(UI_DescriptionFrame);
	etk_widget_hide(UI_PositionFrame);
	etk_widget_hide(UI_PartFrame);
	etk_widget_hide(UI_RectFrame);
	etk_widget_hide(UI_ImageFrame);
	etk_widget_hide(UI_TextFrame); */
}
