
#include <Ewl.h>

/**
 * @param image: the image icon to use for this menu
 * @param title: the text to place in the menu
 * @brief Create a new internal menu
 *
 * @return Returns a new menu on success, NULL on failure.
 */
Ewl_Widget     *ewl_menu_new(char *image, char *title)
{
	Ewl_Menu       *menu;

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
 * @param title: the text label to be displayed
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
	ewl_widget_set_appearance(EWL_WIDGET(menu), "menu");

	/*
	 * The realize needs to create the pop-up.
	 */
	ewl_callback_prepend(EWL_WIDGET(menu), EWL_CALLBACK_SELECT,
			    ewl_menu_expand_cb, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void ewl_menu_expand_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             x = 0, y = 0, xx = 0, yy = 0;
	Ewl_Menu       *menu;
	Ewl_Embed      *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	menu = EWL_MENU(w);
	emb = ewl_embed_find_by_widget(w);

	/*
	 * Create the popup menu portion of the menu. Do this prior to
	 * initializing the rest of the fields to avoid the add callback being
	 * called.
	 */

	menu->base.popup = ewl_window_new();
	ewl_window_set_borderless(EWL_WINDOW(menu->base.popup));
	ewl_object_set_fill_policy(EWL_OBJECT(menu->base.popup),
				   EWL_FLAG_FILL_NONE);

	/*
	 * Position the popup menu relative to the menu.
	 */
	if (EWL_MENU_ITEM(w)->inmenu) {
		xx = CURRENT_X(w) + CURRENT_W(w);
		yy = CURRENT_Y(w);
	} else {
		xx = CURRENT_X(w);
		yy = CURRENT_Y(w) + CURRENT_H(w);

		ewl_object_set_minimum_size(EWL_OBJECT(menu->base.popup),
					CURRENT_W(menu),
					MINIMUM_H(menu->base.popup));
	}

	ewl_embed_coord_to_screen(emb, xx, yy, &x, &y);
	printf("Coords %d, %d, mapped to %d, %d\n", xx, yy, x, y);

	ewl_widget_realize(EWL_WIDGET(menu->base.popup));
	ewl_window_move(EWL_WINDOW(menu->base.popup), x, y);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
