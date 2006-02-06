#ifndef EWL_MODEL_H
#define EWL_MODEL_H

/**
 * @file ewl_model.h
 * @defgroup Ewl_Model Model: A data model
 * @brief Defines communication callbacks for views and controllers. 
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
typedef void *(*Ewl_Model_Fetch)(void *data, unsigned int row, 
						unsigned int column);

/**
 * @def EWL_MODEL_DATA_SORT(f)
 * Model callback to inform the program to sort it's data in the given
 * column
 */
#define EWL_MODEL_DATA_SORT(f) ((Ewl_Model_Sort)f)
typedef void (*Ewl_Model_Sort)(void *data, unsigned int column);

/**
 * @def EWL_MODEL_DATA_COUNT(f)
 * Model callback to have the program return the number of rows in its data
 */
#define EWL_MODEL_DATA_COUNT(f) ((Ewl_Model_Count)f)
typedef int (*Ewl_Model_Count)(void *data);

/**
 * @def EWL_MODEL_DATA_HEADER_GET(f)
 * Model callback to get the header for a given column
 */
#define EWL_MODEL_DATA_HEADER_GET(f) ((Ewl_Model_Header_Fetch)f)
typedef Ewl_Widget *(*Ewl_Model_Header_Fetch)(void *data, unsigned int column);

/**
 * @def EWL_MODEL(model)
 * Typecasts a pointer to an Ewl_Model pointer.
 */
#define EWL_MODEL(model) ((Ewl_Model *)model)
typedef struct Ewl_Model Ewl_Model;

struct Ewl_Model
{
	Ewl_Model_Fetch fetch;    /**< Retrieve data for a cell */
	Ewl_Model_Fetch subfetch; /**< Check for subdata */
	Ewl_Model_Sort sort;      /**< Trigger sort on column */
	Ewl_Model_Count count;    /**< Count of data items */
	Ewl_Model_Header_Fetch header_fetch; /**< Get the header for the given column */
};

Ewl_Model 	*ewl_model_new(void);
int       	 ewl_model_init(Ewl_Model *model);

void             ewl_model_fetch_set(Ewl_Model *m, Ewl_Model_Fetch get);
Ewl_Model_Fetch  ewl_model_fetch_get(Ewl_Model *m);

void             ewl_model_subfetch_set(Ewl_Model *m, Ewl_Model_Fetch get);
Ewl_Model_Fetch  ewl_model_subfetch_get(Ewl_Model *m);

void             ewl_model_sort_set(Ewl_Model *m, Ewl_Model_Sort sort);
Ewl_Model_Sort   ewl_model_sort_get(Ewl_Model *m);

void             ewl_model_count_set(Ewl_Model *m, Ewl_Model_Count count);
Ewl_Model_Count  ewl_model_count_get(Ewl_Model *m);

void		 ewl_model_header_fetch_set(Ewl_Model *m, Ewl_Model_Header_Fetch f);
Ewl_Model_Header_Fetch ewl_model_header_fetch_get(Ewl_Model *m);

/**
 * @}
 */

#endif

