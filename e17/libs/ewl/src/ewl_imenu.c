
#include <Ewl.h>

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

	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_SELECT,
			    ewl_imenu_expand_cb, NULL);
	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_CONFIGURE,
			    ewl_imenu_configure_cb, NULL);

	/*
	 * Create the popup menu portion of the widget.
	 */
	menu->base.popup = ewl_floater_new(EWL_WIDGET(menu));
	ewl_widget_set_internal(menu->base.popup, TRUE);
	ewl_widget_set_layer(menu->base.popup, 1000);
	ewl_widget_set_appearance(EWL_WIDGET(menu->base.popup), "imenu");
	ewl_box_set_orientation(EWL_BOX(menu->base.popup),
				EWL_ORIENTATION_VERTICAL);
	ewl_object_set_fill_policy(EWL_OBJECT(menu->base.popup),
				   EWL_FLAG_FILL_NONE);
	ewl_object_set_alignment(EWL_OBJECT(menu->base.popup),
				 EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_imenu_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_IMenu *menu = EWL_IMENU(w);

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Position the popup menu relative to the menu.
	 */
	if (EWL_MENU_ITEM(w)->inmenu) {
		ewl_floater_set_position(EWL_FLOATER(menu->base.popup),
					 CURRENT_W(w), 0);
	}
	else {
		ewl_floater_set_position(EWL_FLOATER(menu->base.popup), 0,
					 CURRENT_H(w));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_imenu_expand_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_IMenu      *menu;
	Ewl_Embed      *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = EWL_IMENU(w);

	if (!REALIZED(menu->base.popup)) {
		emb = ewl_embed_find_by_widget(w);
		ewl_container_append_child(EWL_CONTAINER(emb),
					   menu->base.popup); 
	}

	ewl_widget_show(menu->base.popup);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
