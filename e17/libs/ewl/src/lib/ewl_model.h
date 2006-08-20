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
 * @def EWL_MODEL_DATA_FETCH(f)
 * Model callback to handle fetching the data at the given row/column
 */
#define EWL_MODEL_DATA_FETCH(f) ((Ewl_Model_Fetch)f)

/**
 * A typedef to shorten the definition of the model_fetch callbacks. 
 */
typedef void *(*Ewl_Model_Fetch)(void *data, unsigned int row, 
						unsigned int column);

/**
 * @def EWL_MODEL_DATA_SUBFETCH(f)
 * Model callback to handle fetching the data for a given parent at a 
 * given row/column
 */
#define EWL_MODEL_DATA_SUBFETCH(f) ((Ewl_Model_Subfetch)f)

/**
 * A typedef to shorten the definition of the model_subfetch callbacks. 
 */
typedef void *(*Ewl_Model_Subfetch)(void *data, unsigned int parent,
				unsigned int row, unsigned int column);

/**
 * @def EWL_MODEL_DATA_SORT(f)
 * Model callback to inform the program to sort it's data in the given
 * column
 */
#define EWL_MODEL_DATA_SORT(f) ((Ewl_Model_Sort)f)

/**
 * A typedef to shorten the definition of the model_sort callbacks. 
 */
typedef void (*Ewl_Model_Sort)(void *data, unsigned int column, 
						Ewl_Sort_Direction sort);

/**
 * @def EWL_MODEL_DATA_COUNT(f)
 * Model callback to have the program return the number of rows in its data
 */
#define EWL_MODEL_DATA_COUNT(f) ((Ewl_Model_Count)f)

/**
 * A typedef to shorten the definition of the model_count callbacks. 
 */
typedef int (*Ewl_Model_Count)(void *data);

/**
 * @def EWL_MODEL(model)
 * Typecasts a pointer to an Ewl_Model pointer.
 */
#define EWL_MODEL(model) ((Ewl_Model *)model)

/**
 * The Ewl_Model structure
 */
typedef struct Ewl_Model Ewl_Model;

/**
 * This holds the callbacks needed to define a model
 */
struct Ewl_Model
{
	Ewl_Model_Fetch fetch;    /**< Retrieve data for a cell */
	Ewl_Model_Subfetch subfetch; /**< Check for subdata */
	Ewl_Model_Sort sort;      /**< Trigger sort on column */
	Ewl_Model_Count count;    /**< Count of data items */
};

Ewl_Model 	*ewl_model_new(void);
int       	 ewl_model_init(Ewl_Model *model);

Ewl_Model	*ewl_model_ecore_list_get(void);

void             ewl_model_fetch_set(Ewl_Model *m, Ewl_Model_Fetch get);
Ewl_Model_Fetch  ewl_model_fetch_get(Ewl_Model *m);

void               ewl_model_subfetch_set(Ewl_Model *m, 
						Ewl_Model_Subfetch get);
Ewl_Model_Subfetch ewl_model_subfetch_get(Ewl_Model *m);

void             ewl_model_sort_set(Ewl_Model *m, Ewl_Model_Sort sort);
Ewl_Model_Sort   ewl_model_sort_get(Ewl_Model *m);

void             ewl_model_count_set(Ewl_Model *m, Ewl_Model_Count count);
Ewl_Model_Count  ewl_model_count_get(Ewl_Model *m);

/*
 * Internal stuff.
 */
void *ewl_model_cb_ecore_list_fetch(void *data, unsigned int row, 
						unsigned int col);
int ewl_model_cb_ecore_list_count(void *data);

/**
 * @}
 */

#endif

