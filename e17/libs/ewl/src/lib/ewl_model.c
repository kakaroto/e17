#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns a new Ewl_Model on success or NULL on failure
 * @brief Creates a new Ewl_Model object.
 */
Ewl_Model *
ewl_model_new(void)
{
	Ewl_Model *model;

	DENTER_FUNCTION(DLEVEL_STABLE);

	model = NEW(Ewl_Model, 1);
	if (!ewl_model_init(model))
	{
		FREE(model);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(model, DLEVEL_STABLE);
}

/**
 * @param model: The Ewl_Model to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Set the model to the default values
 */
int
ewl_model_init(Ewl_Model *model)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("model", model, FALSE);

	/* we're dirty by default */
	model->dirty = TRUE;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the fetch callback into
 * @param get: The Ewl_Model_Fetch callback to set
 * @return Returns no value.
 * @brief Sets the fetch callback in the model
 */
void
ewl_model_fetch_set(Ewl_Model *m, Ewl_Model_Fetch get)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("m", m);

	m->fetch = get;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to get the fetch callback from
 * @return Returns the Ewl_Model_Fetch callback or NULL if none set
 * @brief Gets the fetch callback from the model
 */
Ewl_Model_Fetch
ewl_model_fetch_get(Ewl_Model *m)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("m", m, NULL);

	DRETURN_PTR(m->fetch, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the subfetch callback into
 * @param get: The Ewl_Model_Fetch callback for subrows
 * @return Returns no value.
 * @brief Sets the subfetch callback into the model
 */
void
ewl_model_subfetch_set(Ewl_Model *m, Ewl_Model_Fetch get)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("m", m);

	m->subfetch = get;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to get the subfetch callback from
 * @return Returns the Ewl_Model_Fetch for the subfetch callback
 * @brief Gets the subfetch callback from the model
 */
Ewl_Model_Fetch
ewl_model_subfetch_get(Ewl_Model *m)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("m", m, NULL);

	DRETURN_PTR(m->subfetch, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the sort callback on
 * @param sort: The sort callback to set
 * @return Returns no value.
 * @brief Sets the sort callback into the model
 */
void
ewl_model_sort_set(Ewl_Model *m, Ewl_Model_Sort sort)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("m", m);

	m->sort = sort;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to get the sort callback from
 * @return Returns the Ewl_Model_Sort set on the model or NULL if none set
 * @brief Gets the sort callback from the model
 */
Ewl_Model_Sort
ewl_model_sort_get(Ewl_Model *m)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("m", m, NULL);

	DRETURN_PTR(m->sort, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the count callback on
 * @param count: The count callback to set on the model
 * @return Returns no value.
 * @brief Sets the count callback into the model
 */
void
ewl_model_count_set(Ewl_Model *m, Ewl_Model_Count count)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("m", m);

	m->count = count;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to get the count callback from
 * @return Returns the Ewl_Model_Count callback set on the model or NULL if
 * none set.
 * @brief Gets the count callback from the model
 */
Ewl_Model_Count
ewl_model_count_get(Ewl_Model *m)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("m", m, NULL);

	DRETURN_PTR(m->count, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the header_fetch callback on
 * @param f: The Ewl_Model_Header_Fetch callback
 * @return Returns no value.
 * @brief Sets the header fetch callback into the model
 */
void
ewl_model_header_fetch_set(Ewl_Model *m, Ewl_Model_Header_Fetch f)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("m", m);

	m->header_fetch = f;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to get the Ewl_Model_Header_Fetch function from
 * @return Returns the Ewl_Model_Header_Fetch callback set on the model, or
 * NULL on failure.
 * @brief Gets the header fetch callback from the model
 */
Ewl_Model_Header_Fetch 
ewl_model_header_fetch_get(Ewl_Model *m)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("m", m, NULL);

	DRETURN_PTR(m->header_fetch, DLEVEL_STABLE);
}

/**
 * @param m: The model to set dirty
 * @param dirty: The dirty value to set
 * @return Returns no value
 * @brief Sets if the model is dirty
 */
void
ewl_model_dirty_set(Ewl_Model *m, unsigned int dirty)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("m", m);

	m->dirty = !!dirty;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The model to get the dirty value of
 * @return Returns TRUE if the model is dirty, FALSE otherwise
 * @brief Gets if the model is dirty
 */
unsigned int
ewl_model_dirty_get(Ewl_Model *m)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("m", m, FALSE);

	DRETURN_INT(m->dirty, DLEVEL_STABLE);
}

