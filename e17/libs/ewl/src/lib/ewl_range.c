/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_range.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"


/**
 * @param r: the range to be initialized
 * @return Returns no value.
 * @brief Initialize the range to some sane starting values
 *
 * @note There is no ewl_range_new(), becasue range only serves
 * as a common API for seeker, spinner and progressbar
 */
int
ewl_range_init(Ewl_Range *r)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(r, FALSE);

        w = EWL_WIDGET(r);

        if (!ewl_container_init(EWL_CONTAINER(w)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(w, EWL_RANGE_TYPE);

        /*
         * Set sane default values
         */
        r->value = 0.0;
        r->min_val = 0.0;
        r->max_val = 100.0;
        r->step = 10.0;
        r->invert = FALSE;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 *
 * @param r: the range whose value will be changed
 * @param v: the new value of the range, checked against the valid range
 * @return Returns no value.
 * @brief Set the value of pointer of the range
 */
void
ewl_range_value_set(Ewl_Range *r, double v)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(r);
        DCHECK_TYPE(r, EWL_RANGE_TYPE);

        if (r->value == v)
                DRETURN(DLEVEL_STABLE);

        if (!r->unknown_range) {
                if (v < r->min_val)
                        r->value = r->min_val;
                else if (v > r->max_val)
                        r->value = r->max_val;
                else
                        r->value = v;
        }

        ewl_callback_call(EWL_WIDGET(r), EWL_CALLBACK_VALUE_CHANGED);
        ewl_widget_configure(EWL_WIDGET(r));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param r: the ranges to retrieve the value
 * @return Returns 0 on failure, the value of the range on success.
 * @brief Retrieve the current value of the range
 */
double
ewl_range_value_get(Ewl_Range *r)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(r, 0.0);
        DCHECK_TYPE_RET(r, EWL_RANGE_TYPE, 0.0);

        DRETURN_FLOAT(r->value, DLEVEL_STABLE);
}

/**
 * @param r: the range to change the minimum possible value
 * @param minv: the new minimum possible value for @a r
 * @return Returns no value.
 * @brief Set the minimum value possible for a range
 *
 * Sets the smallest value that @a r can obtain to @a minv.
 */
void
ewl_range_minimum_value_set(Ewl_Range *r, double minv)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(r);
        DCHECK_TYPE(r, EWL_RANGE_TYPE);

        r->min_val = minv;

        /* update to the min value if needed */
        if (r->value < r->min_val)
                ewl_range_value_set(r, r->min_val);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param r: the range to retrieve minimum value
 * @brief Retrieves the minimum value for the range.
 * @return Returns the currently set minimum value for the specified range.
 */
double
ewl_range_minimum_value_get(Ewl_Range *r)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(r, 0.0);
        DCHECK_TYPE_RET(r, EWL_RANGE_TYPE, 0.0);

        DRETURN_FLOAT(r->min_val, DLEVEL_STABLE);
}

/**
 * @param r: the range to change the maximum possible value
 * @param maxv: the new maximum possible value for @a r
 * @return Returns no value.
 * @brief Set the maximum value possible for a range
 *
 * Sets the smallest value that @a r can obtain to @a maxv.
 */
void
ewl_range_maximum_value_set(Ewl_Range *r, double maxv)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(r);
        DCHECK_TYPE(r, EWL_RANGE_TYPE);

        r->max_val = maxv;

        /* update to max value if needed */
        if (r->value > r->max_val)
                ewl_range_value_set(r, r->max_val);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param r: the range to retrieve maximum value
 * @brief Retrieves the maximum value for the range.
 * @return Returns the currently set maximum value for the specified range.
 */
double
ewl_range_maximum_value_get(Ewl_Range *r)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(r, 0.0);
        DCHECK_TYPE_RET(r, EWL_RANGE_TYPE, 0.0);

        DRETURN_FLOAT(r->max_val, DLEVEL_STABLE);
}
/**
 * @param r: the range to change step
 * @param step: the new step value for the range
 * @return Returns no value.
 * @brief Set the steps between increments
 *
 * Changes the amount that each increment or decrement changes the value of the
 * range @a r.
 */
void
ewl_range_step_set(Ewl_Range *r, double step)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(r);
        DCHECK_TYPE(r, EWL_RANGE_TYPE);

        if (step > r->max_val - r->min_val)
                step = r->max_val - r->min_val;

        r->step = step;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param r: the range to retrieve step size
 * @return Returns the step size of the range @a r.
 * @brief Retrieve the step size of the range
 */
double
ewl_range_step_get(Ewl_Range *r)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(r, 0.0);
        DCHECK_TYPE_RET(r, EWL_RANGE_TYPE, 0.0);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
        DRETURN_FLOAT(r->step, DLEVEL_STABLE);
}

/**
 * @param r: the range to set invert property
 * @param invert: the new value for the ranges invert property
 * @return Returns no value.
 * @brief Changes the invert property on the range for inverting it's scale.
 */
void
ewl_range_invert_set(Ewl_Range *r, unsigned int invert)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(r);
        DCHECK_TYPE(r, EWL_RANGE_TYPE);

        if (r->invert != !!invert)
        {
                r->invert = !!invert;
                ewl_widget_configure(EWL_WIDGET(r));
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param r: the range to retrieve invert property value
 * @return Returns the current value of the invert property in the range.
 * @brief Retrieve the current invert value from a range.
 */
unsigned int
ewl_range_invert_get(Ewl_Range *r)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(r, FALSE);
        DCHECK_TYPE_RET(r, EWL_RANGE_TYPE, FALSE);

        DRETURN_INT(r->invert, DLEVEL_STABLE);
}

/**
 * @param r: range to set unknown property
 * @param unknown: the new value for the ranges unknown property
 * @return Returns no value.
 * @brief Change the unknown property of the range if you do not know the max/min values.
 */
void
ewl_range_unknown_set(Ewl_Range *r, unsigned int unknown)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(r);
        DCHECK_TYPE(r, EWL_RANGE_TYPE);

        if (r->unknown_range != !!unknown) {
                r->unknown_range = !!unknown;
                ewl_widget_configure(EWL_WIDGET(r));
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param r: the range to retrieve the unknown property value
 * @return Returns the current value of the unknown property in the range.
 * @brief Retrieve the current unknown value from a range.
 */
unsigned int
ewl_range_unknown_get(Ewl_Range *r)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(r, FALSE);
        DCHECK_TYPE_RET(r, EWL_RANGE_TYPE, FALSE);

        DRETURN_INT(r->unknown_range, DLEVEL_STABLE);
}


/**
 * @param r: the range to increase
 * @return Returns no value.
 * @brief Increase the value of a range by it's step size
 *
 * Increases the value of the range @a r by one increment of it's step size.
 */
void
ewl_range_increase(Ewl_Range *r)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(r);
        DCHECK_TYPE(r, EWL_RANGE_TYPE);

        ewl_range_value_set(r, r->value + r->step);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param r: the range to decrease
 * @return Returns no value.
 * @brief Decrease the value of a range by it's step size
 *
 * Decreases the value of the range @a r by one increment of it's step size.
 */
void
ewl_range_decrease(Ewl_Range *r)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(r);
        DCHECK_TYPE(r, EWL_RANGE_TYPE);

        ewl_range_value_set(r, r->value - r->step);
        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

