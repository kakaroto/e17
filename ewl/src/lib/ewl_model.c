/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_model.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

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
 * @param m: Model that will be destroyed
 * @return Returns no value
 * @brief Destroys the model, so its memory is returned to the system
 */
void
ewl_model_destroy(Ewl_Model *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);

        FREE(m);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns a model that is setup to work with an ecore_list
 * @brief Retrieves a model pre-initialized to work with an ecore list. This
 * will setup the fetch and count methods for you
 */
Ewl_Model *
ewl_model_ecore_list_instance(void)
{
        Ewl_Model *model;

        DENTER_FUNCTION(DLEVEL_STABLE);

        model = ewl_model_new();
        if (model)
        {
                ewl_model_data_fetch_set(model, ewl_model_cb_ecore_list_fetch);
                ewl_model_data_count_set(model, ewl_model_cb_ecore_list_count);
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
        DCHECK_PARAM_PTR_RET(model, FALSE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param m: The model to work with
 * @param get: The fetch callback to set
 * @return Returns no value
 * @brief Sets the fetch callback into the model
 */
void
ewl_model_data_header_fetch_set(Ewl_Model *m, Ewl_Model_Data_Header_Fetch get)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);

        m->header = get;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The model to work with
 * @return Returns the header fetch function
 * @brief Retrieves the header fetch function
 */
Ewl_Model_Data_Header_Fetch
ewl_model_data_header_fetch_get(const Ewl_Model *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, NULL);

        DRETURN_PTR(m->header, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the fetch callback into
 * @param get: The Ewl_Model_Fetch callback to set
 * @return Returns no value.
 * @brief Sets the fetch callback in the model
 */
void
ewl_model_data_fetch_set(Ewl_Model *m, Ewl_Model_Data_Fetch get)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);

        m->fetch = get;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to get the fetch callback from
 * @return Returns the Ewl_Model_Fetch callback or NULL if none set
 * @brief Gets the fetch callback from the model
 */
Ewl_Model_Data_Fetch
ewl_model_data_fetch_get(const Ewl_Model *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, NULL);

        DRETURN_PTR(m->fetch, DLEVEL_STABLE);
}

/**
 * @param m: the model to work with
 * @param sortable: the sortable callback to set
 * @return Returns no value
 * @brief Sets the sortable callback
 */
void
ewl_model_column_sortable_set(Ewl_Model *m, Ewl_Model_Column_Sortable sortable)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);

        m->sortable = sortable;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: the model to work with
 * @return Returns the sortable callback on the model
 * @brief Retrieves the sortable callback on the model
 */
Ewl_Model_Column_Sortable
ewl_model_column_sortable_get(const Ewl_Model *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, NULL);

        DRETURN_PTR(m->sortable, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the sort callback on
 * @param sort: The sort callback to set
 * @return Returns no value.
 * @brief Sets the sort callback into the model
 */
void
ewl_model_data_sort_set(Ewl_Model *m, Ewl_Model_Data_Sort sort)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);

        m->sort = sort;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to get the sort callback from
 * @return Returns the Ewl_Model_Sort set on the model or NULL if none set
 * @brief Gets the sort callback from the model
 */
Ewl_Model_Data_Sort
ewl_model_data_sort_get(const Ewl_Model *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, NULL);

        DRETURN_PTR(m->sort, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the callback on
 * @param highlight: The highlight callback to set on the model
 * @return Returns no value
 * @brief Sets the @a highlight callback into the model @a m
 */
void
ewl_model_data_highlight_set(Ewl_Model *m, Ewl_Model_Data_Highlight highlight)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);

        m->highlight = highlight;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The model to work with
 * @return Returns the highlight model set in the model
 * @brief Retrieves the highlight model set in @a m
 */
Ewl_Model_Data_Highlight
ewl_model_data_highlight_get(const Ewl_Model *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, NULL);

        DRETURN_PTR(m->highlight, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the count callback on
 * @param count: The count callback to set on the model
 * @return Returns no value.
 * @brief Sets the count callback into the model
 */
void
ewl_model_data_count_set(Ewl_Model *m, Ewl_Model_Data_Count count)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);

        m->count = count;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to get the count callback from
 * @return Returns the Ewl_Model_Count callback set on the model or NULL if
 * none set.
 * @brief Gets the count callback from the model
 */
Ewl_Model_Data_Count
ewl_model_data_count_get(const Ewl_Model *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, NULL);

        DRETURN_PTR(m->count, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the unref callback on
 * @param unref: The unref callback to set on the model
 * @return Returns no value.
 * @brief Sets the unref callback into the model
 */
void
ewl_model_data_unref_set(Ewl_Model *m, Ewl_Model_Data_Unref unref)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);

        m->unref = unref;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to get the unref callback from
 * @return Returns the Ewl_Model_Data_Unref callback set on the model or 
 * NULL if none set.
 * @brief Gets the unref callback from the model
 */
Ewl_Model_Data_Unref
ewl_model_data_unref_get(const Ewl_Model *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, NULL);

        DRETURN_PTR(m->unref, DLEVEL_STABLE);
}

/**
 * @param m: The model to set the callback on
 * @param exp: The expandable callback
 * @return Returns no value
 * @brief Sets the expandable callback on the given model
 */
void
ewl_model_data_expandable_set(Ewl_Model *m, Ewl_Model_Data_Expandable exp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);

        m->expansion.is = exp;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The model to get the expandable callback from
 * @return Returns the expandable callback set on the model
 * @brief Retrieves the expandable callback set on the model
 */
Ewl_Model_Data_Expandable
ewl_model_data_expandable_get(const Ewl_Model *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, NULL);

        DRETURN_PTR(m->expansion.is, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the expansion_data callback into
 * @param get: The Ewl_Model_Expansion_Data_Fetch callback for subrows
 * @return Returns no value.
 * @brief Sets the subfetch callback into the model
 */
void
ewl_model_expansion_data_fetch_set(Ewl_Model *m,
                                Ewl_Model_Expansion_Data_Fetch get)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);

        m->expansion.data = get;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to get the expansion_data callback from
 * @return Returns the Ewl_Model_Expansion_Data_Fetch for the model
 * @brief Gets the expansion_data callback from the model
 */
Ewl_Model_Expansion_Data_Fetch
ewl_model_expansion_data_fetch_get(const Ewl_Model *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, NULL);

        DRETURN_PTR(m->expansion.data, DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to set the data free function into
 * @param data_free: The Ewl_Model_Data_Free function to set
 * @return Returns no value.
 * @brief Set the data free pointer on this view
 *
 * This function is necessary to decrement intermediate reference counts or free
 * temporary data buffers passed between the allocated data and the assigned
 * widget.
 */
void
ewl_model_data_free_set(Ewl_Model *m, Ewl_Model_Data_Free data_free)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);

        m->data_free = data_free;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The Ewl_Model to get the data free function from
 * @return Returns the Ewl_Model_Data_Free set into the Ewl_Model or NULL if none
 * set.
 * @brief Get the data free pointer set on this model
 */
Ewl_Model_Data_Free
ewl_model_data_free_get(const Ewl_Model *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, NULL);

        DRETURN_PTR(m->data_free, DLEVEL_STABLE);
}

/**
 * @param m: The model to work with
 * @param f: The model expansion model fetch callback
 * @return Returns no value
 * @brief Sets the model expansion model fetch callback to @a f
 */
void
ewl_model_expansion_model_fetch_set(Ewl_Model *m,
                                Ewl_Model_Expansion_Model_Fetch f)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);

        m->expansion.model = f;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: The model to work with
 * @return Returns the model expansion model fetch function
 * @brief Retrieves the model expansion model fetch function
 */
Ewl_Model_Expansion_Model_Fetch
ewl_model_expansion_model_fetch_get(const Ewl_Model *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, NULL);

        DRETURN_PTR(m->expansion.model, DLEVEL_STABLE);
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
        DCHECK_PARAM_PTR_RET(data, NULL);

        list = data;
        ecore_list_index_goto(list, row);

        DRETURN_PTR(ecore_list_current(list), DLEVEL_STABLE);
}

/**
 * @param data: The ecore_list to get the count from
 * @return Returns the number of elements in the list
 * @brief This will return the number of elements in the ecore_list
 */
unsigned int
ewl_model_cb_ecore_list_count(void *data)
{
        Ecore_List *list;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, 0);

        list = data;

        DRETURN_INT(ecore_list_count(list), DLEVEL_STABLE);
}

