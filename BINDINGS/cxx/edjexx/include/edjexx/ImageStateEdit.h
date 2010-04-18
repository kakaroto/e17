#ifndef IMAGE_STATE_EDIT_H
#define IMAGE_STATE_EDIT_H

#include "StateEdit.h"

namespace Edjexx {

class ImageStateEdit : public StateEdit
{
public:
  ImageStateEdit (Edit &edit, const std::string &part, const std::string &state, double value);
  
  /*! 
   * Get normal image name for a given part state.
   *
   * @param part The name of the part
   * @param state The name of the 'part state' (ex. "default 0.00")
   */
  std::string getImage ();

  /*!
   * Set normal image for a given part state.
   * 
   * @param part The name of the part
   * @param state The name of the 'part state' (ex. "default 0.00")
   * @param image The name of the image for the given state (must be an image contained in the edje file)
   */
  void setImage (const std::string &image);
  
#if 0
/**Get the image border of a part state. Pass NULL to any of [r,g,b,a] to get only the others.*/
EAPI void
edje_edit_state_image_border_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int *l,                 ///< A pointer to store the left value
   int *r,                 ///< A pointer to store the right value
   int *t,                 ///< A pointer to store the top value
   int *b                  ///< A pointer to store the bottom value
);

/**Set the image border of a part state. Pass -1 to any of [l,r,t,b] to leave the value untouched.*/
EAPI void
edje_edit_state_image_border_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int l,                  ///< The new left border (or -1)
   int r,                  ///< The new right border (or -1)
   int t,                  ///< The new top border (or -1)
   int b                   ///< The new bottom border (or -1)
);

/**Get if the image center should be draw. 1 means to draw the center, 0 to don't draw it.*/
EAPI unsigned char         ///@return 1 if the center of the bordered image is draw
edje_edit_state_image_border_fill_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set if the image center should be draw. 1 means to draw the center, 0 to don't draw it.*/
EAPI void
edje_edit_state_image_border_fill_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   unsigned char fill      ///< If set to 0 the image center isn't draw
);

/**Get the list of all the tweens images in the given part state.
 * Use edje_edit_string_list_free() when you don't need it anymore.
 */
EAPI Eina_List *           ///@return A string list containing all the image name that form a tween animation in the given part state
edje_edit_state_tweens_list_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Add a new tween frame to the given part state
 * The tween param must be the name of an existing image.
 */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise
edje_edit_state_tween_add(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   const char *tween       ///< The name of the image to add.
);

/**Remove the first tween with the given name.
 * If none is removed the function return 0.
 * The image is not removed from the edje.
 */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise
edje_edit_state_tween_del(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   const char *tween       ///< The name of the image to remove from the tween list.
);
#endif
  
  //void copyFrom (StateEdit &state);
};

} // end namespace Edjexx

#endif // IMAGE_STATE_EDIT_H
