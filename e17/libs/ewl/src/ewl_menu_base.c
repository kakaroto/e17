#include <Ewl.h>

void		__ewl_menu_base_expand(Ewl_Widget *w, void *ev_data,
					void *user_data);
void            __ewl_menu_base_collapse(Ewl_Widget * w, void *ev_data,
					 void *user_data);
void            __ewl_menu_add(Ewl_Container * parent, Ewl_Widget * child);
void            __item_clicked(Ewl_Widget * w, void *ev_data, void *user_data);

/**
 * ewl_menu_base_init - initialize a menu to starting values
 * @menu: the menu to initialize
 * @follows: the widget the menu will follow
 * @type: the menu type
 *
 * Returns nothing. Sets up the internal variables for the menu.
 */
void ewl_menu_base_init(Ewl_Menu_Base * menu, char *image, char *title)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("menu", menu);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	ewl_menu_item_init(EWL_MENU_ITEM(menu), image, title);
	ewl_object_set_fill_policy(EWL_OBJECT(menu), EWL_FILL_POLICY_NONE);

	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_SELECT,
			    __ewl_menu_base_expand, NULL);

	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_DESELECT,
			    __ewl_menu_base_collapse, NULL);

	/*
	 * The popbox actually holds the children, and is simply added to the
	 * popup part of the menu when the menu is expanded.
	 */
	menu->popbox = ewl_vbox_new();
	ewl_object_set_alignment(EWL_OBJECT(menu->popbox),
				 EWL_ALIGNMENT_LEFT | EWL_ALIGNMENT_TOP);
	ewl_widget_show(menu->popbox);

	/*
	 * The add notifier makes sure newly added children go in the popup
	 * menu.
	 */
	ewl_container_add_notify(EWL_CONTAINER(menu), __ewl_menu_add);

	/*
	 * Initialize the remaining fields of the menu.
	 */
	menu->t_expand = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_menu_item_new - create a new menu item to place in a menu
 * @image: the path to the image to use as an icon
 * @text: the text to display for the menu item
 *
 * Returns a pointer to a newly allocated menu item on success, NULL on
 * failure.
 */
Ewl_Widget     *ewl_menu_item_new(char *image, char *text)
{
	Ewl_Menu_Item  *item;

	DENTER_FUNCTION(DLEVEL_STABLE);

	item = NEW(Ewl_Menu_Item, 1);
	if (!item)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ZERO(item, Ewl_Menu_Item, 1);

	ewl_menu_item_init(item, image, text);

	DRETURN_PTR(EWL_WIDGET(item), DLEVEL_STABLE);
}

/**
 * ewl_menu_item_init - initialize the fields of a menu item to their defaults
 * @item: the item to be initialized
 * @image: the path to image to be used, NULL for no image
 * @text: the text for this menuitem
 *
 * Returns no value. Initializes a menu item to default values and adds the
 * image pointed to by the path @image, and adds the text in @text.
 */
void ewl_menu_item_init(Ewl_Menu_Item * item, char *image, char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("item", item);

	/*
	 * Initialize the inherited fields and override an appearance setting
	 * and the recursive setting. This will cause clicks to stop at this
	 * level.
	 */
	ewl_box_init(EWL_BOX(item), EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_set_appearance(EWL_WIDGET(item), "menuitem");

	ewl_container_notify_callback(EWL_CONTAINER(item),
			EWL_CALLBACK_CLICKED);
	ewl_container_notify_callback(EWL_CONTAINER(item),
			EWL_CALLBACK_MOUSE_DOWN);
	ewl_container_notify_callback(EWL_CONTAINER(item),
			EWL_CALLBACK_MOUSE_UP);
	ewl_container_notify_callback(EWL_CONTAINER(item),
			EWL_CALLBACK_MOUSE_MOVE);
	ewl_container_notify_callback(EWL_CONTAINER(item),
			EWL_CALLBACK_SELECT);
	ewl_container_notify_callback(EWL_CONTAINER(item),
			EWL_CALLBACK_DESELECT);

	/*
	 * Create the icon if one is requested.
	 */
	if (image) {
		item->icon = ewl_image_new(image);
		ewl_object_set_maximum_size(EWL_OBJECT(item->icon), 20, 20);
		ewl_object_set_alignment(EWL_OBJECT(item->icon),
				EWL_ALIGNMENT_CENTER);
		ewl_container_append_child(EWL_CONTAINER(item), item->icon);
		ewl_widget_show(item->icon);
	}

	/*
	 * Create the text object for the menu item.
	 */
	if (text) {
		item->text = ewl_text_new(text);
		ewl_container_append_child(EWL_CONTAINER(item), item->text);
		ewl_object_set_alignment(EWL_OBJECT(item->text),
				EWL_ALIGNMENT_LEFT);
		ewl_widget_show(item->text);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_menu_add(Ewl_Container * parent, Ewl_Widget * child)
{
	Ewl_IMenu      *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Place the newly added child in the popup menu.
	 */
	menu = EWL_IMENU(parent);
	ewl_container_append_child(EWL_CONTAINER(menu->popbox), child);
	EWL_MENU_ITEM(child)->submenu = TRUE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_menu_base_expand(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Menu_Base *menu = EWL_MENU_BASE(w);

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!menu->popup)
		DRETURN(DLEVEL_STABLE);

	ewl_container_append_child(EWL_CONTAINER(menu->popup), menu->popbox);
	ewl_widget_show(menu->popup);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_menu_base_collapse(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Menu_Base      *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = EWL_MENU_BASE(w);

	ewl_widget_hide(menu->popup);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
