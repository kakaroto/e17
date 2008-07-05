/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_MODEL_H
#define EWL_MODEL_H

/**
 * @addtogroup Ewl_Model Ewl_Model: A data model
 * Defines communication callbacks for views and controllers.
 * Query row/column data, indicate expansion points, notify views and
 * controllers of changes, trigger sorting on a row/column combination.
 *
 * @{
 */

/**
 * The Ewl_Model structure
 */
typedef struct Ewl_Model Ewl_Model;

/**
 * @def EWL_MODEL_DATA_FETCH(f)
 * Model callback to handle fetching the data at the given row/column
 */
#define EWL_MODEL_DATA_FETCH(f) ((Ewl_Model_Data_Fetch)f)

/**
 * A typedef to shorten the definition of the model_fetch callbacks.
 */
typedef void *(*Ewl_Model_Data_Fetch)(void *data, unsigned int row,
                                                unsigned int column);

/**
 * @def EWL_MODEL_FREE(f)
 * Model callback to free data after passing it to be assigned in the view.
 */
#define EWL_MODEL_FREE(f) ((Ewl_Model_Data_Free)f)

/**
 * A typedef to shorten the definition of the model_data_free callbacks.
 */
typedef void (*Ewl_Model_Data_Free)(void *data);

/**
 * @def EWL_MODEL_DATA_EXPANDABLE(f)
 * Model callback to handle fetching the expandable flag for a given row of
 * the tree
 */
#define EWL_MODEL_DATA_EXPANDABLE(f) ((Ewl_Model_Data_Expandable)f)

/**
 * A typedef to shorten the definition of the model_expandable callback
 */
typedef int (*Ewl_Model_Data_Expandable)(void *data, unsigned int row);

/**
 * @def EWL_MODEL_EXPANSION_DATA_FETCH(f)
 * Model callback to get the expansion data for a specific row
 */
#define EWL_MODEL_EXPANSION_DATA_FETCH(f) ((Ewl_Model_Expansion_Data_Fetch)f)

/**
 * A typedef to shorten the definition of the model_expansion_data callback
 */
typedef void *(*Ewl_Model_Expansion_Data_Fetch)(void *data, unsigned int row);

/**
 * @def EWL_MODEL_DATA_HEADER_FETCH(f)
 * Model callback to get the header data for a specific column
 */
#define EWL_MODEL_DATA_HEADER_FETCH(f) ((Ewl_Model_Data_Header_Fetch)f)

/**
 * A typedef to shorten the definition of the model_data_header_fetch callback
 */
typedef void *(*Ewl_Model_Data_Header_Fetch)(void *data, unsigned int col);

/**
 * @def EWL_MODEL_EXPANSION_MODEL_FETCH(f)
 * Model callback to get the model to use for the expansion point
 */
#define EWL_MODEL_EXPANSION_MODEL_FETCH(f) ((Ewl_Model_Expansion_Model_Fetch)(f)

/**
 * A typedef to shorten the definition of the model_expansion_model_fetch
 * callback
 */
typedef Ewl_Model *(*Ewl_Model_Expansion_Model_Fetch)(void *data,
                                                unsigned int row);

/**
 * @def EWL_MODEL_DATA_SORT(f)
 * Model callback to inform the program to sort it's data in the given
 * column
 */
#define EWL_MODEL_DATA_SORT(f) ((Ewl_Model_Data_Sort)f)

/**
 * A typedef to shorten the definition of the model_sort callbacks.
 */
typedef void (*Ewl_Model_Data_Sort)(void *data, unsigned int column,
                                                Ewl_Sort_Direction sort);

/**
 * @def EWL_MODEL_DATA_COUNT(f)
 * Model callback to have the program return the number of rows in its data
 */
#define EWL_MODEL_DATA_COUNT(f) ((Ewl_Model_Data_Count)f)

/**
 * A typedef to shorten the definition of the model_count callbacks.
 */
typedef unsigned int (*Ewl_Model_Data_Count)(void *data);

/**
 * @def EWL_MODEL_DATA_UNREF(f)
 * Model callback to unreference the data used by the model. This signalize
 * that the data is not used later from the model nor the controller
 */
#define EWL_MODEL_DATA_UNREF(f) ((Ewl_Model_Data_Unref)f)

/**
 * A typedef to shorten the definition of the model_data_unref callbacks.
 */
typedef unsigned int (*Ewl_Model_Data_Unref)(void *data);

/**
 * @def EWL_MODEL_COLUMN_SORTABLE(f)
 * Model callback to check if a columns data is sortable
 */
#define EWL_MODEL_COLUMN_SORTABLE(f) ((Ewl_Model_Column_Sortable)f)

/**
 * A typedef to shorten the definition of the model_column_sortable callbacks.
 */
typedef int (*Ewl_Model_Column_Sortable)(void *data, unsigned int col);

/**
 * @def EWL_MODEL_DATA_HIGHLIGHT(f)
 * Model callback to check if a cell is to be highlighted
 */
#define EWL_MODEL_DATA_HIGHLIGHT(f) ((Ewl_Model_Data_Highlight)f)

/**
 * A typedef to shorten the definition of the data_highlight callbacks.
 */
typedef unsigned int (*Ewl_Model_Data_Highlight)(void *data, unsigned int row);


/**
 * @def EWL_MODEL(model)
 * Typecasts a pointer to an Ewl_Model pointer.
 */
#define EWL_MODEL(model) ((Ewl_Model *)model)

/**
 * @brief Holds the callbacks needed to define a model
 */
struct Ewl_Model
{
        struct
        {
                Ewl_Model_Data_Expandable is; /**< Is the row expandable */
                Ewl_Model_Expansion_Data_Fetch data; /**< Get expansion data */
                Ewl_Model_Expansion_Model_Fetch model; /**< Get expansion model */
        } expansion;

        Ewl_Model_Column_Sortable sortable; /**< Is a column sortable */

        Ewl_Model_Data_Header_Fetch header; /**< Retrieve header data */
        Ewl_Model_Data_Fetch fetch;    /**< Retrieve data for a cell */
        Ewl_Model_Data_Free data_free; /**< Free data passed to view */
        Ewl_Model_Data_Count count;    /**< Count of data items */
        Ewl_Model_Data_Unref unref;    /**< Unreference the data */
        Ewl_Model_Data_Sort sort;      /**< Trigger sort on column */
        Ewl_Model_Data_Highlight highlight; /**< Highlight the cell */
};

Ewl_Model                       *ewl_model_new(void);
int                              ewl_model_init(Ewl_Model *model);

Ewl_Model                       *ewl_model_ecore_list_instance(void);

void                             ewl_model_data_fetch_set(Ewl_Model *m,
                                        Ewl_Model_Data_Fetch get);
Ewl_Model_Data_Fetch             ewl_model_data_fetch_get(const Ewl_Model *m);

void                             ewl_model_data_header_fetch_set(Ewl_Model *m,
                                        Ewl_Model_Data_Header_Fetch get);
Ewl_Model_Data_Header_Fetch      ewl_model_data_header_fetch_get
                                        (const Ewl_Model *m);

void                             ewl_model_data_free_set(Ewl_Model *m,
                                        Ewl_Model_Data_Free assign);
Ewl_Model_Data_Free              ewl_model_data_free_get(const Ewl_Model *m);

void                             ewl_model_column_sortable_set(Ewl_Model *m,
                                        Ewl_Model_Column_Sortable sortable);
Ewl_Model_Column_Sortable        ewl_model_column_sortable_get
                                        (const Ewl_Model *m);

void                             ewl_model_data_sort_set(Ewl_Model *m,
                                        Ewl_Model_Data_Sort sort);
Ewl_Model_Data_Sort              ewl_model_data_sort_get(const Ewl_Model *m);

void                             ewl_model_data_highlight_set(Ewl_Model *m,
                                        Ewl_Model_Data_Highlight highlight);
Ewl_Model_Data_Highlight         ewl_model_data_highlight_get
                                        (const Ewl_Model *m);

void                             ewl_model_data_count_set(Ewl_Model *m,
                                        Ewl_Model_Data_Count count);
Ewl_Model_Data_Count             ewl_model_data_count_get(const Ewl_Model *m);

void                             ewl_model_data_unref_set(Ewl_Model *m,
                                        Ewl_Model_Data_Unref unref);
Ewl_Model_Data_Unref             ewl_model_data_unref_get(const Ewl_Model *m);

void                             ewl_model_data_expandable_set(Ewl_Model *m,
                                        Ewl_Model_Data_Expandable exp);
Ewl_Model_Data_Expandable        ewl_model_data_expandable_get
                                        (const Ewl_Model *m);

void                             ewl_model_expansion_data_fetch_set(Ewl_Model *m,
                                        Ewl_Model_Expansion_Data_Fetch get);
Ewl_Model_Expansion_Data_Fetch   ewl_model_expansion_data_fetch_get
                                        (const Ewl_Model *m);

void                             ewl_model_expansion_model_fetch_set(Ewl_Model *m,
                                        Ewl_Model_Expansion_Model_Fetch f);
Ewl_Model_Expansion_Model_Fetch  ewl_model_expansion_model_fetch_get
                                        (const Ewl_Model *m);

/*
 * Internal stuff.
 */
void *ewl_model_cb_ecore_list_fetch(void *data, unsigned int row,
                                                unsigned int col);
unsigned int ewl_model_cb_ecore_list_count(void *data);

/**
 * @}
 */

#endif

