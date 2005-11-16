#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"


int ewl_iconbox_icon_label_height_calculate(Ewl_IconBox_Icon* icon) {
	int height=0;
	int ww,hh;

	evas_object_textblock_size_native_get(EWL_TEXT(icon->w_label)->textblock, &ww, &hh);
	height = CURRENT_H(icon->image) + hh;

	

	return height;
}


void ewl_iconbox_icon_floater_resize_cb(Ewl_Widget *w, void *ev_data, void *user_data) {
	Ewl_IconBox_Icon* icon = EWL_ICONBOX_ICON(user_data);


	ewl_object_custom_h_set(EWL_OBJECT(icon->floater),ewl_iconbox_icon_label_height_calculate(EWL_ICONBOX_ICON(icon)) );
	
	//printf("Resized floater to EWL_TEXT(%d)-TEXTBLOCK(%d) %d\n", CURRENT_H(icon->w_label), hh, height);
}



/*Ecore_List *ewl_iconbox_icon_list;*/

/**
 * @return Returns NULL on failure, a new Ewl_IconBox on success
 * @brief Creates a new Ewl_IconBox
 */
Ewl_Widget *ewl_iconbox_new()
{
	Ewl_IconBox* ib;
	DENTER_FUNCTION (DLEVEL_STABLE);

	ib = NEW(Ewl_IconBox, 1);
	if (!ib) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_iconbox_init(ib)) {
		DWARNING("Failed iconbox init...\n");
		FREE(ib);
		ib = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(ib), DLEVEL_STABLE);
}

Ewl_Widget *ewl_iconbox_icon_new()
{
	Ewl_IconBox_Icon* icon;
	DENTER_FUNCTION (DLEVEL_STABLE);

	icon = NEW(Ewl_IconBox_Icon, 1);
	if (!icon) {
		DWARNING("Failed to init icon..\n");
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_iconbox_icon_init(icon)) {
		DWARNING("Failed iconbox init...\n");
		FREE(icon);
		icon = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(icon), DLEVEL_STABLE);

}

void ewl_iconbox_inner_pane_calculate(Ewl_IconBox* ib)
{

	int pw,ph;
	int sw,sh;
	int nw=0,nh=0;


	/*printf ("%d:%d\n", ev->w, ev->h);*/
	
	ewl_object_current_size_get(EWL_OBJECT(ib->ewl_iconbox_scrollpane), &sw,&sh);
	ewl_object_current_size_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner), &pw,&ph);

	if (sw > pw) nw = sw;
	if (sh > ph) nh = sh;

	if (nh | nw) {
		if (!nh) nh = sh;
	 	if (!nw) nw = sw;
		ewl_object_custom_size_set(EWL_OBJECT(ib->ewl_iconbox_pane_inner), nw+50, nh+50);
		/*printf ("Grew iconbox to: %d:%d\n", nw, nh);*/
	}


}

int ewl_iconbox_icon_init(Ewl_IconBox_Icon* icon)
{
	Ewl_Widget *w;

	w = EWL_WIDGET(icon);
	if (!ewl_box_init(EWL_BOX(icon))) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_box_orientation_set(EWL_BOX(icon), EWL_ORIENTATION_VERTICAL);
	icon->w_label = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(icon->w_label), NULL);
	icon->image = ewl_image_new();

	icon->label = NULL;
	icon->label_compressed = NULL;


	

	/* Init ewl setup */
	ewl_widget_appearance_set(EWL_WIDGET(icon), "icon");
	ewl_widget_inherit(EWL_WIDGET(w), "icon");

	ewl_container_child_append(EWL_CONTAINER(icon), icon->image);
	ewl_container_child_append(EWL_CONTAINER(icon), icon->w_label);

	DRETURN_INT(TRUE, DLEVEL_STABLE);

	
}



int ewl_iconbox_init(Ewl_IconBox* ib)
{
	Ewl_Widget *w;

	w = EWL_WIDGET(ib);
	
	if (!ewl_box_init(EWL_BOX(ib)))
			DRETURN_INT(FALSE, DLEVEL_STABLE);

	/* Init ewl setup */
	ewl_box_orientation_set(EWL_BOX(ib), EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_appearance_set(EWL_WIDGET(ib), "iconbox");
	ewl_widget_inherit(EWL_WIDGET(w), "iconbox");

	ib->drag_icon = NULL;
	

	/*Default to non-editable labels */
	ewl_iconbox_editable_set(ib,0);
	
	
	ib->ewl_iconbox_scrollpane = ewl_scrollpane_new();
	if (ib->ewl_iconbox_scrollpane) {
		ewl_scrollpane_hscrollbar_flag_set(EWL_SCROLLPANE(ib->ewl_iconbox_scrollpane), EWL_SCROLLPANE_FLAG_AUTO_VISIBLE);
	} else {
		/*printf ("Bombed out on scrollpane creation\n");*/
	}

	/*Start with an empty background*/
	ib->background = NULL;

	/*Set the defaults to 0 for layout*/
	ib->lx = ib->ly = ib->iw = ib->ih = 0;
	
	ib->ewl_iconbox_pane_inner = ewl_overlay_new();
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_scrollpane), ib->ewl_iconbox_pane_inner);
		
	/*** Context menu **/
	/*Make the menu floater */
	ib->ewl_iconbox_menu_floater = ewl_floater_new();
	ewl_floater_follow_set(EWL_FLOATER(ib->ewl_iconbox_menu_floater),
			       ib->ewl_iconbox_pane_inner);

	/*-------------------------------------------*/
	/* Get the context menu ready */
	ib->ewl_iconbox_context_menu = ewl_menu_new();
	ewl_menu_item_text_set(EWL_MENU_ITEM(ib->ewl_iconbox_context_menu), "");
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_menu_floater), ib->ewl_iconbox_context_menu);
	ewl_widget_show(ib->ewl_iconbox_context_menu);

	/* Add auto-arrange ability */
	ib->ewl_iconbox_view_menu = ewl_menu_new();
	ewl_menu_item_text_set(EWL_MENU_ITEM(ib->ewl_iconbox_view_menu),
			       "View");
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_context_menu), ib->ewl_iconbox_view_menu);
	
	ib->ewl_iconbox_context_menu_item = ewl_menu_item_new();
	ewl_menu_item_text_set(EWL_MENU_ITEM(ib->ewl_iconbox_context_menu_item),
			       "Auto-Arrange");
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_view_menu), ib->ewl_iconbox_context_menu_item);
	ewl_callback_append(ib->ewl_iconbox_context_menu_item, EWL_CALLBACK_MOUSE_DOWN, ewl_iconbox_arrange_cb, ib);
	ewl_widget_show(ib->ewl_iconbox_context_menu_item);

	ib->ewl_iconbox_context_menu_item = ewl_menu_item_new();
	ewl_menu_item_text_set(EWL_MENU_ITEM(ib->ewl_iconbox_context_menu_item), "Expansion Test");
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_view_menu), ib->ewl_iconbox_context_menu_item);
	ewl_callback_append(ib->ewl_iconbox_context_menu_item, EWL_CALLBACK_MOUSE_DOWN, ewl_iconbox_expansion_cb, ib);
	ewl_widget_show(ib->ewl_iconbox_context_menu_item);

	/*ib->ewl_iconbox_context_menu_item = ewl_separator_new();
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_context_menu), ib->ewl_iconbox_context_menu_item);
	ewl_widget_show(ib->ewl_iconbox_context_menu_item);*/
	ewl_widget_show(ib->ewl_iconbox_view_menu);
		
	/* Add the menu floater to the pane inner */
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_pane_inner), ib->ewl_iconbox_menu_floater);

	/* ------------------------------ Menu */


	/*------------------------------------------*/
	/*Icon menu*/

	ib->icon_menu_floater = ewl_floater_new();
	ewl_floater_follow_set(EWL_FLOATER(ib->icon_menu_floater),
			       ib->ewl_iconbox_pane_inner);
	
	ib->icon_menu = ewl_menu_new();
	ewl_menu_item_text_set(EWL_MENU_ITEM(ib->icon_menu), "");
	ewl_container_child_append(EWL_CONTAINER(ib->icon_menu_floater), ib->icon_menu);
	ewl_widget_show(ib->icon_menu);

	ib->icon_menu_item = ewl_menu_item_new();
	ewl_menu_item_text_set(EWL_MENU_ITEM(ib->icon_menu_item),
			       "Icon-Context");
	ewl_container_child_append(EWL_CONTAINER(ib->icon_menu), ib->icon_menu_item);
	ewl_widget_show(ib->icon_menu_item);

			       
		
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_pane_inner), ib->icon_menu_floater);

	
	
	/*-----------------------------------*/



	ewl_object_fill_policy_set(EWL_OBJECT(ib->ewl_iconbox_menu_floater), EWL_FLAG_FILL_FILL);
	ewl_object_fill_policy_set(EWL_OBJECT(ib->icon_menu_floater), EWL_FLAG_FILL_FILL);
	

	/* Add the iconbox scrollpane to the iconbox */
	ewl_container_child_append(EWL_CONTAINER(ib), ib->ewl_iconbox_scrollpane);
	ewl_object_fill_policy_set(EWL_OBJECT(ib), EWL_FLAG_FILL_ALL);

	/* Create the selector / selector floater */
	ib->select_floater = ewl_floater_new();
	ewl_floater_follow_set(EWL_FLOATER(ib->select_floater),
				ib->ewl_iconbox_pane_inner);
	ewl_object_fill_policy_set(EWL_OBJECT(ib->select_floater), EWL_FLAG_FILL_FILL);
	ib->select =ewl_button_new();
	
	ewl_container_child_append(EWL_CONTAINER(ib->select_floater), ib->select);
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_pane_inner), ib->select_floater);
	
	ewl_object_custom_size_set(EWL_OBJECT(ib->select), 80, 40);
	ewl_widget_layer_set(EWL_WIDGET(ib->select_floater), -1);
	ewl_widget_color_set(EWL_WIDGET(ib->select), 128, 50, 70, 128);
	ib->drag_box = 0;

	/*Set the dx/dy drag start points to 'null' values*/
	ib->dx = -1; ib->dy = -1;
	/* -------------------------------- */

	ewl_widget_show(ib->select);
	/*ewl_widget_show(ib->select_floater);*/


	/*Init the icon list*/
	ib->ewl_iconbox_icon_list = ecore_list_new();

	/* Show widgets */
	ewl_widget_show(ib->ewl_iconbox_pane_inner);
	ewl_widget_show(ib->ewl_iconbox_scrollpane);
	ewl_widget_show(ib->ewl_iconbox_menu_floater);
	ewl_widget_show(ib->icon_menu_floater);
	


	/* Ewl Entry for the purposes of label editing - if enabled */
	ib->entry = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(ib->entry), "Test");


	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(EWL_ENTRY(ib->entry)->cursor), 50);
	ewl_widget_show(ib->entry);
	ib->entry_floater = ewl_floater_new();
	ewl_floater_follow_set(EWL_FLOATER(ib->entry_floater),
				ib->ewl_iconbox_pane_inner);
	/*ewl_widget_show(ib->entry_floater);*/
	ib->entry_box = ewl_hbox_new();
	ewl_widget_show(ib->entry_box);
	ewl_container_child_append(EWL_CONTAINER(ib->entry_floater), ib->entry_box);
	ewl_container_child_append(EWL_CONTAINER(ib->entry_box), ib->entry);
	
	ewl_object_custom_size_set(EWL_OBJECT(ib->entry_box), 50, 15);
	ewl_object_fill_policy_set(EWL_OBJECT(ib->entry_floater), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_pane_inner), ib->entry_floater);


	
	

	/* Bind a callback to keypress on the entry, so we can update the label */
	ewl_callback_append(ib->entry, EWL_CALLBACK_KEY_DOWN, ewl_iconbox_label_edit_key_down, ib);
	
	/* ----------------------------- */


	/** Internal Callbacks */
	ewl_callback_append(ib->ewl_iconbox_pane_inner, EWL_CALLBACK_MOUSE_MOVE, ewl_iconbox_mouse_move_cb, ib);
	ewl_callback_append(ib->ewl_iconbox_pane_inner, EWL_CALLBACK_MOUSE_DOWN, ewl_iconbox_pane_mouse_down_cb, ib);
	ewl_callback_append(ib->ewl_iconbox_pane_inner, EWL_CALLBACK_MOUSE_UP, ewl_iconbox_mouse_up, ib);
	ewl_callback_append(EWL_WIDGET(ib), EWL_CALLBACK_CONFIGURE, ewl_iconbox_configure_cb, NULL);
	ewl_callback_append(EWL_WIDGET(ib), EWL_CALLBACK_DESTROY, ewl_iconbox_destroy_cb, NULL);


	/*printf("Setup the iconbox...\n");*/
	DRETURN_INT(TRUE, DLEVEL_STABLE);

}



void ewl_iconbox_icon_size_custom_set(Ewl_IconBox* ib, int w, int h) {
	ib->iw = w;
	ib->ih = h;
}



void ewl_iconbox_context_menu_item_add(Ewl_IconBox* ib, Ewl_Widget* item) {
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_context_menu), item);
}

void ewl_iconbox_icon_menu_item_add(Ewl_IconBox* ib, Ewl_Widget* item) {
	ewl_object_minimum_size_set(EWL_OBJECT(item), 100,15);
	ewl_container_child_append(EWL_CONTAINER(ib->icon_menu), item);
}



Ecore_List* ewl_iconbox_get_selection(Ewl_IconBox* ib) {
	Ewl_IconBox_Icon* list_item;
	Ecore_List* selected = ecore_list_new();
	
	ecore_list_goto_first(ib->ewl_iconbox_icon_list);
	while((list_item = (Ewl_IconBox_Icon*)ecore_list_next(ib->ewl_iconbox_icon_list)) != NULL) {
		if (list_item->selected) ecore_list_append(selected, list_item);
	}

	return selected;
}

/*Move to the root of the scrollpame*/
void ewl_iconbox_scrollpane_goto_root(Ewl_IconBox* ib) {
        ewl_scrollpane_hscrollbar_value_set(EWL_SCROLLPANE(ib->ewl_iconbox_scrollpane),0);
        ewl_scrollpane_vscrollbar_value_set(EWL_SCROLLPANE(ib->ewl_iconbox_scrollpane),0);

}


void ewl_iconbox_background_set(Ewl_IconBox* ib, char* file)
{
	/*Add a background image*/
	int w,h;

	if (ib->background) {
		ewl_widget_destroy(ib->background);
		ib->background = NULL;
	}

	if (!file) return;

	w = CURRENT_W(ib->ewl_iconbox_pane_inner);
	h= CURRENT_H(ib->ewl_iconbox_pane_inner);

	ib->background = ewl_image_new();
	ewl_object_custom_size_set(EWL_OBJECT(ib->background), w,h);
	ewl_image_file_set(EWL_IMAGE(ib->background), file,0);
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_pane_inner), ib->background);
	ewl_widget_show(ib->background);
}



void ewl_iconbox_icon_label_setup(Ewl_IconBox_Icon* icon, char* text)
{
	
	char* compressed;


	
	/*If we have a current label, nuke it*/
	if (icon->label) {
		free(icon->label);
	}

	/*Copy the existing label to our icon*/
	icon->label = strdup(text);
	


	if (strlen(text) <= LABEL_CHARS) {
		ewl_iconbox_icon_label_set(icon, icon->label);
		icon->label_compressed = NULL;
	} else {
		if (icon->label_compressed) {
			free(icon->label_compressed);
		}
		
		compressed = malloc(sizeof(char) * LABEL_CHARS + 3);
		strncpy(compressed, text, LABEL_CHARS);
		strcpy(compressed+LABEL_CHARS, "..\0");
		ewl_iconbox_icon_label_set(icon, compressed);

		icon->label_compressed = compressed;
	}

	
}

void ewl_iconbox_icon_label_set(Ewl_IconBox_Icon* icon, char* text)
{
	int wrap = 0;
	
	/*ewl_text_wrap_set(EWL_TEXT(icon->w_label), 1);*/
	
	if (text) {
		
		ewl_text_text_set(EWL_TEXT(icon->w_label), text);

		wrap = 10;
		while (wrap < (int)strlen(text)) {
			ewl_text_cursor_position_set(EWL_TEXT(icon->w_label), wrap);
			ewl_text_text_insert(EWL_TEXT(icon->w_label), "\n", wrap);

			wrap += 10;
		}
	} else {
		ewl_text_text_set(EWL_TEXT(icon->w_label), icon->label);

	}
		
}


void ewl_iconbox_label_edit_key_down(Ewl_Widget *w __UNUSED__, void *ev_data, void* user_data)
{
	Ewl_Event_Key_Down* ev = ev_data;
	Ewl_IconBox* ib = EWL_ICONBOX(user_data);
	
	if (!strcmp(ev->keyname, "Return")) {
		char* text = ewl_text_text_get(EWL_TEXT(ib->entry));
		
		ewl_iconbox_icon_label_set(EWL_ICONBOX_ICON(ib->edit_icon), text);
		free(text);

		/*printf ("Setting label to: '%s'", ewl_text_text_get(EWL_TEXT(ib->entry)));*/

		/*Hide the entry, now that we have an enter */
		ewl_widget_hide(ib->entry_floater);

		/*Show the label again*/
		ewl_widget_show(EWL_ICONBOX_ICON(ib->edit_icon)->w_label);
	}
}

void ewl_iconbox_editable_set(Ewl_IconBox* ib, int edit)
{
	ib->editable = edit;
}


void ewl_iconbox_scrollpane_recalculate(Ewl_IconBox* ib) {
	int pw,ph;
	
	ewl_object_current_size_get(EWL_OBJECT(ib->ewl_iconbox_scrollpane), &pw, &ph);	
	ewl_object_custom_size_set(EWL_OBJECT(ib->ewl_iconbox_pane_inner), 
			pw > ib->lx ? pw : ib->lx, 
			ph > ib->ly+ib->ih ? ph+ib->ih : ib->ly+(ib->ih*2)+(EWL_ICONBOX_ICON_PADDING*2)  );

	
}

void ewl_iconbox_icon_arrange(Ewl_IconBox* ib)
{
	
	int sw=0,sh=0;
	int iw=0, ih=0;
	Ewl_IconBox_Icon* list_item;
	int maxx=0, maxy=0;
	/*int pw, ph;*/
	int x,y;

	/*printf ("Entering the arrange..\n");*/

	/* Recalc inner pane size */
	/*ewl_iconbox_inner_pane_calculate(ib);*/
	
	/*printf ("Ewl_IconBox -> Arranging icons\n");*/


	ib->lx = ib->ly = 0;
	
	ewl_object_current_size_get(EWL_OBJECT(ib->ewl_iconbox_scrollpane), &sw,&sh);
	
	if (ib->iw > 0 && ib->ih > 0) {
		iw = ib->iw;
		ih = ib->ih;
	} else {	
		ecore_list_goto_first(ib->ewl_iconbox_icon_list);
		while((list_item = (Ewl_IconBox_Icon*)ecore_list_next(ib->ewl_iconbox_icon_list)) != NULL) {
			int nw,nh;
		
			nw= ewl_object_preferred_w_get(EWL_OBJECT(list_item->image));
			nh= ewl_object_preferred_h_get(EWL_OBJECT(list_item->image));
			if (nw > iw) iw = nw;
			if (nh > ih) ih = nh;
		}
	}

	ecore_list_goto_first(ib->ewl_iconbox_icon_list);
	while((list_item = (Ewl_IconBox_Icon*)ecore_list_next(ib->ewl_iconbox_icon_list)) != NULL) {
		if (iw < EWL_ICONBOX_MOVE_TOLERANCE) {
			iw = EWL_ICONBOX_MINIMUM_SIZE;
			ih = EWL_ICONBOX_MINIMUM_SIZE;
		}

		
		if (ib->lx + ib->iw + (EWL_ICONBOX_ICON_PADDING) >= (sw - ib->iw)) {
			ib->lx = 0;
			ib->ly += ih + EWL_ICONBOX_ICON_PADDING;
		}

		x = EWL_FLOATER(list_item->floater)->x;
		y = EWL_FLOATER(list_item->floater)->y;
	
		/*Only move if we have to*/
		if ( abs(x - ib->lx) > 0  ||
		     abs(y - ib->ly) > 0) 
			ewl_floater_position_set(EWL_FLOATER(list_item->floater), ib->lx, ib->ly);

		
		ib->lx += iw + EWL_ICONBOX_ICON_PADDING;
		
		if (ib->lx > maxx) maxx = ib->lx;
		if (ib->ly > maxy) maxy = ib->ly;

		
	}

	/* Now set the extent of the pane inner to be the maxx/y that we had or 
	 * the size of the scrollpane, which ever is bigger
	 */
	ewl_iconbox_scrollpane_recalculate(ib);



}
/* ------------ */




void ewl_iconbox_icon_select(Ewl_IconBox_Icon* ib, int loc, int deselect) /* Loc 0= image, 1= label */
{
	
	int sel = ib->selected;

	if ((!ib->icon_box_parent->drag_box) && deselect) {
		Ewl_IconBox_Icon* list_item;
		ecore_list_goto_first(ib->icon_box_parent->ewl_iconbox_icon_list);
		while((list_item = (Ewl_IconBox_Icon*)ecore_list_next(ib->icon_box_parent->ewl_iconbox_icon_list)) != NULL) {
			ewl_iconbox_icon_deselect(list_item);	
		}
	}
	
	if (sel && loc == 1) {
		int w,h;
		int iw,ih;
		int x,y;
		char* text;

		/* TODO request an ewl_floater_position_get function */
		x = EWL_FLOATER(ib->floater)->x;
		y = EWL_FLOATER(ib->floater)->y;
		
		/*printf ("Edit label event on: '%s'\n", ewl_border_text_get(EWL_BORDER(ib)));*/
		ewl_object_current_size_get(EWL_OBJECT(ib->w_label), &w, &h);
		ewl_object_current_size_get(EWL_OBJECT(ib->image), &iw, &ih);
		ewl_object_minimum_size_set(EWL_OBJECT(ib->icon_box_parent->entry), w,h);

		/* Get the current text to set this to */
		text = ewl_text_text_get(EWL_TEXT(ib->w_label));
		ewl_text_text_set(EWL_TEXT(ib->icon_box_parent->entry), text);
		free(text);
		
		ewl_widget_show(EWL_WIDGET(ib->icon_box_parent->entry_floater));
		ewl_floater_position_set(EWL_FLOATER(ib->icon_box_parent->entry_floater), x,y+ih);
		ewl_widget_layer_set(EWL_WIDGET(ib->icon_box_parent->entry_floater), 1000);
		ewl_widget_focus_send(EWL_WIDGET(ib->icon_box_parent->entry));
		//ewl_widget_hide(ib->w_label);

		/* Record which icon's label we are editing */
		ib->icon_box_parent->edit_icon = ib;
			
	} else if (!ib->selected) {

		/*printf("Selecting..\n");*/
		ewl_widget_hide(EWL_WIDGET(ib->icon_box_parent->entry_floater));
		ewl_iconbox_icon_label_set(ib, ib->label);

		ewl_text_cursor_position_set(EWL_TEXT(ib->w_label), 0);
		ewl_text_color_apply(EWL_TEXT(ib->w_label), 0, 0, 255, 255, ewl_text_length_get(EWL_TEXT(ib->w_label)));
		ewl_widget_color_set(EWL_WIDGET(ib), 216,176,88,70);
		
	}

	ib->selected = 1;

	
}

void ewl_iconbox_icon_deselect(Ewl_IconBox_Icon *ib)
{
	ib->selected = 0;


	/*If we have a compressed label, set it now*/
	if (ib->label_compressed) {
		ewl_iconbox_icon_label_set(ib, ib->label_compressed);
	}
	ewl_text_cursor_position_set(EWL_TEXT(ib->w_label), 0);
	ewl_text_color_apply(EWL_TEXT(ib->w_label), 0, 0, 0, 255,ewl_text_length_get(EWL_TEXT(ib->w_label)) );
	ewl_widget_color_set(EWL_WIDGET(ib), 255,255,255,255);

}

void ewl_iconbox_deselect_all(Ewl_IconBox* ib)
{
	
		Ewl_IconBox_Icon* list_item;
		ecore_list_goto_first(ib->ewl_iconbox_icon_list);
		while((list_item = (Ewl_IconBox_Icon*)ecore_list_next(ib->ewl_iconbox_icon_list)) != NULL) {
			ewl_iconbox_icon_deselect(list_item);	
		}
}



void ewl_iconbox_icon_remove(Ewl_IconBox* ib, Ewl_IconBox_Icon* icon) {
	Ewl_IconBox_Icon* list_item;

	if (ib->ewl_iconbox_icon_list) {
		Ecore_List* new_icon_list = ecore_list_new();
		
		ecore_list_goto_first(ib->ewl_iconbox_icon_list);
		while((list_item = (Ewl_IconBox_Icon*)ecore_list_next(ib->ewl_iconbox_icon_list)) != NULL) {
			
			if (list_item == icon) {
				ewl_floater_follow_set(EWL_FLOATER(EWL_ICONBOX_ICON(list_item)->floater), NULL);
				ewl_widget_destroy(EWL_WIDGET(list_item));		

				if (ib->drag_icon == icon) {
					ib->drag_icon = NULL;
				}
				if (ib->edit_icon == icon) {
					ib->edit_icon = NULL;
				}
				if (ib->select_icon == icon) {
					ib->select_icon = NULL;
				}
				
			} else {
				ecore_list_append(new_icon_list, list_item);
			}
		}

		ecore_list_destroy(ib->ewl_iconbox_icon_list);
		ib->ewl_iconbox_icon_list = new_icon_list;
	}


}



Ewl_IconBox_Icon* ewl_iconbox_icon_add(Ewl_IconBox* iconbox, char* name, char* icon_file)
{
	Ewl_Widget* ib;
	int sw, sh;

	ib = ewl_iconbox_icon_new();

	EWL_ICONBOX_ICON(ib)->selected = 0;
	EWL_ICONBOX_ICON(ib)->floater = ewl_floater_new();
	ewl_floater_follow_set(EWL_FLOATER( EWL_ICONBOX_ICON(ib)->floater ),
				iconbox->ewl_iconbox_pane_inner);

	/*Set the label*/
	ewl_iconbox_icon_label_setup(EWL_ICONBOX_ICON(ib), name);
	
	
	EWL_ICONBOX_ICON(ib)->icon_box_parent = iconbox; /* Set our parent */
	
	ewl_object_fill_policy_set(EWL_OBJECT(ib), EWL_FLAG_FILL_FILL);
	ewl_object_fill_policy_set(EWL_OBJECT(EWL_ICONBOX_ICON(ib)->floater), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(EWL_ICONBOX_ICON(ib)->floater), ib);


	/* Make the image */
	/*printf("MMaking image..\n");*/
	ewl_iconbox_icon_image_set(EWL_ICONBOX_ICON(ib), icon_file);
	ewl_container_child_append(EWL_CONTAINER(ib), EWL_ICONBOX_ICON(ib)->image);

	
	/* Add the floater to our container
	 */
	ewl_container_child_append(EWL_CONTAINER(iconbox->ewl_iconbox_pane_inner), EWL_WIDGET(EWL_ICONBOX_ICON(ib)->floater));
	ewl_floater_position_set(EWL_FLOATER(EWL_ICONBOX_ICON(ib)->floater), iconbox->lx, iconbox->ly);
	
	/*----------------------*/
	/*Get the icon next position*/

	ewl_object_current_size_get(EWL_OBJECT(iconbox->ewl_iconbox_scrollpane), &sw,&sh);

	if (iconbox->lx + iconbox->iw + (EWL_ICONBOX_ICON_PADDING) >= (sw - iconbox->iw)) {
		//printf("%d + %d + %d >= %d, so next line (%s)\n", 
		//iconbox->lx , iconbox->iw , (EWL_ICONBOX_ICON_PADDING*2) , sw, name);
		
		iconbox->ly += EWL_ICONBOX_ICON_PADDING + iconbox->ih;
		iconbox->lx = 0;
	} else {
		//printf("*** %d + %d + %d < %d, so stay (%s)\n", 
		//iconbox->lx , iconbox->iw , (EWL_ICONBOX_ICON_PADDING*2) , sw,name);
		
		iconbox->lx += EWL_ICONBOX_ICON_PADDING + iconbox->iw;	
	}
	/*----------------------*/


	ewl_callback_append(EWL_ICONBOX_ICON(ib)->w_label, EWL_CALLBACK_VALUE_CHANGED, ewl_iconbox_icon_floater_resize_cb, ib);


	/*Show*/
	ewl_widget_show(EWL_ICONBOX_ICON(ib)->image);
	ewl_widget_show(EWL_ICONBOX_ICON(ib)->w_label);
	ewl_widget_show(EWL_ICONBOX_ICON(ib)->floater);
	ewl_widget_show(EWL_WIDGET(ib));


	/*Calculate the correct height for the icon*/
	/*ewl_object_custom_h_set(EWL_OBJECT(EWL_ICONBOX_ICON(ib)->floater), ewl_iconbox_icon_label_height_calculate(EWL_ICONBOX_ICON(ib)));*/
	/*FIXME - at the moment, it appears we can't calculate the height 
		yet - hard set for now*/
	ewl_object_custom_h_set(EWL_OBJECT(EWL_ICONBOX_ICON(ib)->floater), 80);

	

	/* Add the callbacks for mouse */
	ewl_callback_prepend(EWL_ICONBOX_ICON(ib)->image, EWL_CALLBACK_MOUSE_DOWN, ewl_iconbox_icon_mouse_down, ib);
	ewl_callback_prepend(EWL_ICONBOX_ICON(ib)->image, EWL_CALLBACK_MOUSE_UP, ewl_iconbox_icon_mouse_up, ib);
	
	/* Add a callback to the border box label, for editing purposes... */
	ewl_callback_prepend(EWL_ICONBOX_ICON(ib)->w_label, EWL_CALLBACK_MOUSE_DOWN, ewl_iconbox_icon_label_mouse_down_cb, ib);

	ewl_callback_append(EWL_WIDGET(ib), EWL_CALLBACK_DESTROY,
	   ewl_iconbox_icon_destroy_cb, NULL);

	/* Add this icon to the icon list */
	ecore_list_append(iconbox->ewl_iconbox_icon_list, ib);

	return EWL_ICONBOX_ICON(ib);
}


void ewl_iconbox_icon_image_set(Ewl_IconBox_Icon* icon, char* filename)
{
	ewl_image_file_set(EWL_IMAGE(icon->image), filename, NULL);
	ewl_image_constrain_set(EWL_IMAGE(icon->image), 64);	

}

void ewl_iconbox_clear(Ewl_IconBox* ib)
{
	Ewl_IconBox_Icon* list_item;

	/*printf("*** Deleting all icons...\n");*/

	if (ib->ewl_iconbox_icon_list) {
		while((list_item = (Ewl_IconBox_Icon*)ecore_list_remove_first(ib->ewl_iconbox_icon_list)) != NULL) {
			/*printf("Deleting icon..\n");*/

			//ewl_widget_hide(list_item);
			//ewl_container_child_remove(EWL_CONTAINER(ib->ewl_iconbox_pane_inner), EWL_WIDGET(list_item));
			//
			ewl_floater_follow_set(EWL_FLOATER(EWL_ICONBOX_ICON(list_item)->floater), NULL);
			ewl_widget_destroy(EWL_WIDGET(list_item));		

						
		}
		/*printf("...dione\n");*/
	}

	ib->drag_icon = NULL;
	ib->lx = 0;
	ib->ly = 0;

	ewl_iconbox_scrollpane_recalculate(ib);
}


/*Callbacks*/

void ewl_iconbox_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__, void *user_data __UNUSED__)
{
	Ewl_IconBox* ib = EWL_ICONBOX(w);

	ecore_list_destroy(ib->ewl_iconbox_icon_list);
	return;
}

void ewl_iconbox_icon_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	
	Ewl_IconBox_Icon* icon = EWL_ICONBOX_ICON(w);
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	if (icon->label) {
		free(icon->label);
	}
	if (icon->label_compressed) {
		free(icon->label_compressed);
	}

	ewl_widget_destroy(EWL_ICONBOX_ICON(w)->floater);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* ----------- */
void ewl_iconbox_arrange_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, void *user_data)
{
	Ewl_IconBox* ib = EWL_ICONBOX(user_data);
	ewl_iconbox_icon_arrange(ib);
}

void ewl_iconbox_expansion_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, void *user_data)
{
	Ewl_IconBox* ib = EWL_ICONBOX(user_data);

	ewl_object_custom_size_set(EWL_OBJECT(ib->ewl_iconbox_pane_inner), 680,700);
}

void ewl_iconbox_mouse_move_cb(Ewl_Widget *w __UNUSED__, void *ev_data, void *user_data)
{
	Ewl_IconBox* ib = EWL_ICONBOX(user_data);
	Ewl_Event_Mouse_Move *ev = ev_data;
	Ewl_IconBox_Icon* list_item = ib->drag_icon;
	int ibx, iby;
	int px,py;

	ibx = ewl_object_current_x_get(EWL_OBJECT(ib));
	iby = ewl_object_current_y_get(EWL_OBJECT(ib));

	px = ewl_object_current_x_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));
	py = ewl_object_current_y_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));	

	/*Handle selection box*/
	if (ib->drag_box) {
		
		
		
		if (ib->dx == -1) {
			/*  Assume this is the drag start point */
			ib->dx =  ewl_object_current_x_get(EWL_OBJECT(ib->select_floater));
			ib->dy = ewl_object_current_y_get(EWL_OBJECT(ib->select_floater));
		}

		int lx,ly,hx,hy;

		/*Work out where to start/end..*/
		if (ib->dx < ev->x) {
			lx = ib->dx; hx = ev->x;
		} else {
			lx = ev->x; hx = ib->dx; 
		}
		
		if (ib->dy < ev->y) {
			ly = ib->dy ; hy = ev->y;
		} else {
			ly = ev->y; hy = ib->dy;
		}

		
			ewl_floater_position_set(EWL_FLOATER(ib->select_floater), (lx - ibx) + abs(px-ibx) , (ly+1 - iby) + abs(py-iby));
			
			if (hx-lx >0 && hy-ly > 0) {
				ewl_object_custom_size_set(EWL_OBJECT(ib->select), (hx-lx), (hy-ly));
				ewl_object_custom_size_set(EWL_OBJECT(ib->select_floater), hx-lx, hy-ly);
			}

		/*Now check which icons we have to select in this range...*/
		{
			int w,h;
			
			Ewl_IconBox_Icon* list_item;
			int ix,iy;
			ecore_list_goto_first(ib->ewl_iconbox_icon_list);

		
			w = ewl_object_current_w_get(EWL_OBJECT(ib->select));
			h = ewl_object_current_h_get(EWL_OBJECT(ib->select));
			while((list_item = (Ewl_IconBox_Icon*)ecore_list_next(ib->ewl_iconbox_icon_list)) ) {

				
						
				
				ix = ewl_object_current_x_get(EWL_OBJECT(list_item));
				iy = ewl_object_current_y_get(EWL_OBJECT(list_item));
				
				if (ix >= lx && iy >= ly && ix <= hx && iy <= hy && VISIBLE(EWL_WIDGET(list_item)) && !OBSCURED(EWL_WIDGET(list_item))) {
					ewl_iconbox_icon_select(EWL_ICONBOX_ICON(list_item),0,0);
				} else if (list_item->selected && !(ev->modifiers == EWL_KEY_MODIFIER_CTRL)) {
					ewl_iconbox_icon_deselect(EWL_ICONBOX_ICON(list_item));
				}
			}
		}

		/* If we're drag boxing, we can't be moving an icon, so return.. */
		return;
	}

	/*Check move tolerance, if not past, then don't move..*/
	if (abs(ev->x - ib->xdown) < EWL_ICONBOX_MOVE_TOLERANCE && abs(ev->y - ib->ydown) < EWL_ICONBOX_MOVE_TOLERANCE) 
		return;
	
	
	if (list_item != NULL) {			
			/*Ewl_Widget* icon_box = (Ewl_Widget*)list_item->box;*/
			int fw,fh;

			/*Get the value of the scrollpane..
			 */


			/* Get the current width/height to centre
			 */
			
			fw= ewl_object_preferred_w_get(EWL_OBJECT(list_item->image));
			fh= ewl_object_preferred_h_get(EWL_OBJECT(list_item->image));


			

			
			ewl_floater_position_set(EWL_FLOATER(list_item->floater), (ev->x - ibx) + abs(px-ibx) - (fw/2),
										  (ev->y - iby) + abs(py-iby) - (fh/2));
		}
}


void ewl_iconbox_pane_mouse_down_cb(Ewl_Widget *w __UNUSED__, void *ev_data, void *user_data)
{
	Ewl_IconBox* ib = EWL_ICONBOX(user_data);
	
	Ewl_Event_Mouse_Down *ev = ev_data;
	int ibx, iby;
	int px,py;
	
	px = ewl_object_current_x_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));
	py = ewl_object_current_y_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));	
	

	ibx = ewl_object_current_x_get(EWL_OBJECT(ib));
	iby = ewl_object_current_y_get(EWL_OBJECT(ib));
	
	if (ev->button == 3 /* Confirm that this is not an icon event */ && (ib->xdown != ev->x && ib->ydown != ev->y)) {
		/*printf ("Context menu: %d,%d\n", ev->x, ev->y);*/

		ewl_floater_position_set(EWL_FLOATER(ib->ewl_iconbox_menu_floater), ev->x-ibx + abs(px-ibx), ev->y-iby +abs(py-iby));
		//ewl_widget_show(ib->ewl_iconbox_view_menu);
		//ewl_widget_show(ib->ewl_iconbox_context_menu);
		//ewl_menu_popup_move_cb(EWL_MENU(ib->ewl_iconbox_context_menu)->base.popup, NULL, ib->ewl_iconbox_context_menu);
		ewl_callback_call(EWL_WIDGET(ib->ewl_iconbox_context_menu), EWL_CALLBACK_SELECT);
	} else if (ev->button == 1 /* Confirm that this is not an icon event */ && (ib->xdown != ev->x && ib->ydown != ev->y)) {
		ewl_object_custom_size_set(EWL_OBJECT(ib->select), 1, 1);
		
		/* Put the floater at the position we started at */
		ewl_floater_position_set(EWL_FLOATER(ib->select_floater), ev->x-ibx + abs(px-ibx), ev->y-iby + abs(py-iby));

		
		ewl_widget_show(EWL_WIDGET(ib->select_floater));
		ib->drag_box = 1;
	}
	
}



void ewl_iconbox_icon_mouse_down(Ewl_Widget *w __UNUSED__, void *ev_data, void *user_data)
{
	int ibx,iby,px,py, sx,sy;
	Ewl_IconBox_Icon* ib = user_data;
	Ewl_Event_Mouse_Down *ev = ev_data;

	ib->icon_box_parent->xdown = ev->x;
	ib->icon_box_parent->ydown = ev->y;

	/* Set this to selected */
	if (ev->modifiers == EWL_KEY_MODIFIER_CTRL) {
		ewl_iconbox_icon_select(ib,0,0);
	} else {
		ewl_iconbox_icon_select(ib,0,1);	
	}

	ib->icon_box_parent->select_icon = ib; /*We rely on this being the first callback - so
						 client*/

	px = ewl_object_current_x_get(EWL_OBJECT(ib));
	py = ewl_object_current_y_get(EWL_OBJECT(ib));

	sx = ewl_object_current_x_get(EWL_OBJECT(ib->icon_box_parent->ewl_iconbox_pane_inner));
	sy = ewl_object_current_y_get(EWL_OBJECT(ib->icon_box_parent->ewl_iconbox_pane_inner));	
	
	ibx = ewl_object_current_x_get(EWL_OBJECT(ib->icon_box_parent));
	iby = ewl_object_current_y_get(EWL_OBJECT(ib->icon_box_parent));

	if (ev->button == 3) {
		ewl_floater_position_set(EWL_FLOATER(ib->icon_box_parent->icon_menu_floater), ev->x-ibx + abs(sx-ibx), ev->y-iby +abs(sy-iby));
		//ewl_floater_follow_set(EWL_FLOATER(ib->icon_box_parent->icon_menu_floater), ib);
		//ewl_widget_show(ib->icon_box_parent->icon_menu_floater);
		//ewl_widget_show(ib->icon_box_parent->icon_menu);
		ewl_callback_call(EWL_WIDGET(ib->icon_box_parent->icon_menu), EWL_CALLBACK_SELECT);	
	} else {
		/*Select/drag start*/

		/*ewl_widget_hide(ib->icon_box_parent->icon_menu);*/
		//ewl_widget_hide(ib->icon_box_parent->icon_menu_floater);


	
		/*printf ("Button down on icon: %s\n", ewl_border_text_get(EWL_BORDER(ib)));*/
		ib->drag = 1;
		ib->icon_box_parent->drag_icon = ib;
	}

	/*ewl_callback_call_with_event_data(EWL_WIDGET(ib), EWL_CALLBACK_MOUSE_DOWN, ev_data);*/
}

void ewl_iconbox_icon_mouse_up(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, void *user_data)
{
	/*Ewl_Event_Mouse_Down *ev = ev_data;*/

	Ewl_IconBox_Icon* ib = user_data;
	ib->drag = 0;
	ib->icon_box_parent->drag_icon = NULL;
	/*printf ("Button up on icon: %s\n", ewl_border_text_get(EWL_BORDER(ib)) );*/
}

void ewl_iconbox_mouse_up(Ewl_Widget *w __UNUSED__, void *ev_data, void *user_data)
{
	Ewl_Event_Mouse_Up *ev = ev_data;
	Ewl_IconBox* ib = user_data;
	if (ev->button == 1) {
		/*printf("Stopped select: %d:%d\n", ev->x, ev->y);*/
		ewl_widget_hide(EWL_WIDGET(ib->select_floater));
		ib->drag_box = 0;
		ib->dx = -1;
		ib->dy = -1;
	}
}

void ewl_iconbox_icon_label_mouse_down_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, void *user_data)
{
	Ewl_IconBox_Icon* ib = user_data;

	/* Set this to selected */
	ewl_iconbox_icon_select(ib,1, 1);
}



/**
 * @param ib: The iconbox
 * @return Returns no value
 * @brief	Initialize the icon box
 */
void ewl_iconbox_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__, void *user_data __UNUSED__)
{
	/*printf ("Got a configure\n");*/

	Ewl_IconBox* ib = EWL_ICONBOX(w);

/*	xx = CURRENT_X(ib->ewl_iconbox_pane_inner);
	yy = CURRENT_Y(ib->ewl_iconbox_pane_inner);
	ww = CURRENT_W(ib->ewl_iconbox_pane_inner);
	hh = CURRENT_H(ib->ewl_iconbox_pane_inner);

	

		evas_damage_rectangle_add(evas_object_evas_get(EWL_CONTAINER(ib->ewl_iconbox_pane_inner)->clip_box),
							xx, yy, ww, hh);*/

	

	if (REALIZED(ib) && VISIBLE(ib)) { 
		ewl_callback_del(EWL_WIDGET(ib), EWL_CALLBACK_CONFIGURE, ewl_iconbox_configure_cb);
		ewl_iconbox_inner_pane_calculate(EWL_ICONBOX(w));
		ewl_iconbox_icon_arrange(ib); 

		if (ib->background) {
			int w,h;
			w = CURRENT_W(ib->ewl_iconbox_pane_inner);
			h= CURRENT_H(ib->ewl_iconbox_pane_inner);
			ewl_object_custom_size_set(EWL_OBJECT(ib->background),w,h);

		}
		
		ewl_callback_append(EWL_WIDGET(ib), EWL_CALLBACK_CONFIGURE, ewl_iconbox_configure_cb, NULL);

	}
}

