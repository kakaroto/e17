#ifndef GRADIENT_STATE_EDIT_H
#define GRADIENT_STATE_EDIT_H

#include "StateEdit.h"

namespace Edjexx {

class GradientStateEdit : public StateEdit
{
public:
  GradientStateEdit (Edit &edit, const std::string &part, const std::string &state, double value);
  
#if 0
/**Get the type of gradient. Remember to free the string with edje_edit_string_free().*/
EAPI const char *          ///@return The type of gradient used in state
edje_edit_state_gradient_type_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set the type of gradient.
 * Gradient type can be on of the following: linear, linear.diag, linear.codiag, radial, rectangular, angular, sinusoidal
 */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise
edje_edit_state_gradient_type_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   const char *type        ///< The type of gradient to use
);

/**Get if the current gradient use the fill properties or the gradient_rel as params.*/
EAPI Eina_Bool         ///@return 1 if use fill, 0 if use gradient_rel
edje_edit_state_gradient_use_fill_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the spectra used by part state. Remember to free the string with edje_edit_string_free()*/
EAPI const char *          ///@return The spectra name used in state
edje_edit_state_gradient_spectra_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set the spectra used by part state.*/
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise
edje_edit_state_gradient_spectra_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   const char* spectra     ///< The spectra name to assign
);

/**Get the angle of the gradient.*/
EAPI int                   ///@return The angle of the gradient
edje_edit_state_gradient_angle_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set the angle of the gradient.*/
EAPI void
edje_edit_state_gradient_angle_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int angle               ///< The angle to set
);

/**Get the gradien rel1 relative x value */
EAPI double                ///@return The gradien rel1 relative x value
edje_edit_state_gradient_rel1_relative_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the gradien rel1 relative y value */
EAPI double                ///@return The gradien rel1 relative y value
edje_edit_state_gradient_rel1_relative_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the gradien rel2 relative x value */
EAPI double                ///@return The gradien rel2 relative x value
edje_edit_state_gradient_rel2_relative_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the gradien rel2 relative y value */
EAPI double                ///@return The gradien rel2 relative y value
edje_edit_state_gradient_rel2_relative_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set the gradien rel1 relative x value */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise
edje_edit_state_gradient_rel1_relative_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double val
);

/**Set the gradien rel1 relative y value */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise
edje_edit_state_gradient_rel1_relative_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double val
);

/**Set the gradien rel2 relative x value */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise
edje_edit_state_gradient_rel2_relative_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double val
);

/**Set the gradien rel2 relative y value */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise
edje_edit_state_gradient_rel2_relative_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double val
);

/**Get the gradien rel1 offset x value */
EAPI int                   ///@return The gradient rel1 offset x value
edje_edit_state_gradient_rel1_offset_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the gradien rel1 offset y value */
EAPI int                   ///@return The gradient rel1 offset y value
edje_edit_state_gradient_rel1_offset_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the gradien rel2 offset x value */
EAPI int                   ///@return The gradient rel2 offset x value
edje_edit_state_gradient_rel2_offset_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the gradien rel2 offset y value */
EAPI int                   ///@return The gradient rel2 offset y value
edje_edit_state_gradient_rel2_offset_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set the gradien rel1 offset x value */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise
edje_edit_state_gradient_rel1_offset_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int val
);

/**Set the gradien rel1 offset y value */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise
edje_edit_state_gradient_rel1_offset_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int val
);

/**Set the gradien rel2 offset x value */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise
edje_edit_state_gradient_rel2_offset_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int val
);

/**Set the gradien rel2 offset y value */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise
edje_edit_state_gradient_rel2_offset_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int val
);

#endif
};

} // end namespace Edjexx

#endif // GRADIENT_STATE_EDIT_H