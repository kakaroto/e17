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
 * @return Returns a model that is setup to work with an ecore_list
 * @brief Retrieves a model pre-initialized to work with an ecore list. This
 * will setup the fetch and count methods for you 
 */
Ewl_Model *
ewl_model_ecore_list_get(void)
{
	Ewl_Model *model;

	DENTER_FUNCTION(DLEVEL_STABLE);

	model = ewl_model_new();
	if (model)
	{
		ewl_model_fetch_set(model, ewl_model_cb_ecore_list_fetch);
		ewl_model_count_set(model, ewl_model_cb_ecore_list_count);
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

	DRETURN_INT(m->fetch, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the subfetch callback into
 * @param get: The Ewl_Model_Subfetch callback for subrows
 * @return Returns no value.
 * @brief Sets the subfetch callback into the model
 */
void
ewl_model_subfetch_set(Ewl_Model *m, Ewl_Model_Subfetch get)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("m", m);

	m->subfetch = get;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to get the subfetch callback from
 * @return Returns the Ewl_Model_Subfetch for the subfetch callback
 * @brief Gets the subfetch callback from the model
 */
Ewl_Model_Subfetch
ewl_model_subfetch_get(Ewl_Model *m)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("m", m, NULL);

	DRETURN_INT(m->subfetch, DLEVEL_STABLE);
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

	DRETURN_INT(m->sort, DLEVEL_STABLE);
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

	DRETURN_INT(m->count, DLEVEL_STABLE);
}

/**
 * @param data: The ecore_list to get the data from
 * @param row: The row to get the data from
 * @param col: UNUSED
 * @return Returns the data at the given row
 * @brief This will return the @a row data element from the list
 */
void * 
ewl_model_cb_ecore_list_fetch(void *data, unsigned int row, 
				unsigned int col __UNUSED__)
{
	Ecore_List *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, NULL);

	list = data;
	ecore_list_goto_index(list, row);

	DRETURN_PTR(ecore_list_current(list), DLEVEL_STABLE);
}

/**
 * @param data: The ecore_list to get the count from
 * @return Returns the number of elements in the list
 * @brief This will return the number of elements in the ecore_list
 */
int
ewl_model_cb_ecore_list_count(void *data)
{
	Ecore_List *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, 0);

	list = data;

	DRETURN_INT(ecore_list_nodes(list), DLEVEL_STABLE);
}

