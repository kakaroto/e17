
#include <Ewl.h>

/**
 * @param image: the image icon to use for this menu
 * @param title: the text to place in the menu
 * @return Returns a pointer to a new menu on success, NULL on failure.
 * @brief Create a new internal menu
 */
Ewl_Widget     *ewl_menu_new(char *image, char *title)
{
	Ewl_Menu      *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = NEW(Ewl_Menu, 1);
	if (!menu)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_menu_init(menu, image, title);

	DRETURN_PTR(EWL_WIDGET(menu), DLEVEL_STABLE);
}


/**
 * @param menu: the menu to initialize
 * @param image: the path to the icon image
 * @param title: the string displayed in the title
 * @return Returns no value.
 * @brief Initialize an internal menu to starting values
 */
void ewl_menu_init(Ewl_Menu * menu, char *image, char *title)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("menu", menu);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	ewl_menu_base_init(EWL_MENU_BASE(menu), image, title);

	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_SELECT,
			    ewl_menu_expand_cb, NULL);
	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_CONFIGURE,
			    ewl_menu_configure_cb, NULL);

	/*
	 * Create the popup menu portion of the widget.
	 */
	menu->base.popup = ewl_window_new();
	ewl_window_set_borderless(EWL_WINDOW(menu->base.popup));
	ewl_widget_set_internal(menu->base.popup, TRUE);
	ewl_widget_set_layer(menu->base.popup, 1000);
	ewl_widget_set_appearance(EWL_WIDGET(menu->base.popup), "menu");
	ewl_box_set_orientation(EWL_BOX(menu->base.popup),
				EWL_ORIENTATION_VERTICAL);
	ewl_object_fill_policy_set(EWL_OBJECT(menu->base.popup),
				   EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(menu->base.popup),
				 EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_menu_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	int x, y;
	Ewl_Menu *menu = EWL_MENU(w);
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = ewl_embed_find_by_widget(w);

	if (ewl_widget_is_type(EWL_WIDGET(emb), "window"))
		ewl_window_get_position(EWL_WINDOW(emb), &x, &y);
	else {
		x = CURRENT_X(w);
		y = CURRENT_Y(w);
	}

	/*
	 * Position the popup menu relative to the menu.
	 */
	if (EWL_MENU_ITEM(w)->inmenu) {
		ewl_window_move(EWL_WINDOW(menu->base.popup),
				x + CURRENT_W(w), y);
	}
	else {
		ewl_window_move(EWL_WINDOW(menu->base.popup), x,
				y + CURRENT_H(w));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_menu_expand_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Menu      *menu;
	Ewl_Embed      *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = EWL_MENU(w);

	if (!REALIZED(menu->base.popup)) {
		emb = ewl_embed_find_by_widget(w);
		ewl_container_append_child(EWL_CONTAINER(emb),
					   menu->base.popup); 
	}

	ewl_widget_show(menu->base.popup);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
