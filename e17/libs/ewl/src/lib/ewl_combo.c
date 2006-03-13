#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

void ewl_combo_cb_item_clicked(Ewl_Widget *w, void *ev, void *data);

/**
 * @param title: the text to place in the combo
 * @return Returns a pointer to a new combo on success, NULL on failure.
 * @brief Create a new internal combo
 */
Ewl_Widget *
ewl_combo_new(void)
{
	Ewl_Combo *combo;

	DENTER_FUNCTION(DLEVEL_STABLE);

	combo = NEW(Ewl_Combo, 1);
	if (!combo)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_combo_init(combo)) 
	{
		ewl_widget_destroy(EWL_WIDGET(combo));
		combo = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(combo), DLEVEL_STABLE);
}

int
ewl_combo_init(Ewl_Combo *combo)
{

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("combo", combo, FALSE);

	if (!ewl_box_init(EWL_BOX(combo)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(EWL_WIDGET(combo), EWL_COMBO_TYPE);
	ewl_widget_appearance_set(EWL_WIDGET(combo), EWL_COMBO_TYPE);

	ewl_box_orientation_set(EWL_BOX(combo), EWL_ORIENTATION_HORIZONTAL);

	combo->button = ewl_button_new();
	ewl_widget_appearance_set(combo->button, "decrement");
	ewl_widget_internal_set(combo->button, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(combo->button), 
					EWL_FLAG_ALIGN_RIGHT);
	ewl_container_child_append(EWL_CONTAINER(combo), combo->button);
	ewl_object_fill_policy_set(EWL_OBJECT(combo->button), 
					EWL_FLAG_FILL_NONE);
	ewl_callback_append(combo->button, EWL_CALLBACK_CLICKED,
				ewl_combo_cb_decrement_clicked, combo);
	ewl_widget_show(combo->button);

	combo->popup = NEW(Ewl_Menu_Base, 1);
	ewl_menu_base_init(EWL_MENU_BASE(combo->popup));
	ewl_widget_internal_set(EWL_WIDGET(combo->popup), TRUE);

	combo->popup->popup = ewl_floater_new();
	ewl_floater_follow_set(EWL_FLOATER(combo->popup->popup),
						EWL_WIDGET(combo));
	ewl_widget_internal_set(combo->popup->popup, TRUE);
	ewl_widget_layer_set(combo->popup->popup, 1000);
	ewl_widget_appearance_set(combo->popup->popup, EWL_IMENU_TYPE);
	ewl_box_orientation_set(EWL_BOX(combo->popup->popup),
						EWL_ORIENTATION_VERTICAL);
	ewl_object_fill_policy_set(EWL_OBJECT(combo->popup->popup),
						EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(combo->popup->popup),
				EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);

	/* default this to -2 so that when we first show we will set it to -1 */
	combo->selected_idx = -2; 
	ewl_callback_append(EWL_WIDGET(combo), EWL_CALLBACK_CONFIGURE,
					ewl_combo_cb_configure, NULL);
	ewl_object_fill_policy_set(EWL_OBJECT(combo), EWL_FLAG_FILL_NONE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_combo_selected_set(Ewl_Combo *combo, int idx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("combo", combo);
	DCHECK_TYPE("combo", combo, EWL_COMBO_TYPE);

	if (combo->selected_idx == idx)
		DRETURN(DLEVEL_STABLE);

	combo->selected_idx = idx;

	/* remove the previously selected value */
	if (combo->selected)
	{
		ewl_container_child_remove(EWL_CONTAINER(combo), 
						combo->selected);
		ewl_widget_destroy(combo->selected);
	}

	/* if we have a selected value then show it in the top, else show
	 * the header */
	if (idx > -1)
	{
		combo->selected = combo->view->construct();
		combo->view->assign(combo->selected, 
				combo->model->fetch(combo->data, idx, 0));
		ewl_widget_show(combo->selected);
	}
	else
		combo->selected = combo->model->header_fetch(combo->data, 0);	

	ewl_container_child_prepend(EWL_CONTAINER(combo), combo->selected);
						
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int 
ewl_combo_selected_get(Ewl_Combo *combo)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("combo", combo, -1);
	DCHECK_TYPE_RET("combo", combo, EWL_COMBO_TYPE, -1);

	DRETURN_INT(combo->selected_idx, DLEVEL_STABLE);
}

void
ewl_combo_model_set(Ewl_Combo *combo, Ewl_Model *model)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("combo", combo);
	DCHECK_PARAM_PTR("model", model);
	DCHECK_TYPE("combo", combo, EWL_COMBO_TYPE);

	combo->model = model;
	ewl_combo_dirty_set(combo, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Model *
ewl_combo_model_get(Ewl_Combo *combo)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("combo", combo, NULL);
	DCHECK_TYPE_RET("combo", combo, EWL_COMBO_TYPE, NULL);

	DRETURN_PTR(combo->model, DLEVEL_STABLE);
}

void
ewl_combo_view_set(Ewl_Combo *combo, Ewl_View *view)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("combo", combo);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("combo", combo, EWL_COMBO_TYPE);

	combo->view = view;
	ewl_combo_dirty_set(combo, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_View *
ewl_combo_view_get(Ewl_Combo *combo)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("combo", combo, NULL);
	DCHECK_TYPE_RET("combo", combo, EWL_COMBO_TYPE, NULL);

	DRETURN_PTR(combo->view, DLEVEL_STABLE);
}

void
ewl_combo_data_set(Ewl_Combo *combo, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("combo", combo);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("combo", combo, EWL_COMBO_TYPE);

	combo->data = data;
	ewl_combo_dirty_set(combo, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void *
ewl_combo_data_get(Ewl_Combo *combo)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("combo", combo, NULL);
	DCHECK_TYPE_RET("combo", combo, EWL_COMBO_TYPE, NULL);

	DRETURN_PTR(combo->data, DLEVEL_STABLE);
}

void
ewl_combo_dirty_set(Ewl_Combo *combo, unsigned int dirty)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("combo", combo);
	DCHECK_TYPE("combo", combo, EWL_COMBO_TYPE);

	combo->dirty = !!dirty;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_combo_dirty_get(Ewl_Combo *combo)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("combo", combo, FALSE);
	DCHECK_TYPE_RET("combo", combo, EWL_COMBO_TYPE, FALSE);

	DRETURN_INT(combo->dirty, DLEVEL_STABLE);
}

void
ewl_combo_cb_configure(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Combo *combo;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	combo = EWL_COMBO(w);

	if (EWL_MENU_ITEM(combo->popup)->inmenu)
		ewl_floater_position_set(EWL_FLOATER(combo->popup->popup),
						CURRENT_W(w), 0);
	else
		ewl_floater_position_set(EWL_FLOATER(combo->popup->popup),
						0, CURRENT_H(w));

	if (combo->selected_idx < -1)
		ewl_combo_selected_set(combo, -1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_combo_cb_decrement_clicked(Ewl_Widget *w __UNUSED__, void *ev, void *data)
{
	Ewl_Combo *combo;
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	combo = data;

	/* nothing to do if we have no model/view or data */
	if ((!combo->model) || (!combo->view) || (!combo->data))
		DRETURN(DLEVEL_STABLE);

	/* change hte button appearance and expand the menu */
	ewl_widget_appearance_set(combo->button, "increment");
	ewl_callback_del(combo->button, EWL_CALLBACK_CLICKED, 
					ewl_combo_cb_decrement_clicked);
	ewl_callback_append(combo->button, EWL_CALLBACK_CLICKED,
				ewl_combo_cb_increment_clicked, combo);

	ewl_menu_base_expand_cb(EWL_WIDGET(combo->popup), ev, NULL);
	if (!REALIZED(combo->popup->popup))
	{
		Ewl_Embed *emb;
		emb = ewl_embed_widget_find(EWL_WIDGET(combo));
		ewl_container_child_append(EWL_CONTAINER(emb),
						combo->popup->popup);
	}

	if (!combo->dirty)
		DRETURN(DLEVEL_STABLE);

	ewl_container_reset(EWL_CONTAINER(combo->popup));
	for (i = 0; i < combo->model->count(combo->data); i++)
	{
		Ewl_Widget *item;

		item = combo->view->construct();
		combo->view->assign(item, 
				combo->model->fetch(combo->data, i, 0));
		ewl_container_child_append(EWL_CONTAINER(combo->popup), item);
		ewl_widget_data_set(item, "ewl_combo_position", (int *)i);
		ewl_callback_append(item, EWL_CALLBACK_CLICKED,
					ewl_combo_cb_item_clicked, combo);
		ewl_widget_show(item);
	}

	combo->dirty = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_combo_cb_increment_clicked(Ewl_Widget *w __UNUSED__, 
				void *ev __UNUSED__, void *data)
{
	Ewl_Combo *combo;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	combo = data;
	ewl_widget_hide(combo->popup->popup);

	ewl_widget_appearance_set(combo->button, "decrement");
	ewl_callback_del(combo->button, EWL_CALLBACK_CLICKED,
					ewl_combo_cb_increment_clicked);
	ewl_callback_append(combo->button, EWL_CALLBACK_CLICKED,
				ewl_combo_cb_decrement_clicked, combo);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_combo_cb_item_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
								void *data)
{
	int i;
	Ewl_Combo *combo;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	combo = data;

	i = (int)ewl_widget_data_get(w, "ewl_combo_position");
	ewl_combo_selected_set(combo, i);
	ewl_combo_cb_increment_clicked(NULL, NULL, data);

	ewl_callback_call(EWL_WIDGET(combo), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

