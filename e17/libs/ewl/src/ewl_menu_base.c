#include <Ewl.h>

void		__ewl_menu_base_expand(Ewl_Widget *w, void *ev_data,
					void *user_data);
void            __ewl_menu_base_collapse(Ewl_Widget * w, void *ev_data,
					 void *user_data);
void            __ewl_menu_add(Ewl_Container * parent, Ewl_Widget * child);
void            __item_clicked(Ewl_Widget * w, void *ev_data, void *user_data);

/**
 * @param menu: the menu item to initialize
 * @param image: the icon for the menu item
 * @param title: the label for the menu item
 * @return Returns nothing.
 * @brief Initialize a menu item to default values
 *
 * Sets up the internal variables for the menu item and places the icon from
 * @a image and label from @a title in the menu item.
 */
void ewl_menu_base_init(Ewl_Menu_Base * menu, char *image, char *title)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("menu", menu);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	ewl_menu_item_init(EWL_MENU_ITEM(menu), image, title);

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

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param image: the path to the image to use as an icon
 * @param text: the text to display for the menu item
 * @return Returns a pointer to a new menu item on success, NULL on failure.
 * @brief Create a new menu item to place in a menu
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
 * @param item: the item to be initialized
 * @param image: the path to image to be used, NULL for no image
 * @param text: the text for this menuitem
 * @return Returns no value.
 * @brief Initialize the fields of a menu item to their defaults
 *
 * Initializes a menu item to default values and adds the
 * image pointed to by the path @a image, and adds the text in @a text.
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
	ewl_object_set_fill_policy(EWL_OBJECT(item), EWL_FILL_POLICY_HFILL);

	ewl_container_intercept_callback(EWL_CONTAINER(item),
			EWL_CALLBACK_CLICKED);
	ewl_container_intercept_callback(EWL_CONTAINER(item),
			EWL_CALLBACK_MOUSE_DOWN);
	ewl_container_intercept_callback(EWL_CONTAINER(item),
			EWL_CALLBACK_MOUSE_UP);
	ewl_container_intercept_callback(EWL_CONTAINER(item),
			EWL_CALLBACK_MOUSE_MOVE);
	ewl_container_intercept_callback(EWL_CONTAINER(item),
			EWL_CALLBACK_SELECT);
	ewl_container_intercept_callback(EWL_CONTAINER(item),
			EWL_CALLBACK_DESELECT);

	/*
	 * Create the icon if one is requested, or a spacer if not, but there is
	 * text to be displayed.
	 */
	if (image)
		item->icon = ewl_image_new(image);
	else if (text)
		item->icon = ewl_spacer_new();

	/*
	 * Did we create an icon or a placeholder? Goodie, then finish it's
	 * setup. We don't always want to create one, in the case that a
	 * separator is going to be packed in here instead.
	 */
	if (item->icon) {
		ewl_object_set_alignment(EWL_OBJECT(item->icon),
				EWL_ALIGNMENT_CENTER);
		ewl_object_set_maximum_size(EWL_OBJECT(item->icon), 20, 20);
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

/**
 * @return Returns a new menu item separator on success, NULL on failure.
 * @brief Create a separator menu item
 */
Ewl_Menu_Separator *ewl_menu_separator_new()
{
	Ewl_Menu_Separator *sep;

	DENTER_FUNCTION(DLEVEL_STABLE);

	sep = NEW(Ewl_Menu_Separator, 1);
	if (!sep)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ZERO(sep, Ewl_Menu_Separator, 1);
	ewl_menu_separator_init(sep);

	DRETURN_PTR(sep, DLEVEL_STABLE);
}

/**
 * @param sep: the menu separator item to initialize
 * @return Returns no value.
 * @brief Initialize a menu separator item
 *
 * Sets up the internal fields of the menu separator item to
 * some sane defaults.
 */
void ewl_menu_separator_init(Ewl_Menu_Separator *sep)
{
	Ewl_Widget *separator;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("sep", sep);

	ewl_menu_item_init(EWL_MENU_ITEM(sep), NULL, NULL);

	separator = ewl_separator_new(EWL_ORIENTATION_HORIZONTAL);
	if (!separator)
		DRETURN(DLEVEL_STABLE);

	ewl_container_append_child(EWL_CONTAINER(sep), separator);
	ewl_widget_show(separator);

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
