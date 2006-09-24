#include <Ewl.h>
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

/**
 * @param mvc: The MVC to initialize
 * @return Returns TRUE on success or FALSE if unsuccessful
 * @brief Initializes an MVC widget ot default values
 */
int
ewl_mvc_init(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, FALSE);

	if (!ewl_box_init(EWL_BOX(mvc)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(EWL_WIDGET(mvc), EWL_MVC_TYPE);
	ewl_box_orientation_set(EWL_BOX(mvc), EWL_ORIENTATION_VERTICAL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param view: The view to set
 * @return Returns no value
 * @brief Sets the given view onto the MVC
 */
void
ewl_mvc_view_set(Ewl_MVC *mvc, Ewl_View *view)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (mvc->view == view)
		DRETURN(DLEVEL_STABLE);

	mvc->view = view;
	if (mvc->cb.view_change)
		mvc->cb.view_change(mvc);

	ewl_mvc_dirty_set(mvc, TRUE);
		
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @return Returns the current view set on the MVC
 * @brief Retrives the current view set on the MVC
 */
Ewl_View *
ewl_mvc_view_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, NULL);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, NULL);

	DRETURN_PTR(mvc->view, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param cb: The callback to set
 * @return Returns no value
 * @brief This callback will be called whenever the ewl_mvc_view_set routine is
 * called to notify the inheriting widget that the view has changed
 */
void
ewl_mvc_view_change_cb_set(Ewl_MVC *mvc, void (*cb)(Ewl_MVC *mvc))
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	mvc->cb.view_change = cb;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param model: The model to set
 * @return Returns no value
 * @brief Sets the given model into the tree
 */
void
ewl_mvc_model_set(Ewl_MVC *mvc, Ewl_Model *model)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_PARAM_PTR("model", model);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (mvc->model == model)
		DRETURN(DLEVEL_STABLE);

	mvc->model = model;
	ewl_mvc_dirty_set(mvc, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @return Returns the current model set into the MVC widget
 * @brief Retrieves the model set into the MVC widget
 */
Ewl_Model * 
ewl_mvc_model_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, NULL);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, NULL);

	DRETURN_PTR(mvc->model, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param data: The data to set on the MVC
 * @return Returns no value
 * @brief Sets the given data @a data into the MVC widget @a mvc
 */
void
ewl_mvc_data_set(Ewl_MVC *mvc, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	mvc->data = data;
	ewl_mvc_dirty_set(mvc, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @return Returns the data set onto the MVC widget
 * @brief Retrieves the data set into the MVC widget
 */
void *
ewl_mvc_data_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, NULL);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, NULL);

	DRETURN_PTR(mvc->data, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC widget to work with
 * @param dirty: The dirty status to set
 * @return Returns no value.
 * @brief Sets the dirty status of the MVC widget @a mvc to the @dirty state
 */
void
ewl_mvc_dirty_set(Ewl_MVC *mvc, unsigned int dirty)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (mvc->dirty == dirty)
		DRETURN(DLEVEL_STABLE);

	mvc->dirty = !!dirty;
	ewl_widget_configure(EWL_WIDGET(mvc));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC widget use
 * @return Returns the dirty status of the MVC widget
 * @brief Retrieves the dirty status of the MVC widget
 */
unsigned int
ewl_mvc_dirty_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, FALSE);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, FALSE);

	DRETURN_INT(mvc->dirty, DLEVEL_STABLE);
}

