
#include <Ewl.h>

void            __ewl_menu_add(Ewl_Container * parent, Ewl_Widget * child);
void            __expand_imenu(Ewl_Widget * w, void *ev_data, void *user_data);
void            __collapse_imenu(Ewl_Widget * w, void *ev_data,
				 void *user_data);

void            __item_clicked(Ewl_Widget * w, void *ev_data, void *user_data);

void            __ewl_imenu_realize(Ewl_Widget * w, void *ev_data,
				    void *user_data);


void            __expand_imenu_animator(int val, void *ev_data);
void            __collapse_imenu_animator(int val, void *ev_data);
void            __expand_imenu_and_title_animator(int val, void *ev_data);
void            __collapse_imenu_and_title_animator(int val, void *ev_data);

void            __ewl_imenu_start(Ewl_Widget * w);

void            __ewl_imenu_item_show(Ewl_Widget * w, void *ev_data,
				      void *user_data);

/**
 * ewl_imenu_new - create a new internal menu
 * @image: the image icon to use for this menu
 * @title: the text to place in the menu
 *
 * Returns a pointer to a newly allocated menu on success, NULL on
 * failure.
 */
Ewl_Widget     *
ewl_imenu_new(char *image, char *title)
{
	Ewl_IMenu      *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = NEW(Ewl_IMenu, 1);
	if (!menu)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ZERO(menu, Ewl_IMenu, 1);

	ewl_imenu_init(menu, image, title);

	DRETURN_PTR(EWL_WIDGET(menu), DLEVEL_STABLE);
}


/**
 * ewl_imenu_init - initialize an internal menu to starting values
 * @menu: the menu to initialize
 * @follows: the widget the menu will follow
 * @type: the menu type
 *
 * Returns nothing.
 */
void
ewl_imenu_init(Ewl_IMenu * menu, char *image, char *title)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("menu", menu);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	ewl_imenu_item_init(EWL_IMENU_ITEM(menu), image, title);
	ewl_object_set_fill_policy(EWL_OBJECT(menu), EWL_FILL_POLICY_NONE);

	/*
	 * The realize needs to create the pop-up.
	 */
	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_REALIZE,
			    __ewl_imenu_realize, NULL);
	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_SELECT,
			    __expand_imenu, NULL);
	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_DESELECT,
			    __collapse_imenu, NULL);

	/*
	 * Create the popup menu portion of the menu.
	 */
	menu->popup = ewl_floater_new(EWL_WIDGET(menu));
	ewl_object_set_fill_policy(EWL_OBJECT(menu->popup),
				   EWL_FILL_POLICY_NONE);
	ewl_object_set_alignment(EWL_OBJECT(menu->popup),
				 EWL_ALIGNMENT_LEFT | EWL_ALIGNMENT_TOP);

	/*
	 * The add notifier makes sure newly added children go in the popup
	 * menu.
	 */
	ewl_container_add_notify(EWL_CONTAINER(menu), __ewl_menu_add);

	/*
	 * Initialize the remaining fields of the menu.
	 */
	menu->t_expand = 0;
	menu->width = 100;
	menu->height = CURRENT_H(menu);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_imenu_item_new - create a new menu item to place in a menu
 * @image: the path to the image to use as an icon
 * @text: the text to display for the menu item
 *
 * Returns a pointer to a newly allocated menu item on success, NULL on
 * failure.
 */
Ewl_Widget     *
ewl_imenu_item_new(char *image, char *text)
{
	Ewl_IMenu_Item *item;

	DENTER_FUNCTION(DLEVEL_STABLE);

	item = NEW(Ewl_IMenu_Item, 1);
	if (!item)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ZERO(item, Ewl_IMenu_Item, 1);

	ewl_imenu_item_init(item, image, text);

	DRETURN_PTR(EWL_WIDGET(item), DLEVEL_STABLE);
}

/**
 * ewl_imenu_item_init - initialize the fields of a menu item to their defaults
 */
void
ewl_imenu_item_init(Ewl_IMenu_Item * item, char *image, char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("item", item);

	/*
	 * Initialize the inherited fields and override an appearance setting
	 * and the recursive setting. This will cause clicks to stop at this
	 * level.
	 */
	ewl_box_init(EWL_BOX(item), EWL_ORIENTATION_HORIZONTAL);
	RECURSIVE(item) = FALSE;

	/*
	 * Create the icon if one is requested.
	 */
	if (image != NULL) {
		item->icon = ewl_image_load(image);
		ewl_container_append_child(EWL_CONTAINER(item), item->icon);
	}

	/*
	 * Create the text object for the menu item.
	 */
	item->text = ewl_text_new();
	ewl_text_set_text(EWL_TEXT(item->text), text);
	ewl_container_append_child(EWL_CONTAINER(item), item->text);

	/*
	 * Attach the callback for collapsing the menu when the item is
	 * clicked.
	 */
	/* ewl_callback_append(EWL_WIDGET(item), EWL_CALLBACK_MOUSE_UP,
	 * __collapse_imenu, NULL); */
	ewl_callback_append(EWL_WIDGET(item), EWL_CALLBACK_SHOW,
			    __ewl_imenu_item_show, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_imenu_set_title_expandable - allow title to be expanded/contracted
 * @menu: the menu
 *
 * Returns nothing.
 */
void
ewl_imenu_set_title_expandable(Ewl_IMenu * menu)
{

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu->t_expand = 1;

	/*
	 * ewl_object_set_maximum_size (EWL_OBJECT(menu->corner), 30, 22);
	 * ewl_object_set_minimum_size (EWL_OBJECT(menu->corner), 30, 22);
	 */

	/*
	 * ewl_callback_append(menu->corner_button, EWL_CALLBACK_MOUSE_UP,
	 * __expand_imenu, menu);
	 * ewl_container_append_child(EWL_CONTAINER(menu->corner), 
	 * menu->corner_button);
	 * 
	 * ewl_widget_configure(EWL_WIDGET(menu->corner));
	 */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_menu_add(Ewl_Container * parent, Ewl_Widget * child)
{
	Ewl_IMenu      *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Place the newly added child in the popup menu.
	 */
	menu = EWL_IMENU(parent);
	ewl_container_append_child(EWL_CONTAINER(menu->popup), child);
	EWL_IMENU_ITEM(child)->submenu = TRUE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_imenu_item_show(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_IMenu_Item *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = EWL_IMENU_ITEM(w);

	if (menu->icon)
		ewl_widget_show(menu->icon);
	ewl_widget_show(menu->text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__expand_imenu(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_IMenu      *menu;

	menu = EWL_IMENU(w);

	/*
	 * Position the popup menu relative to the menu.
	 */
	if (EWL_IMENU_ITEM(w)->submenu)
		ewl_floater_set_position(EWL_FLOATER(menu->popup), CURRENT_W(w),
					 0);
	else {
		ewl_floater_set_position(EWL_FLOATER(menu->popup), 0,
					 CURRENT_H(w));
		ewl_object_set_minimum_width(EWL_OBJECT(menu->popup),
					     CURRENT_W(menu));
	}

	ewl_widget_show(menu->popup);
	ewl_widget_configure(menu->popup);
}


void
__collapse_imenu(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_IMenu      *menu;

	menu = EWL_IMENU(w);

	ewl_widget_hide(menu->popup);
}


void
__expand_imenu_animator(int val, void *ev_data)
{
}


void
__collapse_imenu_animator(int val, void *ev_data)
{
}


void
__expand_imenu_and_title_animator(int val, void *ev_data)
{
}


void
__collapse_imenu_and_title_animator(int val, void *ev_data)
{
}


void
__ewl_imenu_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_IMenu      *menu;

	menu = EWL_IMENU(w);

	ewl_container_append_child(EWL_CONTAINER
				   (ewl_window_find_window_by_widget(w)),
				   menu->popup);
}

void
__ewl_imenu_start(Ewl_Widget * w)
{
}
