
#include <Ewl.h>

void            __expand_menu(Ewl_Widget * w, void *ev_data, void *user_data);

void            __item_clicked(Ewl_Widget * w, void *ev_data, void *user_data);

void            __ewl_menu_realize(Ewl_Widget * w, void *ev_data,
				   void *user_data);


void            __expand_menu_animator(int val, void *ev_data);
void            __collapse_menu_animator(int val, void *ev_data);
void            __expand_menu_and_title_animator(int val, void *ev_data);
void            __collapse_menu_and_title_animator(int val, void *ev_data);

void            __ewl_menu_start(Ewl_Widget * w);

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

	ZERO(menu, Ewl_Menu, 1);

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
			    __expand_menu, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void __expand_menu(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             x = 0, y = 0;
	Ewl_Menu       *menu;
	Ewl_Embed      *emb;

	menu = EWL_MENU(w);
	emb = ewl_embed_find_by_widget(w);

	/*
	 * Create the popup menu portion of the menu. Do this prior to
	 * initializing the rest of the fields to avoid the add callback being
	 * called.
	 */
	menu->popup = ewl_window_new();
	ewl_window_set_borderless(EWL_WINDOW(menu->popup));
	ewl_object_set_fill_policy(EWL_OBJECT(menu->popup),
				   EWL_FILL_POLICY_NONE);

	/* FIXME: We will need a real function for doing this from the embed
	 * perspective.
	ewl_window_get_position(emb, &x, &y);
	*/

	/*
	 * Position the popup menu relative to the menu.
	 */
	if (EWL_MENU_ITEM(w)->submenu) {
		x += CURRENT_X(w) + CURRENT_W(w);
		y += CURRENT_Y(w);
	} else {
		x += CURRENT_X(w);
		y += CURRENT_Y(w) + CURRENT_H(w);

		ewl_object_set_minimum_size(EWL_OBJECT(menu->popup),
					CURRENT_W(menu),
					MINIMUM_H(menu->popup));
	}

	ewl_widget_realize(EWL_WIDGET(menu->popup));
	ewl_window_move(EWL_WINDOW(menu->popup), x, y);
}
