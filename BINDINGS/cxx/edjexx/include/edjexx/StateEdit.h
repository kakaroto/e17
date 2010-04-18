#ifndef STATE_EDIT_H
#define STATE_EDIT_H

/* STD */
#include <string>

/* Project */
#include "Edit.h"

namespace Edjexx {

class StateEdit
{
public:
  /*!
   * Set a new name for the given state in the given part.
   * Note that state and new_name must include the floating value inside the string (ex. "default 0.00")
   *
   * @param newName The new name to assign (including the value)
   *
   * @return true on success, false on failure
   */
  bool setName (const std::string &newName, double newValue);
  
  const std::string getName () const;
  
  double getValue () const;
  
  /*!
   * Get the rel1 relative x value of state
   *
   * @return The 'rel1 relative X' value of the part state
   */
  double getXRelativeRel1 () const;
  
  /*!
   * Get the rel1 relative y value of state
   *
   * @return The 'rel1 relative Y' value of the part state
   */
  double getYRelativeRel1 () const;

  /*!
   * Get the rel2 relative x value of state
   *
   * @return The 'rel2 relative X' value of the part state
   */
  double getXRelativeRel2 () const;
  
  /*!
   * Get the rel2 relative y value of state
   *
   * @return The 'rel2 relative Y' value of the part state
   */
  double getYRelativeRel2 () const;

  /*!
   * Set the rel1 relative x value of state
   *
   * @param x The new 'rel1 relative X' value to set
   */
  void setXRelativeRel1 (double x);

  /*!
   * Set the rel1 relative y value of state
   *
   * @param y The new 'rel1 relative Y' value to set
   */
  void setYRelativeRel1 (double y);

  /*!
   * Set the rel2 relative x value of state
   *
   * @param x The new 'rel2 relative X' value to set
   */
  void setXRelativeRel2 (double x);

  /*!
   * Set the rel2 relative y value of state
   *
   * @param y The new 'rel2 relative Y' value to set
   */
  void setYRelativeRel2 (double y);
  
#if 0
  
  /**Get the rel1 offset y value of state*/
  EAPI int                   /// @return The 'rel1 offset Y' value of the part state
  edje_edit_state_rel1_offset_y_get(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state       ///< The name of the 'part state' (ex. "default 0.00")
  );
  /**Get the rel2 offset x value of state*/
  EAPI int                   /// @return The 'rel2 offset X' value of the part state
  edje_edit_state_rel2_offset_x_get(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state       ///< The name of the 'part state' (ex. "default 0.00")
  );
  /**Get the rel2 offset y value of state*/
  EAPI int                   /// @return The 'rel2 offset Y' value of the part state
  edje_edit_state_rel2_offset_y_get(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state       ///< The name of the 'part state' (ex. "default 0.00")
  );

  /**Set the rel1 offset x value of state*/
  EAPI void
  edje_edit_state_rel1_offset_x_set(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
     double x                ///< The new 'rel1 offset X' value to set
  );
  /**Get the rel1 offset y value of state*/
  EAPI void
  edje_edit_state_rel1_offset_y_set(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
     double y                ///< The new 'rel1 offset Y' value to set
  );
  /**Get the rel2 offset x value of state*/
  EAPI void
  edje_edit_state_rel2_offset_x_set(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
     double x                ///< The new 'rel2 offset X' value to set
  );
  /**Get the rel2 offset y value of state*/
  EAPI void
  edje_edit_state_rel2_offset_y_set(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
     double y                ///< The new 'rel2 offset Y' value to set
  );

  /**Get the part name rel1x is relative to. The function return NULL if the part is relative to the whole interface.*/
  EAPI const char *          ///@return The name of the part to apply the relativity
  edje_edit_state_rel1_to_x_get(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state       ///< The name of the 'part state' (ex. "default 0.00")
  );
  /**Get the part name rel1y is relative to. The function return NULL if the part is relative to the whole interface.*/
  EAPI const char *          ///@return The name of the part to apply the relativity
  edje_edit_state_rel1_to_y_get(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state       ///< The name of the 'part state' (ex. "default 0.00")
  );
  /**Get the part name rel2x is relative to. The function return NULL if the part is relative to the whole interface.*/
  EAPI const char *         ///@return The name of the part to apply the relativity
  edje_edit_state_rel2_to_x_get(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state       ///< The name of the 'part state' (ex. "default 0.00")
  );
  /**Get the part name rel2y is relative to. The function return NULL if the part is relative to the whole interface.*/
  EAPI const char *         ///@return The name of the part to apply the relativity
  edje_edit_state_rel2_to_y_get(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state       ///< The name of the 'part state' (ex. "default 0.00")
  );

  /**Set the part rel1x is relative to. Set rel_to to NULL make the part relative to the whole interface.*/
  EAPI void
  edje_edit_state_rel1_to_x_set(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
     const char *rel_to      ///< The name of the part that is used as container/parent
  );
  /**Set the part rel1y is relative to. Set rel_to to NULL make the part relative to the whole interface.*/
  EAPI void
  edje_edit_state_rel1_to_y_set(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
     const char *rel_to      ///< The name of the part that is used as container/parent
  );
  /**Set the part rel2x is relative to. Set rel_to to NULL make the part relative to the whole interface.*/
  EAPI void
  edje_edit_state_rel2_to_x_set(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
     const char *rel_to      ///< The name of the part that is used as container/parent
  );
  /**Set the part rel2y is relative to. Set rel_to to NULL make the part relative to the whole interface.*/
  EAPI void
  edje_edit_state_rel2_to_y_set(
     Evas_Object *obj,       ///< The edje object
     const char *part,       ///< The name of the part
     const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
     const char *rel_to      ///< The name of the part that is used as container/parent
  );


  
/**Get the color of a part state. Pass NULL to any of [r,g,b,a] to get only the others.*/
EAPI void
edje_edit_state_color_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int *r,                 ///< A pointer to store the red value
   int *g,                 ///< A pointer to store the green value
   int *b,                 ///< A pointer to store the blue value
   int *a                  ///< A pointer to store the alpha value
);
/**Get the color2 of a part state. Pass NULL to any of [r,g,b,a] to get only the others.*/
EAPI void
edje_edit_state_color2_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int *r,                 ///< A pointer to store the red value
   int *g,                 ///< A pointer to store the green value
   int *b,                 ///< A pointer to store the blue value
   int *a                  ///< A pointer to store the alpha value
);
/**Get the color3 of a part state. Pass NULL to any of [r,g,b,a] to get only the others.*/
EAPI void
edje_edit_state_color3_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int *r,                 ///< A pointer to store the red value
   int *g,                 ///< A pointer to store the green value
   int *b,                 ///< A pointer to store the blue value
   int *a                  ///< A pointer to store the alpha value
);

/**Set the color of a part state. Pass -1 to any of [r,g,b,a] to leave the value untouched.*/
EAPI void
edje_edit_state_color_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int r,                  ///< The red value of the color
   int g,                  ///< The green value of the color
   int b,                  ///< The blue value of the color
   int a                   ///< The alpha value of the color
);
/**Set the color2 of a part state. Pass -1 to any of [r,g,b,a] to leave the value untouched.*/
EAPI void
edje_edit_state_color2_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int r,                  ///< The red value of the color
   int g,                  ///< The green value of the color
   int b,                  ///< The blue value of the color
   int a                   ///< The alpha value of the color
);
/**Set the color3 of a part state. Pass -1 to any of [r,g,b,a] to leave the value untouched.*/
EAPI void
edje_edit_state_color3_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int r,                  ///< The red value of the color
   int g,                  ///< The green value of the color
   int b,                  ///< The blue value of the color
   int a                   ///< The alpha value of the color
);

/**Get the align_x value of a part state.*/
EAPI double                ///@return The horizontal align value for the given state
edje_edit_state_align_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);
/**Get the align_y value of a part state.*/
EAPI double                ///@return The vertical align value for the given state
edje_edit_state_align_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);
/**Set the align_x value of a part state.*/
EAPI void
edje_edit_state_align_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double align            ///< The new horizontal align to set
);
/**Set the align_y value of a part state.*/
EAPI void
edje_edit_state_align_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double align            ///< The new vertical align to set
);
/**Get the min_w value of a part state.*/
EAPI int                   ///@return The minimum width of a part state
edje_edit_state_min_w_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);
/**Set the min_w value of a part state.*/
EAPI void
edje_edit_state_min_w_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int min_w               ///< The new minimum width to set for the part state
);
/**Get the min_h value of a part state.*/
EAPI int                   ///@return The minimum height of a part state
edje_edit_state_min_h_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);
/**Set the min_h value of a part state.*/
EAPI void
edje_edit_state_min_h_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int min_h               ///< The new minimum height to set for the part state
);

/**Get the max_w value of a part state.*/
EAPI int                   ///@return The maximum width of a part state
edje_edit_state_max_w_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);
/**Set the max_w value of a part state.*/
EAPI void
edje_edit_state_max_w_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int max_w               ///< The new maximum width to set for the part state
);
/**Get the max_h value of a part state.*/
EAPI int                   ///@return The maximum height of a part state
edje_edit_state_max_h_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);
/**Set the max_h value of a part state.*/
EAPI void
edje_edit_state_max_h_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int max_h               ///< The new maximum height to set for the part state
);

/**Get the minimum aspect value of a part state.*/
EAPI double                ///@return The aspect minimum value of a part state
edje_edit_state_aspect_min_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);
/**Get the maximum aspect value of a part state.*/
EAPI double                ///@return The aspect maximum value of a part state
edje_edit_state_aspect_max_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);
/**Set the minimum aspect value of a part state.*/
EAPI void
edje_edit_state_aspect_min_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double aspect           ///< The new minimum aspect value to set
);
/**Set the maximum aspect value of a part state.*/
EAPI void
edje_edit_state_aspect_max_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double aspect           ///< The new maximum aspect value to set
);
/**Get the aspect preference value of a part state.*/
EAPI unsigned char         ///@return The aspect preference (0=none, 1=vertical, 2=horizontal, 3=both)
edje_edit_state_aspect_pref_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);
/**Set the aspect preference value of a part state.*/
EAPI void
edje_edit_state_aspect_pref_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   unsigned char pref      ///< The new aspect preference to set (0=none, 1=vertical, 2=horizontal, 3=both)
);

/**Get the fill origin relative x value of a part state.*/
EAPI double                ///@return The fill offset x relative to area
edje_edit_state_fill_origin_relative_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the fill origin relative y value of a part state.*/
EAPI double                ///@return The fill origin y relative to area
edje_edit_state_fill_origin_relative_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the fill origin offset x value of a part state.*/
EAPI int                   ///@return The fill origin offset x relative to area
edje_edit_state_fill_origin_offset_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the fill origin offset y value of a part state.*/
EAPI int                   ///@return The fill origin offset y relative to area
edje_edit_state_fill_origin_offset_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set the fill origin relative x value of a part state.*/
EAPI void
edje_edit_state_fill_origin_relative_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double x                ///< The new value to set
);

/**Set the fill origin relative y value of a part state.*/
EAPI void
edje_edit_state_fill_origin_relative_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double x                ///< The new value to set
);

/**Set the fill origin offset x value of a part state.*/
EAPI void
edje_edit_state_fill_origin_offset_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double x                ///< The new value to set
);

/**Set the fill origin offset x value of a part state.*/
EAPI void
edje_edit_state_fill_origin_offset_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double y                ///< The new value to set
);

/**Get the fill size relative x value of a part state.*/
EAPI double                ///@return The fill size offset x relative to area
edje_edit_state_fill_size_relative_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the fill size relative y value of a part state.*/
EAPI double                ///@return The fill size y relative to area
edje_edit_state_fill_size_relative_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the fill size offset x value of a part state.*/
EAPI int                    ///@return The fill size offset x relative to area
edje_edit_state_fill_size_offset_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the fill size offset y value of a part state.*/
EAPI int                    ///@return The fill size offset y relative to area
edje_edit_state_fill_size_offset_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set the fill size relative x value of a part state.*/
EAPI void
edje_edit_state_fill_size_relative_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double x                ///< The new value to set
);

/**Set the fill size relative y value of a part state.*/
EAPI void
edje_edit_state_fill_size_relative_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double x                ///< The new value to set
);

/**Set the fill size offset x value of a part state.*/
EAPI void
edje_edit_state_fill_size_offset_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double x                ///< The new value to set
);

/**Set the fill size offset x value of a part state.*/
EAPI void
edje_edit_state_fill_size_offset_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double y                ///< The new value to set
);
#endif

/*!
 * Get the visibility of a part state.
 *
 * @return true if the state is visible
 */
bool getVisibility ();

/*!
 * Set the visibility of a part state.
 *
 * @param visible true to set the state visible
 */
void setVisibility (bool visible);
  
#if 0
/**Get the color class of the given part state. Remember to free the string with edje_edit_string_free()*/
EAPI const char*           ///@return The current color_class of the part state
edje_edit_state_color_class_get(
   Evas_Object *obj,      ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set the color class for the given part state.*/
EAPI void
edje_edit_state_color_class_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   const char *color_class ///< The new color_class to assign
);

#endif
  
  void copyFrom (StateEdit &state);
  
protected:
  StateEdit (Edit &edit, const std::string &part, const std::string &state, double value);
  
  Edit *mEdit;
  std::string mPart;
  std::string mState;
  double mValue;
  
private:
  
};

} // end namespace Edjexx

#endif // STATE_EDIT_H
