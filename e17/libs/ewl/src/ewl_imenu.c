
#include <Ewl.h>

void            __ewl_imenu_expand(Ewl_Widget * w, void *ev_data,
		void *user_data);
void            __ewl_imenu_floater_destroy(Ewl_Widget * w, void *ev_data,
		void *user_data);

/**
 * @param image: the image icon to use for this menu
 * @param title: the text to place in the menu
 * @return Returns a pointer to a new menu on success, NULL on failure.
 * @brief Create a new internal menu
 */
Ewl_Widget     *ewl_imenu_new(char *image, char *title)
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
 * @param menu: the menu to initialize
 * @param image: the path to the icon image
 * @param title: the string displayed in the title
 * @return Returns no value.
 * @brief Initialize an internal menu to starting values
 */
void ewl_imenu_init(Ewl_IMenu * menu, char *image, char *title)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("menu", menu);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	ewl_menu_base_init(EWL_MENU_BASE(menu), image, title);

	ewl_callback_prepend(EWL_WIDGET(menu), EWL_CALLBACK_SELECT,
			    __ewl_imenu_expand, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void __ewl_imenu_expand(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_IMenu      *menu;
	Ewl_Embed      *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = EWL_IMENU(w);

	/*
	 * Create the popup menu portion of the menu. Do this prior to
	 * initializing the rest of the fields to avoid the add callback being
	 * called.
	 */
	menu->base.popup = ewl_floater_new(EWL_WIDGET(menu));
	ewl_callback_append(menu->base.popup, EWL_CALLBACK_DESTROY,
			__ewl_imenu_floater_destroy, menu);
	ewl_widget_set_appearance(EWL_WIDGET(menu->base.popup), "imenu");
	ewl_box_set_orientation(EWL_BOX(menu->base.popup),
			EWL_ORIENTATION_VERTICAL);
	ewl_object_set_fill_policy(EWL_OBJECT(menu->base.popup),
				   EWL_FILL_POLICY_NONE);
	ewl_object_set_alignment(EWL_OBJECT(menu->base.popup),
				 EWL_ALIGNMENT_LEFT | EWL_ALIGNMENT_TOP);

	emb = ewl_embed_find_by_widget(w);
	ewl_container_append_child(EWL_CONTAINER(emb), menu->base.popup); 

	/*
	 * Position the popup menu relative to the menu.
	 */
	if (EWL_MENU_ITEM(w)->submenu)
		ewl_floater_set_position(EWL_FLOATER(menu->base.popup),
					 CURRENT_W(w), 0);
	else {
		ewl_floater_set_position(EWL_FLOATER(menu->base.popup), 0,
					 CURRENT_H(w));
		ewl_object_set_minimum_w(EWL_OBJECT(menu->base.popup),
					     CURRENT_W(menu));
	}

	ewl_callback_del(w, EWL_CALLBACK_SELECT, __ewl_imenu_expand);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_imenu_floater_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_IMenu *menu = user_data;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (menu->base.popup == w)
		menu->base.popup = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
