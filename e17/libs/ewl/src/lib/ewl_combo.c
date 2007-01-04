/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_combo_cb_selected_change(Ewl_MVC *mvc);

/**
 * @return Returns a pointer to a new combo on success, NULL on failure.
 * @brief Create a new combo box
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

/**
 * @param combo: The Ewl_Combo to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes a combo to default values 
 */
int
ewl_combo_init(Ewl_Combo *combo)
{
	Ewl_Widget *o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("combo", combo, FALSE);

	if (!ewl_mvc_init(EWL_MVC(combo)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(EWL_WIDGET(combo), EWL_COMBO_TYPE);
	ewl_widget_appearance_set(EWL_WIDGET(combo), EWL_COMBO_TYPE);
	ewl_box_orientation_set(EWL_BOX(combo), EWL_ORIENTATION_HORIZONTAL);
	ewl_mvc_selected_change_cb_set(EWL_MVC(combo), 
					ewl_combo_cb_selected_change);

	combo->button = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(combo), combo->button);
	ewl_widget_internal_set(combo->button, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(combo->button), 
					EWL_FLAG_ALIGN_RIGHT);
	ewl_callback_append(combo->button, EWL_CALLBACK_CLICKED,
				ewl_combo_cb_decrement_clicked, combo);
	ewl_widget_show(combo->button);
	
	/*
	 * setup the popup
	 */
	combo->popup = ewl_popup_new();
	ewl_popup_follow_set(EWL_POPUP(combo->popup), EWL_WIDGET(combo));
	ewl_popup_type_set(EWL_POPUP(combo->popup), 
				EWL_POPUP_TYPE_MENU_VERTICAL);
	ewl_popup_fit_to_follow_set(EWL_POPUP(combo->popup), TRUE);
	ewl_window_keyboard_grab_set(EWL_WINDOW(combo->popup), TRUE);
	ewl_window_pointer_grab_set(EWL_WINDOW(combo->popup), TRUE);
	ewl_widget_appearance_set(combo->popup, EWL_COMBO_TYPE
						"/"EWL_POPUP_TYPE);
	ewl_object_fill_policy_set(EWL_OBJECT(combo->popup),
					EWL_FLAG_FILL_HFILL);
	ewl_object_alignment_set(EWL_OBJECT(combo->popup),
				EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);
	ewl_callback_append(combo->popup, EWL_CALLBACK_MOUSE_DOWN,
					ewl_combo_cb_popup_mouse_down, combo);
	
	/*
	 * setup the popbox
	 */
	o = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(combo->popup), o);
	ewl_container_redirect_set(EWL_CONTAINER(combo->popup), EWL_CONTAINER(o));
	ewl_widget_show(o);

	ewl_object_fill_policy_set(EWL_OBJECT(combo), 
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);


	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param combo: The Ewl_Combo to use
 * @param editable: Set if the combo is editable or not
 * @return Returns no value
 * @brief This will set if the displayed data in the combo is editable. 
 *  
 * If the editable flag is set to TRUE then the combo will always request
 * the header as the item to display. If it is FALSE then the combo will use
 * the selected widget as the display.
 */
void
ewl_combo_editable_set(Ewl_Combo *combo, unsigned int editable)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("combo", combo);
	DCHECK_TYPE("combo", combo, EWL_COMBO_TYPE);

	if (combo->editable == editable)
		DRETURN(DLEVEL_STABLE);

	combo->editable = !!editable;

	/* force the selected display to change */
	ewl_combo_cb_selected_change(EWL_MVC(combo));	

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param combo: The Ewl_Combo to use
 * @return Returns the editable status of the combo
 * @brief Retrieves the editable status of the combo
 */
unsigned int
ewl_combo_editable_get(Ewl_Combo *combo)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("combo", combo, FALSE);
	DCHECK_TYPE_RET("combo", combo, EWL_COMBO_TYPE, FALSE);

	DRETURN_INT(combo->editable, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: The event data
 * @param data: The combo box
 * @return Returns no value
 * @brief Callback for when the button to expand the combo is pressed
 */
void
ewl_combo_cb_decrement_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
								void *data)
{
	Ewl_Combo *combo;
	Ewl_Model *model;
	Ewl_View *view;
	void *mvc_data;
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	combo = data;
	model = ewl_mvc_model_get(EWL_MVC(combo));
	view = ewl_mvc_view_get(EWL_MVC(combo));
	mvc_data = ewl_mvc_data_get(EWL_MVC(combo));

	/* nothing to do if we have no model/view or data */
	if (!model || !view)
		DRETURN(DLEVEL_STABLE);

	/* XXX put checks to make sure all the needed module and view
	 * function callbacks are setup */

	ewl_widget_show(combo->popup);
	ewl_window_raise(EWL_WINDOW(combo->popup));
	ewl_widget_focus_send(EWL_WIDGET(combo->popup));

	ewl_widget_state_set(combo->button, "expanded",
					EWL_STATE_PERSISTENT);

	if (!ewl_mvc_dirty_get(EWL_MVC(combo)))
		DRETURN(DLEVEL_STABLE);

	ewl_container_reset(EWL_CONTAINER(combo->popup));
	for (i = 0; i < model->count(mvc_data); i++)
	{
		Ewl_Widget *cell;
		Ewl_Widget *item;

		cell = ewl_cell_new();
		ewl_object_fill_policy_set(EWL_OBJECT(cell), 
						EWL_FLAG_FILL_HFILL);
		ewl_container_child_append(EWL_CONTAINER(combo->popup), cell);
		ewl_callback_append(cell, EWL_CALLBACK_CLICKED,
					ewl_combo_cb_item_clicked, combo);
		ewl_widget_show(cell);

		item = view->construct();
		view->assign(item, model->fetch(mvc_data, i, 0));
		ewl_container_child_append(EWL_CONTAINER(cell), item);
		ewl_widget_show(item);
	}

	ewl_mvc_dirty_set(EWL_MVC(combo), FALSE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: UNUSED
 * @param data: The combo popup
 * @return Returns no value
 * @brief Callback for when the button to close the combo is clicked
 */
void
ewl_combo_cb_popup_mouse_down(Ewl_Widget *w, 
				void *ev __UNUSED__, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	if (w == ewl_embed_focused_widget_get(EWL_EMBED(w))) {
		Ewl_Combo *combo;
		
		combo = EWL_COMBO(data);
		ewl_widget_hide(combo->popup);
		ewl_widget_state_set(combo->button, "collapsed", 
					EWL_STATE_PERSISTENT);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: UNUSED
 * @param data: The combo widget
 * @return Returns no value
 * @brief Callback for when a combo item is clicked
 */
void
ewl_combo_cb_item_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
								void *data)
{
	int i;
	Ewl_Combo *combo;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	combo = data;

	i = ewl_container_child_index_get(EWL_CONTAINER(combo->popup), w);
	ewl_mvc_selected_set(EWL_MVC(combo), i, -1);

	ewl_widget_hide(combo->popup);
	ewl_widget_state_set(combo->button, "collapsed",
					EWL_STATE_PERSISTENT);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_combo_cb_selected_change(Ewl_MVC *mvc)
{
	Ewl_View *view;
	Ewl_Model *model;
	Ewl_Combo *combo;
	Ewl_Widget *item = NULL;
	void *mvc_data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	combo = EWL_COMBO(mvc);
	view = ewl_mvc_view_get(mvc);
	model = ewl_mvc_model_get(mvc);
	mvc_data = ewl_mvc_data_get(mvc);

	if (!mvc_data)
		DRETURN(DLEVEL_STABLE);

	/*
	 * if we haven't already build a cell for the header, build it
	 * now
	 */
	if (!combo->header) {
		combo->header = ewl_cell_new();
		ewl_container_child_prepend(EWL_CONTAINER(combo), 
							combo->header);
		ewl_object_fill_policy_set(EWL_OBJECT(combo->header),
							EWL_FLAG_FILL_FILL);
		ewl_widget_show(combo->header);
	}
	else
		ewl_container_reset(EWL_CONTAINER(combo->header));

	if (ewl_mvc_selected_count_get(mvc))
	{
		Ewl_Selection_Idx *idx;

		idx = ewl_mvc_selected_get(mvc);
		item = view->construct();
		view->assign(item, model->fetch(mvc_data, idx->row, 0));
	}
	else if (view->header_fetch)
		item = view->header_fetch(mvc_data, -1);

	if (item)
	{
		ewl_container_child_prepend(EWL_CONTAINER(combo->header), item);
		ewl_widget_show(item);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

