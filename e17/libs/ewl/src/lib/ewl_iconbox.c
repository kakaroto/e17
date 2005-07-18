#include <Ewl.h>
#include "ewl_debug.h"
#include <Evas.h>
#include "ewl_macros.h"
#include <stdlib.h>


int mouse_state = 0;
static int nextx=0;



/*Ecore_List *ewl_iconbox_icon_list;*/

/**
 * @return Returns NULL on failure, a new Ewl_IconBox on success
 * @brief Creates a new Ewl_IconBox
 */
Ewl_Widget *ewl_iconbox_new() {
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

Ewl_Widget *ewl_iconbox_icon_new() {
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


static void ewl_iconbox_mouse_move_cb(Ewl_Widget *w, void *ev_data, void *user_data);
static void ewl_iconbox_pane_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @param ib: The iconbox
 * @return Returns no value
 * @brief	Initialize the icon box
 */


void configure (Ewl_Widget *w, void *ev_data, void *user_data) {
	/*printf ("Got a configure\n");*/

	ewl_iconbox_inner_pane_calculate(EWL_ICONBOX(w));

	
}

void ewl_iconbox_inner_pane_calculate(Ewl_IconBox* ib) {

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
		ewl_object_custom_size_set(EWL_OBJECT(ib->ewl_iconbox_pane_inner), nw, nh);
		/*printf ("Grew iconbox to: %d:%d\n", nw, nh);*/
	}

	if (REALIZED(ib) && VISIBLE(ib)) { 
		/*ewl_callback_del(EWL_WIDGET(ib), EWL_CALLBACK_CONFIGURE, configure);*/
		/*ewl_iconbox_icon_arrange(ib); */
		/*ewl_callback_append(EWL_WIDGET(ib), EWL_CALLBACK_CONFIGURE, configure, NULL);*/
	}
}

int ewl_iconbox_icon_init(Ewl_IconBox_Icon* icon) {
	Ewl_Widget *w;

	w = EWL_WIDGET(icon);
	if (!ewl_border_init(EWL_BORDER(icon), "Test")) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	/* Init ewl setup */
	ewl_widget_appearance_set(EWL_WIDGET(icon), "icon");
	ewl_widget_inherit(EWL_WIDGET(w), "icon");

	DRETURN_INT(TRUE, DLEVEL_STABLE);

	
}

int ewl_iconbox_init(Ewl_IconBox* ib) {
	Ewl_Widget *w;

	w = EWL_WIDGET(ib);
	
	if (!ewl_box_init(EWL_BOX(ib), EWL_ORIENTATION_HORIZONTAL))
			DRETURN_INT(FALSE, DLEVEL_STABLE);

	/* Init ewl setup */
	ewl_widget_appearance_set(EWL_WIDGET(ib), "iconbox");
	ewl_widget_inherit(EWL_WIDGET(w), "iconbox");


	/*ib->test = "Hello!";*/

	ib->drag_icon = NULL;
	
	/*printf("Ewl_IconBox: Entering init..\n");*/
	/* -------------------- */

	/*Default to non-editable labels */
	ewl_iconbox_editable_set(ib,0);
	
	
	ib->ewl_iconbox_scrollpane = ewl_scrollpane_new();
	if (ib->ewl_iconbox_scrollpane) {
		ewl_scrollpane_hscrollbar_flag_set(EWL_SCROLLPANE(ib->ewl_iconbox_scrollpane), EWL_SCROLLPANE_FLAG_AUTO_VISIBLE);
	} else {
		/*printf ("Bombed out on scrollpane creation\n");*/
	}
	
	ib->ewl_iconbox_pane_inner = ewl_overlay_new();
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_scrollpane), ib->ewl_iconbox_pane_inner);
		
	/*** Context menu **/
	/*Make the menu floater */
	ib->ewl_iconbox_menu_floater = ewl_floater_new(ib->ewl_iconbox_pane_inner);
	ib->ewl_iconbox_menu_box = ewl_vbox_new();

	ewl_object_fill_policy_set(EWL_OBJECT(ib->ewl_iconbox_menu_box), EWL_FLAG_FILL_FILL);
	ewl_object_fill_policy_set(EWL_OBJECT(ib->ewl_iconbox_menu_floater), EWL_FLAG_FILL_FILL);

	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_menu_box), ib->ewl_iconbox_menu_floater);

	/* Get the context menu ready */
	ib->ewl_iconbox_context_menu = ewl_menu_new(NULL, "");
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_menu_floater), ib->ewl_iconbox_context_menu);
	ewl_widget_show(ib->ewl_iconbox_context_menu);

	/* Add auto-arrange ability */
	ib->ewl_iconbox_view_menu = ewl_menu_new(NULL, "View");
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_context_menu), ib->ewl_iconbox_view_menu);
	
	ib->ewl_iconbox_context_menu_item = ewl_menu_item_new(NULL, "Auto-Arrange");
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_view_menu), ib->ewl_iconbox_context_menu_item);
	ewl_callback_append(ib->ewl_iconbox_context_menu_item, EWL_CALLBACK_MOUSE_DOWN, ewl_iconbox_arrange_cb, ib);
	ewl_widget_show(ib->ewl_iconbox_context_menu_item);

	ib->ewl_iconbox_context_menu_item = ewl_menu_item_new(NULL, "Expansion Test");
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_view_menu), ib->ewl_iconbox_context_menu_item);
	ewl_callback_append(ib->ewl_iconbox_context_menu_item, EWL_CALLBACK_MOUSE_DOWN, ewl_iconbox_expansion_cb, ib);
	ewl_widget_show(ib->ewl_iconbox_context_menu_item);


	ewl_widget_show(ib->ewl_iconbox_view_menu);
		
	/* Add the menu floater to the pane inner */
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_pane_inner), ib->ewl_iconbox_menu_floater);

	/* ------------------------------ Menu */


	/* Fix these to be more generic */
	/*ewl_object_preferred_inner_size_set(EWL_OBJECT(ib->ewl_iconbox_pane_inner), 680,300);
	ewl_object_maximum_size_set(EWL_OBJECT(ib->ewl_iconbox_scrollpane), 400,400);*/

	/* Add the iconbox scrollpane to the iconbox */
	ewl_container_child_append(EWL_CONTAINER(ib), ib->ewl_iconbox_scrollpane);
	ewl_object_fill_policy_set(EWL_OBJECT(ib), EWL_FLAG_FILL_ALL);

	/* Create the selector / selector floater */
	ib->select_floater = ewl_floater_new(ib->ewl_iconbox_pane_inner);
	ewl_object_fill_policy_set(EWL_OBJECT(ib->select_floater), EWL_FLAG_FILL_FILL);
	ib->select =ewl_button_new("");
	
	ewl_container_child_append(EWL_CONTAINER(ib->select_floater), ib->select);
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_pane_inner), ib->select_floater);
	
	ewl_object_custom_size_set(EWL_OBJECT(ib->select), 80, 40);
	ewl_widget_layer_set(EWL_WIDGET(ib->select_floater), 1000);
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
	ewl_widget_show(ib->ewl_iconbox_menu_box);


	/* Ewl Entry for the purposes of label editing - if enabled */
	ib->entry = ewl_entry_new("Test");
	ewl_text_text_set(EWL_TEXT(ib->entry), "Test");
	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(EWL_ENTRY(ib->entry)->cursor), 50);
	ewl_widget_show(ib->entry);
	ib->entry_floater = ewl_floater_new(ib->ewl_iconbox_pane_inner);
	/*ewl_widget_show(ib->entry_floater);*/
	ib->entry_box = ewl_box_new(EWL_ORIENTATION_HORIZONTAL);
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
	ewl_callback_append(EWL_WIDGET(ib), EWL_CALLBACK_CONFIGURE, configure, NULL);



	/*printf("Setup the iconbox...\n");*/
	DRETURN_INT(TRUE, DLEVEL_STABLE);

}


/* ----------- */
void ewl_iconbox_arrange_cb(Ewl_Widget *w, void *ev_data, void *user_data) {
	Ewl_IconBox* ib = EWL_ICONBOX(user_data);
	ewl_iconbox_icon_arrange(ib);
}

void ewl_iconbox_expansion_cb(Ewl_Widget *w, void *ev_data, void *user_data) {
	Ewl_IconBox* ib = EWL_ICONBOX(user_data);

	ewl_object_custom_size_set(EWL_OBJECT(ib->ewl_iconbox_pane_inner), 680,700);
}



void ewl_iconbox_label_edit_key_down(Ewl_Widget *w, void *ev_data, void* user_data) {
	Ewl_Event_Key_Down* ev = ev_data;
	Ewl_IconBox* ib = EWL_ICONBOX(user_data);
	
	if (!strcmp(ev->keyname, "Return")) {
		ewl_border_text_set(EWL_BORDER(ib->edit_icon), ewl_text_text_get(EWL_TEXT(ib->entry)));
		/*printf ("Setting label to: '%s'", ewl_text_text_get(EWL_TEXT(ib->entry)));*/

		/*Hide the entry, now that we have an enter */
		ewl_widget_hide(ib->entry_floater);
	}
}

void ewl_iconbox_editable_set(Ewl_IconBox* ib, int edit) {
	ib->editable = edit;
}

void ewl_iconbox_icon_arrange(Ewl_IconBox* ib) {
	int sw=0,sh=0;
	int iw=0, ih=0;
	int nextx=0, nexty=0;
	Ewl_IconBox_Icon* list_item;
	int maxx=0, maxy=0;
	int pw, ph;

	/*printf ("Entering the arrange..\n");*/

	/* Recalc inner pane size */
	/*ewl_iconbox_inner_pane_calculate(ib);*/
	
	/*printf ("Ewl_IconBox -> Arranging icons\n");*/

	
	
	ewl_object_current_size_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner), &sw,&sh);
	/*printf("   Ewl_IconBox -> We have %d*%d to work with\n", sw,sh);*/

	ecore_list_goto_first(ib->ewl_iconbox_icon_list);
	
	while((list_item = (Ewl_IconBox_Icon*)ecore_list_next(ib->ewl_iconbox_icon_list)) != NULL) {
		if (iw == 0 || ih == 0) {
			iw= ewl_object_preferred_w_get(EWL_OBJECT(list_item));
			ih= ewl_object_preferred_h_get(EWL_OBJECT(list_item));
		}

		if (iw < EWL_ICONBOX_MOVE_TOLERANCE) {
			iw = EWL_ICONBOX_MINIMUM_SIZE;
			ih = EWL_ICONBOX_MINIMUM_SIZE;
		}

		
		if (nextx + iw > sw) {
			nextx = 0;
			nexty += ih + EWL_ICONBOX_ICON_PADDING;
		}
		ewl_floater_position_set(EWL_FLOATER(list_item->floater), nextx, nexty);
		nextx += iw + EWL_ICONBOX_ICON_PADDING;
		
		if (nextx > maxx) maxx = nextx;
		if (nexty > maxy) maxy = nexty;

		
	}

	/* Now set the extent of the pane inner to be the maxx/y that we had or 
	 * the size of the scrollpane, which ever is bigger
	 */
	ewl_object_current_size_get(EWL_OBJECT(ib->ewl_iconbox_scrollpane), &pw, &ph);	
	ewl_object_preferred_inner_size_set(EWL_OBJECT(ib->ewl_iconbox_pane_inner), pw > maxx ? pw : maxx, ph > maxy+ih ? ph : maxy+ih);



}
/* ------------ */



void ewl_iconbox_mouse_move_cb(Ewl_Widget *w, void *ev_data, void *user_data)
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
			Ewl_IconBox_Icon* list_item;
			int ix,iy;
			ecore_list_goto_first(ib->ewl_iconbox_icon_list);

			
			while((list_item = (Ewl_IconBox_Icon*)ecore_list_next(ib->ewl_iconbox_icon_list)) != NULL) {
				int w,h;

				w = ewl_object_current_w_get(EWL_OBJECT(ib->select));
				h = ewl_object_current_h_get(EWL_OBJECT(ib->select));
						
				
				ix = ewl_object_current_x_get(EWL_OBJECT(list_item));
				iy = ewl_object_current_y_get(EWL_OBJECT(list_item));
				
				if (ix >= lx && iy >= ly && ix <= hx && iy <= hy ) {
					ewl_iconbox_icon_select(EWL_ICONBOX_ICON(list_item),0);
				} else {
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
			
			fw= ewl_object_preferred_w_get(EWL_OBJECT(list_item));
			fh= ewl_object_preferred_h_get(EWL_OBJECT(list_item));


			

			
			ewl_floater_position_set(EWL_FLOATER(list_item->floater), (ev->x - ibx) + abs(px-ibx) - (fw/2),
										  (ev->y - iby) + abs(py-iby) - (fh/2));
		}
}


void ewl_iconbox_pane_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data) {
	Ewl_IconBox* ib = EWL_ICONBOX(user_data);
	
	Ewl_Event_Mouse_Down *ev = ev_data;
	int ibx, iby;
	int px,py;
	
	px = ewl_object_current_x_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));
	py = ewl_object_current_y_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));	
	

	ibx = ewl_object_current_x_get(EWL_OBJECT(ib));
	iby = ewl_object_current_y_get(EWL_OBJECT(ib));
	
	if (ev->button == 3) {
		/*printf ("Context menu: %d,%d\n", ev->x, ev->y);*/

		ewl_widget_show(ib->ewl_iconbox_menu_floater);
		ewl_floater_position_set(EWL_FLOATER(ib->ewl_iconbox_menu_floater), ev->x-ibx + abs(px-ibx), ev->y-iby +abs(py-iby));
		ewl_callback_call(EWL_WIDGET(ib->ewl_iconbox_context_menu), EWL_CALLBACK_SELECT);
	} else if (ev->button == 1 /* Confirm that this is not an icon event */ && (ib->xdown != ev->x && ib->ydown != ev->y)) {
		/*printf ("Start select at %d:%d\n", ev->x, ev->y);*/
		ewl_object_custom_size_set(EWL_OBJECT(ib->select), 1, 1);
		
		/* Put the floater at the position we started at */
		ewl_floater_position_set(EWL_FLOATER(ib->select_floater), ev->x-ibx + abs(px-ibx), ev->y-iby + abs(py-iby));

		
		ewl_widget_show(EWL_WIDGET(ib->select_floater));
		ib->drag_box = 1;
	}
	
}



void ewl_iconbox_icon_mouse_down(Ewl_Widget *w, void *ev_data, void *user_data) {

	Ewl_IconBox_Icon* ib = user_data;
	Ewl_Event_Mouse_Down *ev = ev_data;
	
	/*printf ("Button down on icon: %s\n", ewl_border_text_get(EWL_BORDER(ib)));*/
	ib->drag = 1;
	ib->icon_box_parent->drag_icon = ib;
	ib->icon_box_parent->xdown = ev->x;
	ib->icon_box_parent->ydown = ev->y;

	/* Set this to selected */
	ewl_iconbox_icon_select(ib,0);

	/*ewl_callback_call_with_event_data(EWL_WIDGET(ib), EWL_CALLBACK_MOUSE_DOWN, ev_data);*/
}

void ewl_iconbox_icon_mouse_up(Ewl_Widget *w, void *ev_data, void *user_data) {
	/*Ewl_Event_Mouse_Down *ev = ev_data;*/

	Ewl_IconBox_Icon* ib = user_data;
	ib->drag = 0;
	ib->icon_box_parent->drag_icon = NULL;
	/*printf ("Button up on icon: %s\n", ewl_border_text_get(EWL_BORDER(ib)) );*/
}

void ewl_iconbox_mouse_up(Ewl_Widget *w, void *ev_data, void *user_data) {
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

void ewl_iconbox_icon_label_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data) {
	Ewl_IconBox_Icon* ib = user_data;

	/* Set this to selected */
	ewl_iconbox_icon_select(ib,1);
}

void ewl_iconbox_icon_select(Ewl_IconBox_Icon* ib, int loc) { /* Loc 0= image, 1= label */
	
	if (ib->selected == 1 && loc == 1) {
		int w,h;
		int iw,ih;
		int x,y;

		/* TODO request an ewl_floater_position_get function */
		x = EWL_FLOATER(ib->floater)->x;
		y = EWL_FLOATER(ib->floater)->y;
		
		/*printf ("Edit label event on: '%s'\n", ewl_border_text_get(EWL_BORDER(ib)));*/
		ewl_object_current_size_get(EWL_OBJECT(EWL_BORDER(ib)->label), &w, &h);
		ewl_object_current_size_get(EWL_OBJECT(ib->image), &iw, &ih);
		ewl_object_minimum_size_set(EWL_OBJECT(ib->icon_box_parent->entry), w,h);

		/* Get the current text to set this to */
		ewl_text_text_set(EWL_TEXT(ib->icon_box_parent->entry), ewl_border_text_get(EWL_BORDER(ib)));
		
		ewl_widget_show(EWL_WIDGET(ib->icon_box_parent->entry_floater));
		ewl_floater_position_set(EWL_FLOATER(ib->icon_box_parent->entry_floater), x,y+ih);
		ewl_widget_layer_set(EWL_WIDGET(ib->icon_box_parent->entry_floater), 1000);
		ewl_widget_focus_send(EWL_WIDGET(ib->icon_box_parent->entry));

		/* Record which icon's label we are editing */
		ib->icon_box_parent->edit_icon = ib;
			
	} else {
		ewl_widget_hide(EWL_WIDGET(ib->icon_box_parent->entry_floater));
	}

	
	/*TODO allow multiselect, as per a "select policy" set on widget create/init*/
	
	if (!ib->icon_box_parent->drag_box) {
		Ewl_IconBox_Icon* list_item;
		ecore_list_goto_first(ib->icon_box_parent->ewl_iconbox_icon_list);
		while((list_item = (Ewl_IconBox_Icon*)ecore_list_next(ib->icon_box_parent->ewl_iconbox_icon_list)) != NULL) {
			ewl_iconbox_icon_deselect(list_item);	
		}
	}
	
	ib->selected = 1;

	/*printf("Setting color..\n");*/
	ewl_widget_color_set(EWL_WIDGET(EWL_BORDER(ib)->label), 140,0,255,128);
	ewl_callback_call(EWL_WIDGET(EWL_BORDER(ib)->label), EWL_CALLBACK_APPEARANCE_CHANGED);
}

void ewl_iconbox_icon_deselect(Ewl_IconBox_Icon *ib) {
	ib->selected = 0;
	ewl_widget_color_set(EWL_WIDGET(EWL_BORDER(ib)->label), 0,0,0,255);
}

void ewl_iconbox_deselect_all(Ewl_IconBox* ib) {
		return;
	
		Ewl_IconBox_Icon* list_item;
		ecore_list_goto_first(ib->ewl_iconbox_icon_list);
		while((list_item = (Ewl_IconBox_Icon*)ecore_list_next(ib->ewl_iconbox_icon_list)) != NULL) {
			ewl_iconbox_icon_deselect(list_item);	
		}
}



void ewl_iconbox_icon_remove(Ewl_IconBox_Icon* icon) {
	/*printf("Removing icon: %s", ewl_border_text_get(EWL_BORDER(icon)));*/
}

Ewl_IconBox_Icon* ewl_iconbox_icon_add(Ewl_IconBox* iconbox, char* name, char* icon_file) {
	Ewl_Widget* ib;
	/*ib = malloc(sizeof(Ewl_IconBox_Icon));*/
	/*printf ("Making an icon called '%s'\n", name);*/

	ib = ewl_iconbox_icon_new();

	EWL_ICONBOX_ICON(ib)->selected = 0;

	EWL_ICONBOX_ICON(ib)->floater = ewl_floater_new(iconbox->ewl_iconbox_pane_inner);
	ewl_border_text_set(EWL_BORDER(ib), name);
	EWL_ICONBOX_ICON(ib)->icon_box_parent = iconbox; /* Set our parent */
	
	/*printf("Setting fill policy..\n");*/
	ewl_object_fill_policy_set(EWL_OBJECT(ib), EWL_FLAG_FILL_SHRINK);
	ewl_object_fill_policy_set(EWL_OBJECT(EWL_ICONBOX_ICON(ib)->floater), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(EWL_ICONBOX_ICON(ib)->floater), ib);


	/* Make the image */
	/*printf("MMaking image..\n");*/

	EWL_ICONBOX_ICON(ib)->image = ewl_image_new(icon_file, NULL);

	/* Put the image into the border */
	/*printf("Adding image to box..\n");*/
	ewl_container_child_append(EWL_CONTAINER(ib), EWL_ICONBOX_ICON(ib)->image);

	ewl_border_label_position_set(EWL_BORDER(ib), EWL_FLAG_ALIGN_BOTTOM);
	
	/* Add the floater to our container
	 */
	ewl_container_child_append(EWL_CONTAINER(iconbox->ewl_iconbox_pane_inner), EWL_WIDGET(EWL_ICONBOX_ICON(ib)->floater));

	/* Find the enxt pos for this icon FIXME add this to layout engine */
	ewl_floater_position_set(EWL_FLOATER(EWL_ICONBOX_ICON(ib)->floater), nextx, 0);
	nextx += 60;


	/*Show*/
	ewl_widget_show(EWL_ICONBOX_ICON(ib)->image);
	ewl_widget_show(EWL_ICONBOX_ICON(ib)->floater);
	ewl_widget_show(EWL_WIDGET(ib));

	

	/* Add the callbacks for mouse */
	ewl_callback_prepend(EWL_ICONBOX_ICON(ib)->image, EWL_CALLBACK_MOUSE_DOWN, ewl_iconbox_icon_mouse_down, ib);
	ewl_callback_prepend(EWL_ICONBOX_ICON(ib)->image, EWL_CALLBACK_MOUSE_UP, ewl_iconbox_icon_mouse_up, ib);
	
	/* Add a callback to the border box label, for editing purposes... */
	ewl_callback_prepend(EWL_BORDER(ib)->label, EWL_CALLBACK_MOUSE_DOWN, ewl_iconbox_icon_label_mouse_down_cb, ib);

	/* Add this icon to the icon list */
	ecore_list_append(iconbox->ewl_iconbox_icon_list, ib);

	/*BUG FIX - Call the realize event so that selection works */
	/*ewl_callback_call(EWL_WIDGET(EWL_BORDER(ib->box)->label), EWL_CALLBACK_REALIZE);*/

	/*printf("Added icon: '%s'\n", name);*/

	ewl_object_alignment_set(EWL_OBJECT(ib), EWL_FLAG_ALIGN_CENTER);
	ewl_widget_layer_set(EWL_WIDGET(ib), 500);
	
	return EWL_ICONBOX_ICON(ib);
}


void ewl_iconbox_icon_image_set(Ewl_IconBox_Icon* icon, char* filename) {
	int iw,ih,lw,lh;
	
	ewl_image_file_set(EWL_IMAGE(icon->image), filename, NULL);

	/*Make sure we're not too small now */
	/*Shouldn't the floater auto-resize?*/
	
	ewl_object_current_size_get(EWL_OBJECT(icon->image), &iw,&ih);
	ewl_object_current_size_get(EWL_BORDER(icon)->label, &lw, &lh); /* Shouldn't access this directly, is there another way? */
	ewl_object_minimum_size_set(EWL_OBJECT(icon->floater), iw, ih+lh);
}
