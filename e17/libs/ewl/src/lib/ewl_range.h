/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_RANGE_H
#define EWL_RANGE_H

/**
 * @addtogroup Ewl_Range Ewl_Range: A Value Selector from a Range
 * Defines an Ewl_Widget with a draggable button enclosed, used to select a
 * value from a range.
 *
 * @remarks Inherits from Ewl_Container.
 * @if HAVE_IMAGES
 * @image html Ewl_Range_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_RANGE_TYPE
 * The type name for the Ewl_Range widget
 */
#define EWL_RANGE_TYPE "range"

/**
 * @def EWL_RANGE_IS(w)
 * Returns TRUE if the widget is an Ewl_Range, FALSE otherwise
 */
#define EWL_RANGE_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_RANGE_TYPE))

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
 * @brief Inherits from Ewl_Container and extends to provide a basic range
 * widget
 */
struct Ewl_Range
{
        Ewl_Container container; /**< Inherit from Ewl_Container */

        double value; /**< Currently chosen value */
        double min_val; /**< Minimal valide value */
        double max_val; /**< Maximal valide value */
        double step; /**< Size of increments in the range */
        unsigned int invert:1; /**< Invert the axis */
        unsigned int unknown_range:1; /**< Unknown range */
};

int              ewl_range_init(Ewl_Range *r);

void             ewl_range_value_set(Ewl_Range *r, double v);
double           ewl_range_value_get(Ewl_Range *r);

void             ewl_range_minimum_value_set(Ewl_Range *r, double minv);
double           ewl_range_minimum_value_get(Ewl_Range *r);

void             ewl_range_maximum_value_set(Ewl_Range *r, double maxv);
double           ewl_range_maximum_value_get(Ewl_Range *r);

void             ewl_range_step_set(Ewl_Range *r, double step);
double           ewl_range_step_get(Ewl_Range *r);

void             ewl_range_unknown_set(Ewl_Range *r, unsigned int unknown);
unsigned int     ewl_range_unknown_get(Ewl_Range *r);

void             ewl_range_invert_set(Ewl_Range *r, unsigned int invert);
unsigned int     ewl_range_invert_get(Ewl_Range *r);

void             ewl_range_decrease(Ewl_Range *r);
void             ewl_range_increase(Ewl_Range *r);

/**
 * @}
 */

#endif
