#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

#define ICON_LABEL_INITIAL 80
#define ICONBOX_SELECT_LAYER 600
#define ICONBOX_ICON_LAYER 500
#define ICONBOX_BACKGROUND_LAYER 500
#define ICONBOX_ENTRY_LAYER 1000
#define ICONBOX_REPEAT 10000

static void ewl_iconbox_icon_select(Ewl_Iconbox_Icon *ib, int loc, int deselect);
static void ewl_iconbox_icon_deselect(Ewl_Iconbox_Icon *ib);
static void ewl_iconbox_label_edit_key_down(Ewl_Widget *w, void *ev_data, void *user_data);
static void ewl_iconbox_inner_pane_calculate(Ewl_Iconbox *ib);
static void ewl_iconbox_icon_label_set(Ewl_Iconbox_Icon *, const char *);

void *
ewl_iconbox_drag_data_get(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	printf("Request for drag data!\n");
	
	DRETURN_PTR(NULL, DLEVEL_STABLE);
}


int
ewl_iconbox_icon_label_height_calculate(Ewl_Iconbox_Icon* icon)
{
	int height=0;
	int ww,hh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("icon", icon, FALSE);
	DCHECK_TYPE_RET("icon", icon, EWL_ICON_TYPE, FALSE);

	if (EWL_TEXT(icon->w_label)->textblock) {
		evas_object_textblock_size_native_get(EWL_TEXT(icon->w_label)->textblock, &ww, &hh);
		height = CURRENT_H(icon->image) + hh;
	}

	DRETURN_INT(height, DLEVEL_STABLE);
}

void
ewl_iconbox_overlay_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__, void *user_data)
{

	int nx, ny;
	Ewl_Iconbox *iconbox = EWL_ICONBOX(user_data);
	Ewl_Widget *ib;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("w", w, EWL_OVERLAY_TYPE);
	DCHECK_TYPE("user_data", user_data, EWL_ICONBOX_TYPE);



	nx = ewl_object_current_x_get(EWL_OBJECT(w));
	ny = ewl_object_current_y_get(EWL_OBJECT(w));

	ewl_container_child_iterate_begin(EWL_CONTAINER(w));
	while ((ib = ewl_container_child_next(EWL_CONTAINER(w)))) {
		if (ewl_widget_type_is(ib,EWL_ICON_TYPE)) {
			int cx, cy;
			cx = ewl_object_current_x_get(EWL_OBJECT(ib));
			cy = ewl_object_current_y_get(EWL_OBJECT(ib));

			ewl_object_position_request(EWL_OBJECT(ib),
					cx + (nx - iconbox->ox),
					cy + (ny - iconbox->oy));

		}

		
	}

	iconbox->ox = nx;
	iconbox->oy = ny;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_icon_floater_resize(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, 
									void *user_data)
{
	Ewl_Iconbox_Icon *icon;
	int height;
	int width;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	icon = EWL_ICONBOX_ICON(user_data);
	height = ewl_iconbox_icon_label_height_calculate(icon) + 10;
	width = CURRENT_W(icon->image) + 10;

	ewl_callback_del(w, EWL_CALLBACK_CONFIGURE, ewl_iconbox_icon_floater_resize);

	if (height >0) {  
		ewl_object_custom_size_set(EWL_OBJECT(icon), width,height); 
	}

	//printf("Resized floater to EWL_TEXT(%d)-TEXTBLOCK(%d) %d\n", CURRENT_H(icon->w_label), hh, height);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @return Returns NULL on failure, a new Ewl_Iconbox on success
 * @brief Creates a new Ewl_Iconbox
 */
Ewl_Widget *
ewl_iconbox_new(void)
{
	Ewl_Iconbox *ib;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ib = NEW(Ewl_Iconbox, 1);
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


/**
 * @return Returns a new Ewl_Iconbox_Icon, NULL on failure
 *
 */
Ewl_Widget *
ewl_iconbox_icon_new(void)
{
	Ewl_Iconbox_Icon *icon;

	DENTER_FUNCTION (DLEVEL_STABLE);

	icon = NEW(Ewl_Iconbox_Icon, 1);
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


static void
ewl_iconbox_inner_pane_calculate(Ewl_Iconbox *ib)
{
	int pw,ph;
	int sw,sh;
	int nw=0,nh=0;

	return;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);

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

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The Ewl_Iconbox_Icon to initialize
 * @return Returns  a positive value on success
 *
 */
int
ewl_iconbox_icon_init(Ewl_Iconbox_Icon *icon)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("icon", icon, FALSE);

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
	ewl_widget_appearance_set(EWL_WIDGET(icon), EWL_ICON_TYPE);
	ewl_widget_inherit(EWL_WIDGET(w), EWL_ICON_TYPE);

	ewl_container_child_append(EWL_CONTAINER(icon), icon->image);
	ewl_container_child_append(EWL_CONTAINER(icon), icon->w_label);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


/**
 * @param ib: The length of text to delete
 * @return Returns a positive value on success
 *
 * Initialize an iconbox
 */
int
ewl_iconbox_init(Ewl_Iconbox *ib)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ib", ib, FALSE);

	w = EWL_WIDGET(ib);
	
	if (!ewl_box_init(EWL_BOX(ib)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	/* Init ewl setup */
	ewl_box_orientation_set(EWL_BOX(ib), EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_appearance_set(EWL_WIDGET(ib), EWL_ICONBOX_TYPE);
	ewl_widget_inherit(EWL_WIDGET(w), EWL_ICONBOX_TYPE);

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
	ewl_callback_prepend(EWL_WIDGET(ib->ewl_iconbox_pane_inner),
			     EWL_CALLBACK_CONFIGURE,
			     ewl_iconbox_overlay_configure_cb, ib);

	/*** Context menu **/
	/*Make the menu floater */
	ib->ewl_iconbox_menu_floater = ewl_floater_new();
	ewl_floater_follow_set(EWL_FLOATER(ib->ewl_iconbox_menu_floater),
			       ib->ewl_iconbox_pane_inner);

	/*-------------------------------------------*/
	/* Get the context menu ready */
	ib->ewl_iconbox_context_menu = ewl_menu_new();
	ewl_menu_item_text_set(EWL_MENU_ITEM(ib->ewl_iconbox_context_menu), " ");
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_menu_floater), ib->ewl_iconbox_context_menu);
	ewl_widget_show(ib->ewl_iconbox_context_menu);

	/* Add auto-arrange ability */
	ib->ewl_iconbox_view_menu = ewl_menu_new();
	ewl_menu_item_text_set(EWL_MENU_ITEM(ib->ewl_iconbox_view_menu),
			       "View");
	ewl_widget_appearance_set(EWL_WIDGET(ib->ewl_iconbox_view_menu), 
							EWL_MENU_ITEM_TYPE);
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_context_menu), ib->ewl_iconbox_view_menu);
	
	ib->ewl_iconbox_context_menu_item = ewl_menu_item_new();
	ewl_menu_item_text_set(EWL_MENU_ITEM(ib->ewl_iconbox_context_menu_item),
			       "Auto-Arrange");
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_view_menu), ib->ewl_iconbox_context_menu_item);
	ewl_callback_append(ib->ewl_iconbox_context_menu_item, EWL_CALLBACK_MOUSE_DOWN, ewl_iconbox_arrange_cb, ib);
	ewl_widget_show(ib->ewl_iconbox_context_menu_item);

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
	ewl_menu_item_text_set(EWL_MENU_ITEM(ib->icon_menu), " ");
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
	
	ewl_object_custom_size_set(EWL_OBJECT(ib->select), 1, 1);
	ewl_widget_layer_set(EWL_WIDGET(ib->select_floater), ICONBOX_SELECT_LAYER);
	ewl_widget_color_set(EWL_WIDGET(ib->select), 255, 255, 25, 50);
	ib->drag_box = 0;

	/*Set the dx/dy drag start points to 'null' values*/
	ib->dx = -1; ib->dy = -1;
	/* -------------------------------- */

	ewl_widget_show(ib->select);

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

	/*Mark widgets internal*/
	ewl_widget_internal_set(EWL_WIDGET(ib->entry_floater), TRUE);
	ewl_widget_internal_set(EWL_WIDGET(ib->ewl_iconbox_menu_floater), TRUE);
	ewl_widget_internal_set(EWL_WIDGET(ib->icon_menu_floater), TRUE);
	ewl_widget_internal_set(EWL_WIDGET(ib->select_floater), TRUE);

	


	/** Internal Callbacks */
	ewl_callback_append(ib->ewl_iconbox_pane_inner, EWL_CALLBACK_MOUSE_MOVE, ewl_iconbox_mouse_move_cb, ib);
	ewl_callback_append(ib->ewl_iconbox_pane_inner, EWL_CALLBACK_MOUSE_DOWN, ewl_iconbox_pane_mouse_down_cb, ib);
	ewl_callback_append(ib->ewl_iconbox_pane_inner, EWL_CALLBACK_MOUSE_UP, ewl_iconbox_mouse_up, ib);
	ewl_callback_append(ib->ewl_iconbox_pane_inner, EWL_CALLBACK_DND_POSITION, ewl_iconbox_dnd_position_cb, ib);
	ewl_callback_append(ib->ewl_iconbox_pane_inner, EWL_CALLBACK_DND_DROP, ewl_iconbox_dnd_drop_cb, ib);
	ewl_container_callback_notify(EWL_CONTAINER(ib), EWL_CALLBACK_KEY_DOWN);
	ewl_callback_append(EWL_WIDGET(ib), EWL_CALLBACK_KEY_DOWN, ewl_iconbox_key_press_cb, ib);
	ewl_callback_append(EWL_WIDGET(ib), EWL_CALLBACK_CONFIGURE, ewl_iconbox_configure_cb, NULL);
	ewl_callback_prepend(EWL_WIDGET(ib), EWL_CALLBACK_DESTROY, ewl_iconbox_destroy_cb, NULL);

	/*Timing setup*/
	ib->lasttime.tv_sec = 0;
	ib->lasttime.tv_usec = 0;

	/*Keyboard event callback setup*/
	ib->key_event_cb = NULL;


	/*printf("Setup the iconbox...\n");*/
	DRETURN_INT(TRUE, DLEVEL_STABLE);

}


/**
 * @param ib: The IconBox to set the custom icon height for
 * @param w: The custom width
 * @param h: The custom height
 * @return Returns no value
 *
 * Set a custom image size for all icons in this iconbox
 */
void
ewl_iconbox_icon_size_custom_set(Ewl_Iconbox* ib, int w, int h) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);

	ib->iw = w;
	ib->ih = h;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ib: The Ewl_Iconbox to add a menu item to
 * @param item: The ewl_menu_item to add.
 * @return Returns no value
 *
 * Add a menu item to the iconbox background menu
 */
void
ewl_iconbox_context_menu_item_add(Ewl_Iconbox *ib, Ewl_Widget *item) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_PARAM_PTR("item", item);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);
	DCHECK_TYPE("item", item, EWL_WIDGET_TYPE);

	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_context_menu), item);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to delete the text from
 * @param length: The length of text to delete
 * @return Returns no value
 *
 * This will delete the specified length of text from the current cursor
 * position
 */
void
ewl_iconbox_icon_menu_item_add(Ewl_Iconbox *ib, Ewl_Widget *item) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_PARAM_PTR("item", item);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);
	DCHECK_TYPE("item", item, EWL_WIDGET_TYPE);

	ewl_object_minimum_size_set(EWL_OBJECT(item), 100, 15);
	ewl_container_child_append(EWL_CONTAINER(ib->icon_menu), item);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ecore_List *
ewl_iconbox_get_selection(Ewl_Iconbox *ib) 
{
	Ewl_Iconbox_Icon *list_item;
	Ecore_List *selected;
	int add_last = 1;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ib", ib, NULL);
	DCHECK_TYPE_RET("ib", ib, EWL_ICONBOX_TYPE, NULL);

	selected = ecore_list_new();	
	ecore_list_goto_first(ib->ewl_iconbox_icon_list);
	while ((list_item = ecore_list_next(ib->ewl_iconbox_icon_list))) {
		if (ib->select_icon 
				&& (list_item == ib->select_icon) 
				&& list_item->selected) 
			add_last = 0;
		if (list_item->selected) 
			ecore_list_append(selected, list_item);
	}

	if (ib->select_icon && add_last) 
		ecore_list_append(selected, ib->select_icon);

	DRETURN_PTR(selected, DLEVEL_STABLE);
}

/* Move to the root of the scrollpame */
void
ewl_iconbox_scrollpane_goto_root(Ewl_Iconbox *ib) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);

        ewl_scrollpane_hscrollbar_value_set(EWL_SCROLLPANE(ib->ewl_iconbox_scrollpane),0);
        ewl_scrollpane_vscrollbar_value_set(EWL_SCROLLPANE(ib->ewl_iconbox_scrollpane),0);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_background_set(Ewl_Iconbox *ib, const char *file)
{
	/* Add a background image */
	int w, h;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);

	if (!file) {
		if (ib->background) {
			ewl_widget_destroy(ib->background);
			ib->background = NULL;
		}
		DRETURN(DLEVEL_STABLE);
	}

	w = CURRENT_W(ib);
	h= CURRENT_H(ib);

	if (!ib->background) 
		ib->background = ewl_image_new();
	
	ewl_image_file_set(EWL_IMAGE(ib->background), file, 0);
	ewl_container_child_append(EWL_CONTAINER(ib->ewl_iconbox_pane_inner), 
							ib->background);

	if (EWL_WIDGET(ib)->parent) {
		int width, height, sw, sh;
		Ewl_Widget *parent;
	
		sw = CURRENT_W(EWL_SCROLLPANE(ib->ewl_iconbox_scrollpane)->vscrollbar);
		sh = CURRENT_H(EWL_SCROLLPANE(ib->ewl_iconbox_scrollpane)->hscrollbar);	
		parent = EWL_WIDGET(ib)->parent;

		width = CURRENT_W(ib);
		height = CURRENT_H(ib);

		ewl_object_position_request(EWL_OBJECT(ib->background),
				CURRENT_X(parent), CURRENT_Y(parent));
		ewl_object_custom_size_set(EWL_OBJECT(ib->background), width - sw,
								height - sh);
	}
	
	ewl_widget_show(ib->background);
	ewl_widget_layer_set(EWL_WIDGET(ib->background), ICONBOX_BACKGROUND_LAYER);
	
	ewl_widget_configure(EWL_WIDGET(ib));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_iconbox_icon_label_setup(Ewl_Iconbox_Icon *icon, const char *text)
{
	
	char *compressed;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("icon", icon);
	DCHECK_PARAM_PTR("text", text);
	DCHECK_TYPE("icon", icon, EWL_ICON_TYPE);
	
	/* If we have a current label, nuke it */
	if (icon->label) {
		free(icon->label);
	}

	/* Copy the existing label to our icon */
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
		strcpy(compressed + LABEL_CHARS, "..\0");
		ewl_iconbox_icon_label_set(icon, compressed);

		icon->label_compressed = compressed;
	}


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_icon_label_set(Ewl_Iconbox_Icon *icon, const char *text)
{
	int wrap = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("icon", icon);
	DCHECK_TYPE("icon", icon, EWL_ICON_TYPE);

	
	//if (REALIZED(EWL_WIDGET(icon))) { 
	//	printf("Icon '%s' realized, setting up label..\n", text);
		ewl_callback_append(EWL_WIDGET(icon->w_label), EWL_CALLBACK_CONFIGURE, 
						ewl_iconbox_icon_floater_resize, icon);
	//}
	
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

	/* Overestimate the label height to begin with, to give the text room to expand */
	if (REALIZED(EWL_WIDGET(icon))) {
		ewl_object_custom_h_set( EWL_OBJECT(icon), CURRENT_H(icon->image) + ICON_LABEL_INITIAL);	
		
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


static void
ewl_iconbox_label_edit_key_down(Ewl_Widget *w __UNUSED__, void *ev_data, 
							void *user_data)
{
	Ewl_Event_Key_Down *ev;
	Ewl_Iconbox *ib;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_PARAM_PTR("user_data", user_data);

	ev = ev_data;
	ib = EWL_ICONBOX(user_data);

	if (!strcmp(ev->keyname, "Return")) {
		char *text;
		
		text = ewl_text_text_get(EWL_TEXT(ib->entry));
		ewl_iconbox_icon_label_set(EWL_ICONBOX_ICON(ib->edit_icon), text);
		free(text);

		/*printf ("Setting label to: '%s'", ewl_text_text_get(EWL_TEXT(ib->entry)));*/

		/*Hide the entry, now that we have an enter */
		ewl_widget_hide(ib->entry_floater);

		/*Show the label again*/
		ewl_widget_show(EWL_ICONBOX_ICON(ib->edit_icon)->w_label);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_editable_set(Ewl_Iconbox *ib, int edit)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);

	ib->editable = edit;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
ewl_iconbox_scrollpane_recalculate(Ewl_Iconbox *ib)
{
	int pw, ph;
	int w,h;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);

	ewl_object_current_size_get(EWL_OBJECT(ib->ewl_iconbox_scrollpane), &pw, &ph);	

	w = pw > ib->lx ? pw : ib->lx;
	h = ph > ib->ly + ib->ih ? ph + ib->ih : 
				ib->ly + (ib->ih * 2) + (EWL_ICONBOX_ICON_PADDING * 2);
	ewl_object_custom_size_set(EWL_OBJECT(ib->ewl_iconbox_pane_inner), w,h );
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_icon_arrange(Ewl_Iconbox *ib)
{
	int sw = 0,sh = 0;
	int iw = 0, ih = 0;
	int nx, ny;
	Ewl_Iconbox_Icon *list_item;
	int maxx = 0, maxy = 0;
	int px, py;
	/*int pw, ph;*/
	int x, y;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);

	/*printf ("Entering the arrange..\n");*/

	/* Recalc inner pane size */
	/*ewl_iconbox_inner_pane_calculate(ib);*/
	
	/*printf ("Ewl_Iconbox -> Arranging icons\n");*/


	px = CURRENT_X(ib);
	py = CURRENT_Y(ib);

	nx = ewl_object_current_x_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));
	ny = ewl_object_current_y_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));
	
	ib->lx = ib->ly = 0;

	ewl_object_current_size_get(EWL_OBJECT(ib->ewl_iconbox_scrollpane), &sw, &sh);
	
	if (ib->iw > 0 && ib->ih > 0) {
		iw = ib->iw;
		ih = ib->ih;
	} else {	
		ecore_list_goto_first(ib->ewl_iconbox_icon_list);
		while ((list_item = ecore_list_next(ib->ewl_iconbox_icon_list))) {
			int nw,nh;
		
			nw= ewl_object_preferred_w_get(EWL_OBJECT(list_item->image));
			nh= ewl_object_preferred_h_get(EWL_OBJECT(list_item->image));
			if (nw > iw) iw = nw;
			if (nh > ih) ih = nh;
		}
	}

	ecore_list_goto_first(ib->ewl_iconbox_icon_list);
	while((list_item = ecore_list_next(ib->ewl_iconbox_icon_list))) {
		if (iw < EWL_ICONBOX_MOVE_TOLERANCE) {
			iw = EWL_ICONBOX_MINIMUM_SIZE;
			ih = EWL_ICONBOX_MINIMUM_SIZE;
		}


		if ((ib->lx + ib->iw + (EWL_ICONBOX_ICON_PADDING)) >= (sw - ib->iw)) {
			ib->lx = 0;
			ib->ly += ih + EWL_ICONBOX_ICON_PADDING;
		}

		x = ewl_object_current_x_get(EWL_OBJECT(list_item));
		y = ewl_object_current_y_get(EWL_OBJECT(list_item));

		/* Only move if we have to */
		if (abs(x - ib->lx) > 0 || abs(y - ib->ly) > 0) {
			if ( !(list_item->ox || list_item->oy)) {
				ewl_object_position_request(EWL_OBJECT(list_item), 
						ib->lx + nx, ib->ly + ny);
			} else {
				ewl_object_position_request(EWL_OBJECT(list_item), 
					list_item->ox + ib->ox, list_item->oy + ib->oy);

			}
			//printf("Requested position: %d:%d\n", ib->lx + nx, ib->ly + ny);
		}


		ib->lx += iw + EWL_ICONBOX_ICON_PADDING;

		if (ib->lx > maxx) maxx = ib->lx;
		if (ib->ly > maxy) maxy = ib->ly;
	}

	/* Now set the extent of the pane inner to be the maxx/y that we had or 
	 * the size of the scrollpane, which ever is bigger
	 */
	ewl_iconbox_scrollpane_recalculate(ib);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
/* ------------ */

static void
ewl_iconbox_icon_select(Ewl_Iconbox_Icon *ib, int loc, int deselect) /* Loc 0= image, 1= label */
{
	int sel;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_TYPE("ib", ib, EWL_ICON_TYPE);

	sel = ib->selected;
	ib->icon_box_parent->select_icon = ib;

	if ((!ib->icon_box_parent->drag_box) && deselect) {
		Ewl_Iconbox_Icon *list_item;

		ecore_list_goto_first(ib->icon_box_parent->ewl_iconbox_icon_list);
		while((list_item = ecore_list_next(ib->icon_box_parent->ewl_iconbox_icon_list))) {
			ewl_iconbox_icon_deselect(list_item);	
		}
	}
	
	if (sel && loc == 1) {
		int w, h;
		int iw, ih;
		int x, y;
		char *text;

		/* TODO request an ewl_floater_position_get function */
		x = ewl_object_current_x_get(EWL_OBJECT(ib));
		y = ewl_object_current_y_get(EWL_OBJECT(ib));
		
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
		ewl_widget_layer_set(EWL_WIDGET(ib->icon_box_parent->entry_floater), ICONBOX_ENTRY_LAYER);
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

	DLEAVE_FUNCTION(DLEVEL_STABLE);	
}

static void
ewl_iconbox_icon_deselect(Ewl_Iconbox_Icon *ib)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_TYPE("ib", ib, EWL_ICON_TYPE);

	if (ib->selected) {
		ib->selected = 0;

		/*If we have a compressed label, set it now*/
		if (ib->label_compressed) {
			ewl_iconbox_icon_label_set(ib, ib->label_compressed);
		}
		ewl_text_cursor_position_set(EWL_TEXT(ib->w_label), 0);
		ewl_text_color_apply(EWL_TEXT(ib->w_label), 0, 0, 0, 255, 
				ewl_text_length_get(EWL_TEXT(ib->w_label)));
		ewl_widget_color_set(EWL_WIDGET(ib), 255, 255, 255, 255);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_deselect_all(Ewl_Iconbox *ib)
{
	Ewl_Iconbox_Icon *list_item;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);

	ecore_list_goto_first(ib->ewl_iconbox_icon_list);
	while((list_item = ecore_list_next(ib->ewl_iconbox_icon_list))) {
		ewl_iconbox_icon_deselect(list_item);	
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_icon_remove(Ewl_Iconbox *ib, Ewl_Iconbox_Icon *icon) 
{
	Ewl_Iconbox_Icon *list_item;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_PARAM_PTR("icon", icon);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);
	DCHECK_TYPE("icon", icon, EWL_ICON_TYPE);

	if (ib->ewl_iconbox_icon_list) {
		Ecore_List *new_icon_list;
		
		new_icon_list = ecore_list_new();
		ecore_list_goto_first(ib->ewl_iconbox_icon_list);
		while((list_item = ecore_list_next(ib->ewl_iconbox_icon_list))) {
			
			if (list_item == icon) {
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

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Iconbox_Icon * 
ewl_iconbox_icon_add(Ewl_Iconbox *iconbox, const char *name, const char *icon_file)
{
	Ewl_Widget *ib;
	int sw, sh;
	int px, py;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(EWL_ICONBOX_TYPE, iconbox, NULL);
	DCHECK_PARAM_PTR_RET("name", name, NULL);
	DCHECK_PARAM_PTR_RET("icon_file", icon_file, NULL);
	DCHECK_TYPE_RET(EWL_ICONBOX_TYPE, iconbox, EWL_ICONBOX_TYPE, NULL);

	/*Make a brand new icon*/
	ib = ewl_iconbox_icon_new();
	EWL_ICONBOX_ICON(ib)->selected = 0;
	EWL_ICONBOX_ICON(ib)->icon_box_parent = iconbox; /* Set our parent */
	ewl_object_fill_policy_set(EWL_OBJECT(ib), EWL_FLAG_FILL_FILL);

	/*Get the current iconbox location*/
	px = CURRENT_X(iconbox);
	py = CURRENT_Y(iconbox);

	/* Make the image */
	ewl_iconbox_icon_image_set(EWL_ICONBOX_ICON(ib), icon_file);
	ewl_container_child_append(EWL_CONTAINER(ib), EWL_ICONBOX_ICON(ib)->image);
	
	/* Add the floater to our container
	 */
	ewl_container_child_append(EWL_CONTAINER(iconbox->ewl_iconbox_pane_inner), 
					EWL_WIDGET(EWL_ICONBOX_ICON(ib)));
	ewl_object_position_request(EWL_OBJECT(ib),
				    	iconbox->lx + iconbox->ox, 
					iconbox->ly + iconbox->oy);
	
	/*----------------------*/
	/* Get the icon next position */

	ewl_object_current_size_get(EWL_OBJECT(iconbox->ewl_iconbox_scrollpane),
				    					&sw,&sh);

	if (  (iconbox->lx + ((iconbox->iw + EWL_ICONBOX_ICON_PADDING)*2)) >= (sw - iconbox->iw)) {
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

	/* Show */
	ewl_widget_show(EWL_ICONBOX_ICON(ib)->image);
	ewl_widget_show(EWL_ICONBOX_ICON(ib)->w_label);
	ewl_widget_show(EWL_WIDGET(ib));

	/* Set the label */
	ewl_iconbox_icon_label_setup(EWL_ICONBOX_ICON(ib), name);


	/* FIXME - at the moment, it appears we can't calculate the height 
		yet - hard set for now */
	//ewl_object_custom_h_set(EWL_OBJECT(ib), 80);
	ewl_object_custom_size_set(EWL_OBJECT(ib), 60, 60);

	/* Add the callbacks for mouse */
	ewl_callback_prepend(EWL_ICONBOX_ICON(ib)->image, EWL_CALLBACK_MOUSE_DOWN,
						ewl_iconbox_icon_mouse_down, ib);
	ewl_callback_prepend(EWL_ICONBOX_ICON(ib)->image, EWL_CALLBACK_MOUSE_UP, 
						ewl_iconbox_icon_mouse_up, ib);
	
	/* Add a callback to the border box label, for editing purposes... */
	ewl_callback_prepend(EWL_ICONBOX_ICON(ib)->w_label, EWL_CALLBACK_MOUSE_DOWN, 
						ewl_iconbox_icon_label_mouse_down_cb, ib);

	ewl_callback_prepend(EWL_WIDGET(ib), EWL_CALLBACK_DESTROY,
				   ewl_iconbox_icon_destroy_cb, NULL);

	/* Add this icon to the icon list */
	ecore_list_append(iconbox->ewl_iconbox_icon_list, ib);

	ewl_widget_layer_set(EWL_WIDGET(ib), ICONBOX_ICON_LAYER);
	ewl_widget_draggable_set(EWL_WIDGET(ib), 1, ewl_iconbox_drag_data_get);

	DRETURN_PTR(EWL_ICONBOX_ICON(ib), DLEVEL_STABLE);
}

void
ewl_iconbox_icon_image_set(Ewl_Iconbox_Icon *icon, const char *filename)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("icon", icon);
	DCHECK_PARAM_PTR("filename", filename);
	DCHECK_TYPE("icon", icon, EWL_ICON_TYPE);

	ewl_image_file_set(EWL_IMAGE(icon->image), filename, NULL);
	ewl_image_constrain_set(EWL_IMAGE(icon->image), 64);	

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_clear(Ewl_Iconbox *ib)
{
	Ewl_Iconbox_Icon *list_item;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);

	/*printf("*** Deleting all icons...\n");*/

	if (ib->ewl_iconbox_icon_list) {
		while((list_item = ecore_list_remove_first(ib->ewl_iconbox_icon_list))) {
			/*printf("Deleting icon..\n");*/

			ewl_widget_hide(EWL_WIDGET(list_item));
			//ewl_container_child_remove(EWL_CONTAINER(ib->ewl_iconbox_pane_inner), EWL_WIDGET(list_item));
			//
			ewl_widget_destroy(EWL_WIDGET(list_item));		
		}
		/*printf("...dione\n");*/
	}

	ib->drag_icon = NULL;
	ib->lx = 0;
	ib->ly = 0;

	//ewl_container_reset(EWL_CONTAINER(EWL_ICONBOX(ib)->ewl_iconbox_pane_inner));
	ewl_iconbox_scrollpane_recalculate(ib);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* Callbacks */
void
ewl_iconbox_dnd_drop_cb(Ewl_Widget *item __UNUSED__, 
			void *ev_data, void *user_data) 
{
	int ibx, iby, px, py, fw, fh;
	Ewl_Iconbox *ib;
	Ewl_Iconbox_Icon *list_item;
	Ewl_Event_Dnd_Drop *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_TYPE("user_data", user_data, EWL_ICONBOX_TYPE);

	ib = EWL_ICONBOX(user_data);
	list_item = ib->select_icon;
	ev = ev_data;

	ibx = ewl_object_current_x_get(EWL_OBJECT(ib));
	iby = ewl_object_current_y_get(EWL_OBJECT(ib));

	px = ewl_object_current_x_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));
	py = ewl_object_current_y_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));

	fw= ewl_object_preferred_w_get(EWL_OBJECT(list_item->image));
	fh= ewl_object_preferred_h_get(EWL_OBJECT(list_item->image));

	ewl_object_position_request(EWL_OBJECT(list_item),  ev->x - (fw/2), ev->y - (fh/2));

	printf("Setting override position to %d:%d, layout: %d:%d, offset: %d:%d, pane %d:%d, box: %d:%d\n", 
			ev->x - (fw/2), ev->y - (fh/2), 
			ib->lx, ib->ly, 
			ib->ox , ib->oy,
			px, py,
			ibx,iby
	);

	printf("Position: %d:%d\n", ev->x - (fw/2) - ibx + abs(px - ibx), ev->y - (fh/2) - iby  + abs(py - iby) );
	list_item->ox = ev->x - (fw/2) - ibx + abs(px - ibx);
	list_item->oy = ev->y - (fh/2) - iby  + abs(py - iby);


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_dnd_position_cb(Ewl_Widget *item __UNUSED__, 
		void *ev_data __UNUSED__, void *user_data) 
{
	int ibx, iby, px, py, fw, fh;
	Ewl_Iconbox *ib;
	Ewl_Iconbox_Icon *list_item;
	Ewl_Dnd_Types *types;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("user_data", user_data, EWL_ICONBOX_TYPE);

	ib = EWL_ICONBOX(user_data);
	list_item = ib->select_icon;

	ibx = ewl_object_current_x_get(EWL_OBJECT(ib));
	iby = ewl_object_current_y_get(EWL_OBJECT(ib));

	px = ewl_object_current_x_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));
	py = ewl_object_current_y_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));

	fw= ewl_object_preferred_w_get(EWL_OBJECT(list_item->image));
	fh= ewl_object_preferred_h_get(EWL_OBJECT(list_item->image));
	
	//ewl_object_position_request(EWL_OBJECT(list_item), (ev->x - ibx) + abs(px) - (fw/2),
	//			  (ev->y - iby) + abs(py) - (fh/2));

	/* Get types */
	if ((types = ewl_dnd_types_for_widget_get(EWL_WIDGET(ib)))) {
		/*printf("We have %d types!\n", types->num_types);
		if (types->num_types > 0) {
			printf("First type is '%s'\n", types->types[0]);
		}*/
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);	
}

void
ewl_iconbox_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__, void *user_data __UNUSED__)
{
	Ewl_Iconbox *ib;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ib = EWL_ICONBOX(w);

	ecore_list_destroy(ib->ewl_iconbox_icon_list);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_iconbox_icon_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Iconbox_Icon *icon;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_ICON_TYPE);

	icon = EWL_ICONBOX_ICON(w);
	if (icon->label) {
		free(icon->label);
	}

	if (icon->label_compressed) {
		free(icon->label_compressed);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* ----------- */
void
ewl_iconbox_arrange_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Iconbox* ib;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("user_data", user_data, EWL_ICONBOX_TYPE);

	ib = EWL_ICONBOX(user_data);
	ewl_iconbox_icon_arrange(ib);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_mouse_move_cb(Ewl_Widget *w __UNUSED__, void *ev_data, void *user_data)
{
	Ewl_Iconbox *ib;
	Ewl_Event_Mouse_Move *ev;
	Ewl_Iconbox_Icon *list_item;
	int ibx, iby;
	int px, py;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_TYPE("user_data", user_data, EWL_ICONBOX_TYPE);

	ib = EWL_ICONBOX(user_data);
	ev = ev_data;
	list_item = ib->drag_icon;

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

		
		ewl_floater_position_set(EWL_FLOATER(ib->select_floater), 
						(lx - ibx) + abs(px - ibx), 
						(ly + 1 - iby) + abs(py - iby));
			
		if (hx-lx >0 && hy-ly > 0) {
			ewl_object_custom_size_set(EWL_OBJECT(ib->select), 
							(hx - lx), (hy - ly));
			ewl_object_custom_size_set(EWL_OBJECT(ib->select_floater), 
								hx - lx, hy - ly);
		}

		/*Now check which icons we have to select in this range...*/
		{
			int w, h;
			
			Ewl_Iconbox_Icon *list_item;
			int ix, iy;

			ecore_list_goto_first(ib->ewl_iconbox_icon_list);
		
			w = ewl_object_current_w_get(EWL_OBJECT(ib->select));
			h = ewl_object_current_h_get(EWL_OBJECT(ib->select));
			while((list_item = ecore_list_next(ib->ewl_iconbox_icon_list))) {
				
				ix = ewl_object_current_x_get(EWL_OBJECT(list_item));
				iy = ewl_object_current_y_get(EWL_OBJECT(list_item));
				
				if ((ix >= lx) && (iy >= ly) 
						&& (ix <= hx) && (iy <= hy) 
						&& VISIBLE(EWL_WIDGET(list_item)) 
						&& !OBSCURED(EWL_WIDGET(list_item))) 
				{
					ewl_iconbox_icon_select(EWL_ICONBOX_ICON(list_item), 0, 0);
				} 
				else if (list_item->selected 
						&& !(ev->modifiers == EWL_KEY_MODIFIER_CTRL)) 
				{
					ewl_iconbox_icon_deselect(EWL_ICONBOX_ICON(list_item));
				}
			}
		}

		/* If we're drag boxing, we can't be moving an icon, so return.. */
		DRETURN(DLEVEL_STABLE);
	}

	/*Check move tolerance, if not past, then don't move..*/
	if (abs(ev->x - ib->xdown) < EWL_ICONBOX_MOVE_TOLERANCE 
			&& abs(ev->y - ib->ydown) < EWL_ICONBOX_MOVE_TOLERANCE) 
		DRETURN(DLEVEL_STABLE);
	
	
	if (list_item != NULL) {			
		DRETURN(DLEVEL_STABLE);
		
		/*Ewl_Widget* icon_box = (Ewl_Widget*)list_item->box;*/
		int fw,fh;

		/* Get the value of the scrollpane..  */

		/* Get the current width/height to centre */
			
		fw= ewl_object_current_w_get(EWL_OBJECT(list_item->image));
		fh= ewl_object_current_h_get(EWL_OBJECT(list_item->image));
		
		ewl_object_position_request(EWL_OBJECT(list_item), 
							ev->x - (fw/2), 
							ev->y - (fh/2));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_pane_mouse_down_cb(Ewl_Widget *w __UNUSED__, void *ev_data, void *user_data)
{
	Ewl_Iconbox *ib;
	Ewl_Event_Mouse_Down *ev;
	int ibx, iby;
	int px, py;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_TYPE("user_data", user_data, EWL_ICONBOX_TYPE);

	ib = EWL_ICONBOX(user_data);	
	ev = ev_data;

	px = ewl_object_current_x_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));
	py = ewl_object_current_y_get(EWL_OBJECT(ib->ewl_iconbox_pane_inner));	


	ibx = ewl_object_current_x_get(EWL_OBJECT(ib));
	iby = ewl_object_current_y_get(EWL_OBJECT(ib));
	
	if (ev->button == 3 /* Confirm that this is not an icon event */ 
			&& (ib->xdown != ev->x 
				&& ib->ydown != ev->y)) 
	{
		ewl_widget_show(ib->ewl_iconbox_menu_floater);
		ewl_floater_position_set(EWL_FLOATER(ib->ewl_iconbox_menu_floater), 
							ev->x-ibx + abs(px-ibx), 
							ev->y-iby +abs(py-iby));
		ewl_callback_call(EWL_WIDGET(ib->ewl_iconbox_context_menu), 
							EWL_CALLBACK_FOCUS_IN);

		ewl_object_state_remove(EWL_OBJECT(ib->ewl_iconbox_menu_floater), EWL_FLAG_STATE_PRESSED);
	} else if (ev->button == 1 /* Confirm that this is not an icon event */ 
			&& (ib->xdown != ev->x 
				&& ib->ydown != ev->y)) 
	{

		ewl_object_custom_size_set(EWL_OBJECT(ib->select), 1, 1);
		
		/* Put the floater at the position we started at */
		ewl_floater_position_set(EWL_FLOATER(ib->select_floater), ev->x-ibx + abs(px-ibx), ev->y-iby + abs(py-iby));
		
		ewl_widget_show(EWL_WIDGET(ib->select_floater));
		ib->drag_box = 1;
		ewl_dnd_disable();
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_icon_mouse_down(Ewl_Widget *w __UNUSED__, void *ev_data, void *user_data)
{
	int ibx, iby, px, py, sx, sy;
	Ewl_Iconbox_Icon *ib;
	Ewl_Event_Mouse_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_TYPE("user_data", user_data, EWL_ICON_TYPE);

	ib = user_data;
	ev = ev_data;

	ib->icon_box_parent->xdown = ev->x;
	ib->icon_box_parent->ydown = ev->y;

	ib->icon_box_parent->select_icon = ib; /*We rely on this being the first callback - so
						 client*/

	px = ewl_object_current_x_get(EWL_OBJECT(ib));
	py = ewl_object_current_y_get(EWL_OBJECT(ib));

	sx = ewl_object_current_x_get(EWL_OBJECT(ib->icon_box_parent->ewl_iconbox_pane_inner));
	sy = ewl_object_current_y_get(EWL_OBJECT(ib->icon_box_parent->ewl_iconbox_pane_inner));	
	
	ibx = ewl_object_current_x_get(EWL_OBJECT(ib->icon_box_parent));
	iby = ewl_object_current_y_get(EWL_OBJECT(ib->icon_box_parent));

	if (ev->button == 3) {
		ewl_floater_position_set(EWL_FLOATER(ib->icon_box_parent->icon_menu_floater), 
						ev->x-ibx + abs(sx-ibx), 
						ev->y-iby +abs(sy-iby));
		ewl_callback_call(EWL_WIDGET(ib->icon_box_parent->icon_menu), 
						EWL_CALLBACK_FOCUS_IN);
	} else {
		DRETURN(DLEVEL_STABLE);

		/*Select/drag start*/
		ib->drag = 1;
		ib->icon_box_parent->drag_icon = ib;
	}

	/*ewl_callback_call_with_event_data(EWL_WIDGET(ib), EWL_CALLBACK_MOUSE_DOWN, ev_data);*/

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_icon_mouse_up(Ewl_Widget *w __UNUSED__, void *ev_data , void *user_data)
{
	Ewl_Event_Mouse_Down *ev;
	Ewl_Iconbox_Icon *ib;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_TYPE("user_data", user_data, EWL_ICON_TYPE);

	ib = user_data;
	ev = ev_data;

	/* Set this to selected */
	if (ev->modifiers == EWL_KEY_MODIFIER_CTRL) {
		ewl_iconbox_icon_select(ib, 0, 0);
	} else {
		ewl_iconbox_icon_select(ib, 0, 1);	
	}

	ib->drag = 0;
	ib->icon_box_parent->drag_icon = NULL;
	/*printf ("Button up on icon: %s\n", ewl_border_text_get(EWL_BORDER(ib)) );*/

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_mouse_up(Ewl_Widget *w __UNUSED__, void *ev_data, void *user_data)
{
	Ewl_Event_Mouse_Up *ev;
	Ewl_Iconbox *ib;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_TYPE("user_data", user_data, EWL_ICONBOX_TYPE);

	ib = user_data;
	ev = ev_data;
	if (ev->button == 1) {
		/*printf("Stopped select: %d:%d\n", ev->x, ev->y);*/
		ewl_widget_hide(EWL_WIDGET(ib->select_floater));
		ib->drag_box = 0;
		ib->dx = -1;
		ib->dy = -1;

		ewl_dnd_enable();
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_icon_label_mouse_down_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Iconbox_Icon *ib;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("user_data", user_data, EWL_ICON_TYPE);

	ib = user_data;

	/* Set this to selected */
	ewl_iconbox_icon_select(ib, 1, 1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ib: The iconbox
 * @return Returns no value
 * @brief	Initialize the icon box
 */
void
ewl_iconbox_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__, void *user_data __UNUSED__)
{
	Ewl_Iconbox *ib;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_ICONBOX_TYPE);

	ib = EWL_ICONBOX(w);

	ewl_iconbox_inner_pane_calculate(EWL_ICONBOX(w));
	ewl_iconbox_icon_arrange(ib); 

	if (ib->background) {
		int width, height;
		int sw, sh = 0;
		Ewl_Widget *parent;

		sw = CURRENT_W(EWL_SCROLLPANE(ib->ewl_iconbox_scrollpane)->vscrollbar);
		parent = w->parent;

		if (VISIBLE(EWL_SCROLLPANE(ib->ewl_iconbox_scrollpane)->hscrollbar))
			sh = CURRENT_H(EWL_SCROLLPANE(ib->ewl_iconbox_scrollpane)->hscrollbar);
		
		width = CURRENT_W(ib);
		height = CURRENT_H(ib);
		ewl_object_position_request(EWL_OBJECT(ib->background),
				CURRENT_X(parent), CURRENT_Y(parent));
		ewl_object_custom_size_set(EWL_OBJECT(ib->background), width-sw,
				height-sh);

		ewl_widget_layer_set(EWL_WIDGET(ib->background), ICONBOX_BACKGROUND_LAYER);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

double
ewl_iconbox_icon_distance(Ewl_Iconbox_Icon *i1, Ewl_Iconbox_Icon *i2) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("i1", i1, FALSE);
	DCHECK_PARAM_PTR_RET("i2", i2, FALSE);
	DCHECK_TYPE_RET("i1", i1, EWL_ICON_TYPE, FALSE);
	DCHECK_TYPE_RET("i2", i2, EWL_ICON_TYPE, FALSE);

	DRETURN_FLOAT(sqrt(pow(CURRENT_X(i1) - CURRENT_X(i2), 2) 
				+ pow(CURRENT_Y(i1) - CURRENT_Y(i2), 2)), DLEVEL_STABLE);
}

void
ewl_iconbox_key_press_cb (Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, void *user_data) 
{
	Ewl_Event_Key_Down *event;
	Ewl_Iconbox *ib;
	struct timeval tim;
	int cx = 0,cy = 0;
	Ewl_Iconbox_Icon *list_item;
	Ewl_Iconbox_Icon *current;
	Ewl_Iconbox_Icon *best = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_TYPE("user_data", user_data, EWL_ICONBOX_TYPE);

	ib = user_data;
	current = ib->select_icon;
	event = ev_data;

	gettimeofday(&tim, NULL);

	if (current) {
		cx = CURRENT_X(current);
		cy = CURRENT_Y(current);
	}

	
	if (!(((double)(tim.tv_sec) * 1000000) + tim.tv_usec < 
				((double)(ib->lasttime.tv_sec) * 1000000) 
				+ ib->lasttime.tv_usec + ICONBOX_REPEAT)) {
		gettimeofday(&ib->lasttime, NULL);

		if (!strcmp(event->keyname, "Down")) {
			ewl_iconbox_deselect_all(ib);	
			ecore_list_goto_first(ib->ewl_iconbox_icon_list);
			while((list_item = ecore_list_next(ib->ewl_iconbox_icon_list))) {
				if (!current) {
					current = list_item;
					cx = CURRENT_X(current);
					cy = CURRENT_Y(current);
				}
				
				if (CURRENT_Y(list_item) > cy) {
					if (!best) 
						best = list_item;
					else if (ewl_iconbox_icon_distance(current,best) > 
							ewl_iconbox_icon_distance(current,list_item)) 
						best = list_item;

				}
			}
		} else if (!strcmp(event->keyname, "Up")) {
			ewl_iconbox_deselect_all(ib);	
			ecore_list_goto_first(ib->ewl_iconbox_icon_list);
			while((list_item = ecore_list_next(ib->ewl_iconbox_icon_list))) {
				if (!current) {
					current = list_item;
					cx = CURRENT_X(current);
					cy = CURRENT_Y(current);
				}
				
				if (CURRENT_Y(list_item) < cy) {
					if (!best) 
						best = list_item;
				 	else if (ewl_iconbox_icon_distance(current,best) > 
							ewl_iconbox_icon_distance(current,list_item)) 
						best = list_item;
					
				}
			}		
		} else if (!strcmp(event->keyname, "Left")) {
			ewl_iconbox_deselect_all(ib);	
			ecore_list_goto_first(ib->ewl_iconbox_icon_list);
			while((list_item = ecore_list_next(ib->ewl_iconbox_icon_list))) {
				if (!current) {
					current = list_item;
					cx = CURRENT_X(current);
					cy = CURRENT_Y(current);
				}
				
				if (CURRENT_X(list_item) < cx) {
					if (!best) 
						best = list_item;
				 	else if (ewl_iconbox_icon_distance(current,best) > 
							ewl_iconbox_icon_distance(current,list_item)) 
						best = list_item;
				}
			}		
		} else if (!strcmp(event->keyname, "Right")) {
			ewl_iconbox_deselect_all(ib);	
			ecore_list_goto_first(ib->ewl_iconbox_icon_list);
			while((list_item = ecore_list_next(ib->ewl_iconbox_icon_list))) {
				if (!current) {
					current = list_item;
					cx = CURRENT_X(current);
					cy = CURRENT_Y(current);
				}
				
				if (CURRENT_X(list_item) > cx) {
					if (!best) 
						best = list_item;
				 	else if (ewl_iconbox_icon_distance(current,best) > 
							ewl_iconbox_icon_distance(current,list_item)) 
						best = list_item;
				}
			}		
		} else if (ib->key_event_cb) {
			(*ib->key_event_cb)(ib, ib->key_event_data, event->keyname);
		}

		/* Perform new selection, if any */
		if (best) ewl_iconbox_icon_select(best, 0, 1);

		if (!strcmp(event->keyname, "Down") && best && OBSCURED(best)) {
			Ewl_Event_Mouse_Wheel ev;
			ev.z = 1;
			ewl_callback_call_with_event_data(ib->ewl_iconbox_scrollpane, EWL_CALLBACK_MOUSE_WHEEL, &ev);

		} else if (!strcmp(event->keyname, "Up") && best && OBSCURED(best)) {
			Ewl_Event_Mouse_Wheel ev;
			ev.z = -1;
			ewl_callback_call_with_event_data(ib->ewl_iconbox_scrollpane, EWL_CALLBACK_MOUSE_WHEEL, &ev);
		}
	} 

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_iconbox_controlled_key_callback_register(Ewl_Iconbox *ib, void (*cb)(Ewl_Iconbox *, void *, char *), void *data) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ib", ib);
	DCHECK_PARAM_PTR("cb", cb);
	DCHECK_TYPE("ib", ib, EWL_ICONBOX_TYPE);

	ib->key_event_cb = cb;
	ib->key_event_data = data;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

