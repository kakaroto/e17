#ifndef TEXT_STATE_EDIT_H
#define TEXT_STATE_EDIT_H

#include "StateEdit.h"

namespace Edjexx {

class TextStateEdit : public StateEdit
{
public:
  TextStateEdit (Edit &edit, const std::string &part, const std::string &state, double value);

#if 0
  
/**Get the text of a part state. Remember to free the returned string with edje_edit_string_free(). */
EAPI const char *          ///@return A newly allocated string containing the text for the given state
edje_edit_state_text_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set the text of a part state.*/
EAPI void
edje_edit_state_text_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   const char *text        ///< The new text to assign
);

/**Get the text size of a part state. The function will return -1 on errors.*/
EAPI int                   ///@return The font size in pixel or -1 on errors.
edje_edit_state_text_size_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set the text size of a part state.*/
EAPI void
edje_edit_state_text_size_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   int size                ///< The new font size to set (in pixel)
);

/**Get the text horizontal align of a part state. The value range is from 0.0(right) to 1.0(left)*/
EAPI double                ///@return The text align X value
edje_edit_state_text_align_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Get the text vertical align of a part state. The value range is from 0.0(top) to 1.0(bottom)*/
EAPI double                ///@return The text align Y value
edje_edit_state_text_align_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set the text horizontal align of a part state. The value range is from 0.0(right) to 1.0(left)*/
EAPI void
edje_edit_state_text_align_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double align            ///< The new text align X value
);

/**Set the text vertical align of a part state. The value range is from 0.0(top) to 1.0(bottom)*/
EAPI void
edje_edit_state_text_align_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double align            ///< The new text align Y value
);

/**Get the text elipsis of a part state. The value range is from 0.0(right) to 1.0(left)*/
EAPI double                ///@return The text elipsis value
edje_edit_state_text_elipsis_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set the text vertical align of a part state. The value range is from 0.0(right) to 1.0(left)*/
EAPI void
edje_edit_state_text_elipsis_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   double balance          ///< The position where to cut the string
);

/**Get if the text part fit it's container horizontally */
EAPI Eina_Bool         ///@return 1 If the part fit it's container horizontally
edje_edit_state_text_fit_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set if the text part should fit it's container horizontally */
EAPI void
edje_edit_state_text_fit_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   Eina_Bool fit       ///< 1 to make the text fit it's container
);

/**Get if the text part fit it's container vertically */
EAPI Eina_Bool         ///@return 1 If the part fit it's container vertically
edje_edit_state_text_fit_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set if the text part should fit it's container vertically */
EAPI void
edje_edit_state_text_fit_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   Eina_Bool fit       ///< 1 to make the text fit it's container
);

/**Get the list of all the fonts in the given edje.
 * Use edje_edit_string_list_free() when you don't need the list anymore.
 */
EAPI Eina_List *          ///@return An Eina_List* of string (char *)containing all the fonts names found in the edje file.
edje_edit_fonts_list_get(
   Evas_Object *obj       ///< The edje object
);

/**Add a new ttf font to the edje file.
 * The newly created font will be available to all the groups in the edje, not only the current one.
 * If font can't be load FALSE is returned.
 */
EAPI Eina_Bool         ///@return TRUE on success or FALSE on failure
edje_edit_font_add(
   Evas_Object *obj,       ///< The edje object
   const char* path        ///< The file path to load the ttf font from
);

/**Get font name for a given part state. Remember to free the returned string using edje_edit_string_free().*/
EAPI const char *          ///@return The name of the font used in the given part state
edje_edit_state_font_get(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the 'part state' (ex. "default 0.00")
);

/**Set font name for a given part state. */
EAPI void
edje_edit_state_font_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state,      ///< The name of the 'part state' (ex. "default 0.00")
   const char *font        ///< The name of the font to use in the given part state
);

#endif
  
};

} // end namespace Edjexx

#endif // TEXT_STATE_EDIT_H
