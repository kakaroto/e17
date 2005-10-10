#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns a pointer to a new menu on success, NULL on failure.
 * @brief Create a new internal menu
 */
Ewl_Widget *ewl_imenu_new(void)
{
	Ewl_IMenu      *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = NEW(Ewl_IMenu, 1);
	if (!menu)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_imenu_init(menu);

	DRETURN_PTR(EWL_WIDGET(menu), DLEVEL_STABLE);
}


/**
 * @param menu: the menu to initialize
 * @return Returns no value.
 * @brief Initialize an internal menu to starting values
 */
void ewl_imenu_init(Ewl_IMenu * menu)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("menu", menu);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	ewl_menu_base_init(EWL_MENU_BASE(menu));
	ewl_widget_inherit(EWL_WIDGET(menu), "imenu");

	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_SELECT,
			    ewl_imenu_expand_cb, NULL);
	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_CONFIGURE,
			    ewl_imenu_configure_cb, NULL);

	/*
	 * Create the popup menu portion of the widget.
	 */
	menu->base.popup = ewl_floater_new();
	ewl_floater_follow_set(EWL_FLOATER(menu->base.popup), EWL_WIDGET(menu));
	ewl_widget_internal_set(menu->base.popup, TRUE);
	ewl_widget_layer_set(menu->base.popup, 1000);
	ewl_widget_appearance_set(EWL_WIDGET(menu->base.popup), "imenu");
	ewl_box_orientation_set(EWL_BOX(menu->base.popup),
				EWL_ORIENTATION_VERTICAL);
	ewl_object_fill_policy_set(EWL_OBJECT(menu->base.popup),
				   EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(menu->base.popup),
				 EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_imenu_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_IMenu *menu = EWL_IMENU(w);

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Position the popup menu relative to the menu.
	 */
	if (EWL_MENU_ITEM(w)->inmenu) {
		ewl_floater_position_set(EWL_FLOATER(menu->base.popup),
				 ewl_object_current_w_get(EWL_OBJECT(w)), 0);
	}
	else {
		ewl_floater_position_set(EWL_FLOATER(menu->base.popup), 0,
				 ewl_object_current_h_get(EWL_OBJECT(w)));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_imenu_expand_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_IMenu      *menu;
	Ewl_Embed      *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = EWL_IMENU(w);

	if (!REALIZED(menu->base.popup)) {
		emb = ewl_embed_widget_find(w);
		ewl_container_child_append(EWL_CONTAINER(emb),
					   menu->base.popup); 
	}

	ewl_widget_show(menu->base.popup);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
