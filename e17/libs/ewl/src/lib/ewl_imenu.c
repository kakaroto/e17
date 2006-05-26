#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns a pointer to a new menu on success, NULL on failure.
 * @brief Create a new internal menu
 */
Ewl_Widget *
ewl_imenu_new(void)
{
	Ewl_Imenu *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = NEW(Ewl_Imenu, 1);
	if (!menu)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_imenu_init(menu)) {
		ewl_widget_destroy(EWL_WIDGET(menu));
		menu = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(menu), DLEVEL_STABLE);
}

/**
 * @param menu: the menu to initialize
 * @return Returns no value.
 * @brief Initialize an internal menu to starting values
 */
int
ewl_imenu_init(Ewl_Imenu *menu)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("menu", menu, FALSE);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	if (!ewl_menu_base_init(EWL_MENU_BASE(menu)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(EWL_WIDGET(menu), EWL_IMENU_TYPE);

	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_FOCUS_IN,
			    ewl_imenu_expand_cb, NULL);
	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_CONFIGURE,
			    ewl_imenu_configure_cb, NULL);

	/*
	 * Create the popup menu portion of the widget.
	 */
	menu->base.popup = ewl_floater_new();
	ewl_floater_follow_set(EWL_FLOATER(menu->base.popup), EWL_WIDGET(menu));
	ewl_widget_internal_set(menu->base.popup, TRUE);
	ewl_widget_layer_top_set(menu->base.popup, TRUE);
	ewl_widget_appearance_set(EWL_WIDGET(menu->base.popup), EWL_IMENU_TYPE);
	ewl_box_orientation_set(EWL_BOX(menu->base.popup),
				EWL_ORIENTATION_VERTICAL);
	ewl_object_fill_policy_set(EWL_OBJECT(menu->base.popup),
				   EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(menu->base.popup),
				 EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);

	ewl_callback_prepend(EWL_WIDGET(menu), EWL_CALLBACK_DESTROY,
				ewl_imenu_destroy_cb, NULL);
	ewl_callback_prepend(menu->base.popup, EWL_CALLBACK_DESTROY,
				ewl_imenu_popup_destroy_cb, menu);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_imenu_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Imenu *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	menu = EWL_IMENU(w);
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

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The expand callback
 */
void
ewl_imenu_expand_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Imenu *menu;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	menu = EWL_IMENU(w);
	if (!REALIZED(menu->base.popup)) {
		emb = ewl_embed_widget_find(w);
		ewl_container_child_append(EWL_CONTAINER(emb),
					   menu->base.popup); 
	}
	ewl_widget_show(menu->base.popup);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: UNUSED
 * @param data: The Ewl_Imenu
 * @return Returns no value
 * @brief The popup destroy callback
 */
void
ewl_imenu_popup_destroy_cb(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
				void *data)
{
	Ewl_Imenu *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	menu = data;
	if (menu->base.popup) menu->base.popup = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_imenu_destroy_cb(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Imenu *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	menu = EWL_IMENU(w);
	if (menu->base.popup)
		ewl_callback_del(menu->base.popup, EWL_CALLBACK_DESTROY, 
						ewl_imenu_popup_destroy_cb);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

