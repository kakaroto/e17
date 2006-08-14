#ifndef EWL_RANGE_H
#define EWL_RANGE_H

/**
 * @addtogroup Ewl_Range Ewl_Range: A Value Selector from a Range
 * Defines an Ewl_Widget with a draggable button enclosed, used to select a
 * value from a range.
 *
 * @{
 */

/**
 * @def EWL_RANGE_TYPE
 * The type name for the Ewl_Range widget
 */
#define EWL_RANGE_TYPE "range"

/**
 * The Ewl_Range provides a means to select a value from a range using a
 * draggable button.
 */
typedef struct Ewl_Range Ewl_Range;

/**
 * @def EWL_RANGE(range)
 * Typecasts a pointer to an Ewl_Range pointer.
 */
#define EWL_RANGE(range) ((Ewl_Range *) range)

/**
 * Inherits from Ewl_Container and creates internal Ewl_Widget's to provide a
 * range and a draggable area to select a value from that range.
 */
struct Ewl_Range
{
	Ewl_Container   container; /**< Inherit from Ewl_Container */
	
	double          value; /**< Currently chosen value */
	double          min_val; /**< Minimal valide value */
	double          max_val; /**< Maximal valide value */
	double          step; /**< Size of increments in the range */
	int             invert; /**< Invert the axis */
};

int             ewl_range_init(Ewl_Range *r);

void            ewl_range_value_set(Ewl_Range *r, double v);
double          ewl_range_value_get(Ewl_Range *r);

void            ewl_range_minimum_value_set(Ewl_Range *r, double minv);
double          ewl_range_minimum_value_get(Ewl_Range *r);

void            ewl_range_maximum_value_set(Ewl_Range *r, double maxv);
double          ewl_range_maximum_value_get(Ewl_Range *r);

void            ewl_range_step_set(Ewl_Range *r, double step);
double          ewl_range_step_get(Ewl_Range *r);

void            ewl_range_invert_set(Ewl_Range *r, int invert);
int             ewl_range_invert_get(Ewl_Range *r);

void            ewl_range_decrease(Ewl_Range *r);
void            ewl_range_increase(Ewl_Range *r);

/**
 * @}
 */

#endif
